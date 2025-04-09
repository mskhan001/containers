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
    stack_impl(size_t size) : v_(size == 0 ? nullptr : static_cast<T *>(operator new(size * sizeof(T)))),
                              size_(size),
                              used_(0) {}

    // move constructor
    stack_impl(stack_impl &&oth) noexcept : v_(oth.v_), size_(oth.size_), used_(oth.used_)
    {
        oth.v_ = nullptr;
        oth.size_ = oth.used_ = 0;
    }

    // move assignment
    stack_impl &operator=(stack_impl &&oth) noexcept
    {
        if (this != &oth)
        {
            stack_impl temp(std::move(oth));
            swap(*this, temp);
        }
        return *this;
    }

    // destructor
    ~stack_impl()
    {
        if (v_)
        {
            destroy(v_, v_ + used_);
            operator delete(v_);
        }
    }

    // swap two stack_impl
    friend void swap(stack_impl &lhs, stack_impl &rhs) noexcept
    {
        std::swap(lhs.v_, rhs.v_);
        std::swap(lhs.size_, rhs.size_);
        std::swap(lhs.used_, rhs.used_);
    }

    // constructs an object using placement-new
    template <typename... Args>
    void construct(T *ptr, const Args &...args)
    {
        new (ptr) T(args...);
    }

    // calls destructor on an object
    void destroy(T *ptr) noexcept
    {
        ptr->~T();
    }

    template <typename Iter>
    void destroy(Iter first, Iter last) noexcept
    {
        while (first != last)
        {
            destroy(&*first);
            ++first;
        }
    }

    T *v_;        // pointer to memory area
    size_t size_; // size of memory area
    size_t used_; // no of elements
};
