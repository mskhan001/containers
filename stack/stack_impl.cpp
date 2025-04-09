#include <utility>

/*
    repnsible for
        1. raw memory management
        2. object destruction
*/
template <typename T>
class stack_impl
{
protected:
    // constructor
    stack_impl(size_t);

    // move operations
    stack_impl(stack_impl &&);
    stack_impl &operator=(stack_impl &&);

    // destructor
    ~stack_impl();

    // swap two stack_impl
    friend void swap(stack_impl &lhs, stack_impl &rhs) noexcept;

    // constructs an object using placement-new
    template <typename... Args>
    void construct(T *ptr, const Args &...args);

    // calls destructor on an object
    void destroy(T *ptr) noexcept;

    template <typename Iter>
    void destroy(Iter first, Iter last) noexcept;

    T *v_;        // pointer to memory area
    size_t size_; // size of memory area
    size_t used_; // no of elements
};

template <typename T>
stack_impl<T>::stack_impl(size_t size) : v_(static_cast<T *>(size == 0 ? nullptr : operator new(size * sizeof(T)))), // allocate raw memory
                                         size_(size),
                                         used_(0)
{
}

template <typename T>
void swap(stack_impl<T> &lhs, stack_impl<T> &rhs) noexcept
{
    std::swap(lhs.v_, rhs.v_);
    std::swap(lhs.size_, rhs.size_);
    std::swap(lhs.used_, rhs.used_);
}

template <typename T>
stack_impl<T>::stack_impl(stack_impl<T> &&oth) : stack_impl(0)
{
    swap(*this, oth);
}

template <typename T>
stack_impl<T> &stack_impl<T>::operator=(stack_impl<T> &&oth)
{
    stack_impl temp{std::move(oth)};
    swap(*this, temp);
    return *this;
}

template <typename T>
template <typename... Args>
void stack_impl<T>::construct(T *ptr, const Args &...args)
{
    new (ptr) T(args...);
}

template <typename T>
void stack_impl<T>::destroy(T *ptr) noexcept
{
    ptr->~T();
}

template <typename T>
template <typename Iter>
void stack_impl<T>::destroy(Iter first, Iter last) noexcept
{
    while (first != last)
    {
        destroy(&*first);
        ++first;
    }
}

template <typename T>
stack_impl<T>::~stack_impl()
{
    destroy(v_, v_ + used_);
    operator delete(v_);
}