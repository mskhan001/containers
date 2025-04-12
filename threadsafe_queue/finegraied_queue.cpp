#include <memory>
#include <mutex>

template <typename T>
class threadsafe_queue
{
private:
    struct node
    {
        std::shared_ptr<T> data_;
        std::unique_ptr<node> next_;
    };

    node *get_tail()
    {
        std::lock_guard<std::mutex> lk(tail_mutex_);
        return tail_;
    }

    std::unique_lock<std::mutex> wait_for_data()
    {
        std::unique_lock<std::mutex> lk(head_mutex_);
        cv_.wait(lk, [this]()
                 { return head_.get() != get_tail(); });

        return std::move(lk);
    }

public:
    threadsafe_queue() : head_(new node), tail_(head_.get()) {}
    threadsafe_queue(const threadsafe_queue &) = delete;
    threadsafe_queue &operator=(const threadsafe_queue &) = delete;

    void push(T value)
    {
        std::shared_ptr<T> data = std::make_shared<T>(std::move(value));
        std::unique_ptr<node> new_dummy_node = std::make_unique<node>();

        {
            std::lock_guard<std::mutex> lk(tail_mutex_);

            node *current_dummy_node = tail_;
            current_dummy_node->data_ = std::move(data);
            current_dummy_node->next_ = std::move(new_dummy_node);

            tail_ = current_dummy_node->next_.get();
        }
        cv_.notify_one();
    }

    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock<std::mutex> lk(wait_for_data());
        std::shared_ptr<T> value(std::move(head_->data_));
        head_ = std ::move(head_->next_);
        return value;
    }

    void wait_and_pop(T &value)
    {
        std::unique_lock<std::mutex> lk(wait_for_data());
        value = std::move(*(head_->data_));
        head_ = std::move(head_->next_);
    }

    std::shared_ptr<T> try_pop()
    {
        std::unique_lock<std::mutex> lk(head_mutex_);
        if (head_.get() == get_tail())
            return {};

        std::shared_ptr<T> value(std::move(head_->data_));
        head_ = std::move(head_->next_);
        return value;
    }

    bool try_pop(T &value)
    {
        std::unique_lock<std::mutex> lk(head_mutex_);
        if (head_.get() == get_tail())
            return false;
        value = std::move(*(head_->data_));
        head_ = std::move(head_->next_);
        return true;
    }

private:
    std::unique_ptr<node> head_;
    std::mutex head_mutex_;

    node *tail_;
    std::mutex tail_mutex_;

    std::condition_variable cv_;
};