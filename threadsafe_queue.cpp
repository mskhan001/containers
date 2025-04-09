#include <mutex>
#include <queue>
#include <exception>
#include <condition_variable>

class empty_queue : public std::exception
{
public:
    const char *what() const noexcept override
    {
        return "no elements in queue";
    }
};

template <typename T>
class threadsafe_queue
{
public:
    // deafult constructor
    threadsafe_queue() {}

    // copy constructor
    threadsafe_queue(const threadsafe_queue &oth)
    {
        std::lock_guard<std::mutex> lk(oth.m_);
        data_ = oth.data_;
    }

    // copy assignment is deleted
    threadsafe_queue &operator=(const threadsafe_queue &oth) = delete;

    void push(const T &val)
    {
        std::lock_guard<std::mutex> lk(m_);
        data_.push(val);
    }

    bool wait_and_pop(T &val)
    {
        std::lock_guard<std::mutex> lk(m_);
        cv_.wait(lk, [this]()
                 { return !data_.empty(); });
        val = data_.front();
        data_.pop();
    }

    std::shared_ptr<T> wait_and_pop()
    {
        std::lock_guard<std::mutex> lk(m_);
        cv_.wait(lk, [this]()
                 { return !data_.empty(); });
        std::shared_ptr<T> val = std::make_shared(data_.front());
        data_.pop();
        return val;
    }

    bool try_pop(T &val)
    {
        std::lock_guard<std::mutex> lk(m_);
        if (data_.empty())
            return false;
        val = data_.front();
        data_.pop();
        return true;
    }

    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lk(m_);
        if (data_.empty())
            return nullptr;

        std::shared_ptr<T> val = std::make_shared(data_.front());
        data_.pop();
        return val;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lk(m_);
        return data_.empty();
    }

private:
    std::queue<T> data_;
    mutable std::mutex m_;
    std::condition_variable cv_;
};

int main()
{
    threadsafe_queue<int> q;
    if (q.empty())
        ;
}