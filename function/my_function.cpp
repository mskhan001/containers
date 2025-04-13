#include <iostream>
#include <memory>
#include <utility>

class bad_function_call : public std::exception
{
public:
    const char *what() const noexcept override
    {
        return "bad_function_call";
    }
};

template <typename R, typename... Args>
class AbstractInvoker
{
public:
    virtual R operator()(Args... args) = 0;
    virtual AbstractInvoker<R, Args...> *clone() const = 0; // added clone method for copying
    virtual ~AbstractInvoker() = default;                   // virtual destructor
};

template <typename R, typename Func, typename... Args>
class AbstractInvokerDerived : public AbstractInvoker<R, Args...>
{
public:
    AbstractInvokerDerived(Func func) : func_(func) {}
    AbstractInvokerDerived(const AbstractInvokerDerived &other) : func_(other.func_) {} // copy ctor

    R operator()(Args... args) override { return func_(args...); }

    AbstractInvoker<R, Args...> *clone() const override
    {
        return new AbstractInvokerDerived(*this);
    }

private:
    Func func_;
};

template <typename...>
class my_function;

template <typename R, typename... Args>
class my_function<R(Args...)>
{
public:
    my_function() {}

    template <typename Func>
    explicit my_function(Func &&func)
        : func_(std::make_unique<AbstractInvokerDerived<R, Func, Args...>>(std::forward<Func>(func)))
    {
    }

    // Copy constructor using clone()
    my_function(const my_function &other)
        : func_(other.func_ ? std::unique_ptr<AbstractInvoker<R, Args...>>(other.func_->clone()) : nullptr)
    {
    }

    // Copy assignment operator using clone()
    my_function &operator=(const my_function &other)
    {
        if (this != &other)
            func_ = other.func_ ? std::unique_ptr<AbstractInvoker<R, Args...>>(other.func_->clone()) : nullptr;
        return *this;
    }

    // Defaulted move semantics
    my_function(my_function &&) noexcept = default;
    my_function &operator=(my_function &&) noexcept = default;

    R operator()(Args... args)
    {
        if (!func_)
            throw bad_function_call();

        return (*func_)(args...);
    }

private:
    std::unique_ptr<AbstractInvoker<R, Args...>> func_;
};