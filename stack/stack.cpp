#include "stack_impl.cpp"

template <typename T>
class stack : private stack_impl<T>
{
public:
    stack(size_t size = 0);

    // copy operations
    stack(const stack &);
    stack &operator=(stack);

    // move operations
    stack(stack &&);
    stack &operator=(stack &&);

    friend void swap(stack &lhs, stack &rhs);

    template <typename... Args>
    void push(const Args &...args);

    void pop();

    const T &top();

    size_t count() const noexcept { return this->used_; };
};

template <typename T>
stack<T>::stack(size_t size) : stack_impl<T>(size) {}

template <typename T>
stack<T>::stack(const stack &oth) : stack_impl<T>(oth.size_)
{
    while (count() < oth.count())
    {
        construct(this->v_ + this->used_, oth.v_[this->used_]);
        this->used_++;
    }
}

template <typename T>
stack<T> &stack<T>::operator=(stack oth)
{
    swap(*this, oth);
    return *this;
}

template <typename T>
stack<T>::stack(stack &&oth)
{
    swap(*this, oth);
}

template <typename T>
stack<T> &stack<T>::operator=(stack &&oth)
{
    stack<T> temp(std::move(oth));
    swap(*this, oth);
    return *this;
}

template <typename T>
void swap(stack<T> &lhs, stack<T> &rhs)
{
    swap(lhs.v_, rhs.v_);
    swap(lhs.used_, rhs.used_);
    swap(lhs.used_, rhs.used_);
}

template <typename T>
template <typename... Args>
void stack<T>::push(const Args &...args)
{
    if (this->size_ == this->used_)
    {
        stack<T> temp(this->size_ * 2 + 1);
        while (temp.count() < this->used_)
            temp.push(this->v[temp.count()]);
        temp.push(args...);
        swap(*this, temp);
    }
    else
    {
        this->v_[this->used_] = construct(this->v_ + this->used_, args...);
        ++(this->used_);
    }
}

template <typename T>
void stack<T>::pop()
{
    if (0 == count())
        throw "empty stack";

    destroy(this->v_ + this->used_ - 1);
    --(this->used_);
}

template <typename T>
const T &stack<T>::top()
{
    if (0 == count())
        throw "empty stack";
    return this->v_[this->used_ - 1];
}