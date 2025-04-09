#include <iostream>
#include <functional>

template <typename T, typename Func>
class my_weak_ptr;

template <typename T, typename Func>
class control_block
{
public:
    control_block(T *ptr, Func deleter) : ptr_(ptr),
                                          deleter_(deleter),
                                          strong_count_(1),
                                          weak_count_(0) {}

    void increment_strong() { strong_count_ += 1; }
    void decrement_strong() { strong_count_ -= 1; }
    std::size_t strong_count() const noexcept { return strong_count_; }

    void increment_weak() { weak_count_ += 1; }
    void decrement_weak() { weak_count_ -= 1; }
    std::size_t weak_count() const noexcept { return weak_count_; }

    void on_zero_strong_count()
    {
        if (!ptr_)
            return;
        deleter_(ptr_);
        ptr_ = nullptr;
    }

    T *get() const noexcept { return ptr_; }

    ~control_block()
    {
    }

private:
    T *ptr_;
    Func deleter_;
    std::size_t strong_count_;
    std::size_t weak_count_;
};

template <typename T, typename Func = std::function<void(T *)>>
class my_shared_ptr
{
public:
    // constructor
    explicit my_shared_ptr(
        T *ptr = nullptr,
        Func deleter = [](T *p)
        { delete p; }) : control_block_(ptr ? new control_block<T, Func>(ptr, deleter) : nullptr),
                         stored_ptr_(ptr)
    {
    }

    // Aliasing constructor
    template <typename U>
    my_shared_ptr(const my_shared_ptr<U, Func> &other, T *ptr)
        : control_block_(other.control_block_),
          stored_ptr_(ptr)
    {
        if (control_block_)
            control_block_->increment_strong();
    }

    // Update copy constructor
    my_shared_ptr(const my_shared_ptr &oth)
        : control_block_(oth.control_block_),
          stored_ptr_(oth.stored_ptr_)
    {
        if (control_block_)
            control_block_->increment_strong();
    }

    // Update move constructor
    my_shared_ptr(my_shared_ptr &&oth) noexcept
        : control_block_(oth.control_block_),
          stored_ptr_(oth.stored_ptr_)
    {
        oth.control_block_ = nullptr;
        oth.stored_ptr_ = nullptr;
    }

    // copy assignment operator
    my_shared_ptr &operator=(const my_shared_ptr &oth)
    {
        if (this != &oth)
        { // Add self-assignment check
            release();
            control_block_ = oth.control_block_;
            stored_ptr_ = oth.stored_ptr_;
            if (control_block_)
                control_block_->increment_strong();
        }
        return *this;
    }

    // move assignment operator
    my_shared_ptr &operator=(my_shared_ptr &&oth) noexcept
    {
        release();
        control_block_ = oth.control_block_;
        stored_ptr_ = oth.stored_ptr_;
        oth.control_block_ = nullptr;
        oth.stored_ptr_ = nullptr;
        return *this;
    }

    my_shared_ptr(const my_weak_ptr<T, Func> &weak_ptr) : control_block_(weak_ptr.control_block_)
    {
        if (weak_ptr.expired())
            return;

        if (control_block_)
            control_block_->increment_strong();
    }

    // destructor
    ~my_shared_ptr()
    {
        release();
    }

    T &operator*() const
    {
        if (!stored_ptr_)
            throw std::runtime_error("Dereferencing null pointer");
        return *stored_ptr_;
    }

    T *operator->() const
    {
        if (!stored_ptr_)
            throw std::runtime_error("Dereferencing null pointer");
        return stored_ptr_;
    }

    T *get() const { return stored_ptr_; }

    std::size_t use_count() { return control_block_ ? control_block_->strong_count() : 0; }

    void reset(T *ptr = nullptr, Func deleter = [](T *p)
                                 { delete p; })
    {
        release();
        control_block_ = ptr ? new control_block<T, Func>(ptr, deleter) : nullptr;
        stored_ptr_ = ptr;
    }

private:
    void release()
    {
        if (!control_block_)
            return;

        control_block_->decrement_strong();

        if (0 == control_block_->strong_count())
        {
            control_block_->on_zero_strong_count();

            if (0 == control_block_->weak_count())
            {
                delete control_block_;
                control_block_ = nullptr;
            }
        }
    }

    control_block<T, Func> *control_block_;
    T *stored_ptr_; // Add this new member

    template <typename U, typename UDeleter>
    friend class my_shared_ptr;

    friend class my_weak_ptr<T, Func>;
};

template <typename T, typename Func = std::function<void(T *)>>
class my_weak_ptr
{
public:
    // Add default constructor
    my_weak_ptr() : control_block_(nullptr) {}

    my_weak_ptr(const my_shared_ptr<T, Func> &my_shared_ptr) : control_block_(my_shared_ptr.control_block_)
    {
        if (control_block_)
            control_block_->increment_weak();
    }

    // copy constructor
    my_weak_ptr(const my_weak_ptr &oth) : control_block_(oth.control_block_)
    {
        if (control_block_)
            control_block_->increment_weak();
    }

    // copy assignment operator
    my_weak_ptr &operator=(const my_weak_ptr &oth)
    {
        release();
        control_block_ = oth.control_block_;
        if (control_block_)
            control_block_->increment_weak();
        return *this;
    }

    // move constructor
    my_weak_ptr(my_weak_ptr &&oth) : control_block_(oth.control_block_)
    {
        oth.control_block_ = nullptr;
    }

    // move assignment
    my_weak_ptr &operator=(my_weak_ptr &&oth)
    {
        if (this == &oth)
            return *this;

        release();
        control_block_ = oth.control_block_;
        oth.control_block_ = nullptr;
        return *this;
    }

    ~my_weak_ptr()
    {
        release();
    }

    my_shared_ptr<T, Func> lock()
    {
        return my_shared_ptr<T, Func>(*this);
    }

    bool expired()
    {
        return !control_block_ || (0 == control_block_->strong_count());
    }

    std::size_t use_count() const
    {
        return control_block_ ? control_block_->strong_count() : 0;
    }

private:
    void release()
    {
        if (!control_block_)
            return;

        control_block_->decrement_weak();
        if (0 == control_block_->strong_count() && 0 == control_block_->weak_count())
            delete control_block_;
        control_block_ = nullptr;
    }

    control_block<T, Func> *control_block_;

    friend class my_shared_ptr<T, Func>;
};