#include <utility>
#include <iostream>
#include "stack_impl.cpp"

template <typename T>
class stack : private stack_impl<T>
{
public:
    stack(size_t size = 0) : stack_impl<T>(size) {}

    // copy construtor
    stack(const stack &oth) : stack_impl<T>(oth.size_)
    {
        while (count() < oth.count())
            push(oth.v_[count()]);
    }

    // copy assignment
    stack &operator=(const stack &oth)
    {
        if (this != &oth)
        {
            stack temp(oth);
            swap(*this, temp);
        }
        return *this;
    }

    // move constructor
    stack(stack &&oth) noexcept : stack_impl<T>(0)
    {
        swap(*this, oth);
    }

    // move assignment
    stack &operator=(stack &&oth) noexcept
    {
        if (this != &oth)
        {
            stack<T> temp(std::move(oth));
            swap(*this, temp);
        }
        return *this;
    }

    friend void swap(stack &lhs, stack &rhs)
    {
        std::swap(lhs.v_, rhs.v_);
        std::swap(lhs.size_, rhs.size_);
        std::swap(lhs.used_, rhs.used_);
    }

    template <typename... Args>
    void push(const Args &...args)
    {
        if (this->size_ == this->used_)
        {
            stack temp(this->size_ * 2 + 1);

            while (temp.count() < this->used_)
                temp.push(this->v_[temp.count()]);
            temp.push(args...);
            swap(*this, temp);
        }
        else
        {
            this->v_[this->used_] = T(args...);
            ++(this->used_);
        }
    }

    void pop()
    {
        if (0 == count())
            throw "empty stack";
        stack_impl<T>::destroy(this->v_ + this->used_);
        --(this->used_);
    }

    const T &top()
    {
        if (empty())
            throw "empty stack";

        return this->v_[this->used_ - 1];
    }

    size_t count() const noexcept { return this->used_; };

    size_t capacity() noexcept { return this->size_; };

    bool empty() const noexcept { return this->used_ == 0; };
    bool full() const noexcept { return this->used_ == this->size_; };

    // required for BOOST testing
    friend bool operator==(const stack &lhs, const stack &rhs)
    {
        if (lhs.count() != rhs.count())
            return false;

        for (size_t i = 0; i < lhs.count(); ++i)
        {
            if (lhs.v_[i] != rhs.v_[i])
                return false;
        }
        return true;
    }

    // required for BOOST testing
    friend std::ostream &operator<<(std::ostream &os, const stack &s)
    {
        os << "stack(";
        for (size_t i = 0; i < s.count(); ++i)
        {
            os << s.v_[i];
            if (i < s.count() - 1)
                os << ", ";
        }
        os << ")";
        return os;
    }
};
