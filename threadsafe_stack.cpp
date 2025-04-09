#include <stack>
#include <mutex>
#include <exception>

class empty_stack : public std::exception
{
public:
    const char *what() const noexcept override
    {
        return "popping from an empty stack";
    }
};

template <typename T>
class threadsafe_stack
{
public:
    // default constructor
    threadsafe_stack() {}

    // copy constructor
    threadsafe_stack(const threadsafe_stack &oth)
    {
        std::lock_guard<std::mutex> lk(oth.m_);
        data_ = oth.data_;
    }

    // delete copy assignment operator
    threadsafe_stack &operator=(const threadsafe_stack &) = delete;

    void push(const T &val)
    {
        std::lock_guard<std::mutex> lk(m_);
        data_.push(val);
    }

    void pop(T &val)
    {
        std::lock_guard<std::mutex> lk(m_);
        if (data_.empty())
            throw empty_stack();
        val = data_.top();
        data_.pop();
    }

    std::shared_ptr<T> pop()
    {
        std::lock_guard<std::mutex> lk(m_);
        if (data_.empty())
            throw empty_stack();
        std::shared_ptr<T> res = std::make_shared<T>(data_.top());
        data_.pop();
        return res;
    }

private:
    std::stack<T> data_;
    std::mutex m_;
};

int main()
{
    threadsafe_stack<int> st;
}