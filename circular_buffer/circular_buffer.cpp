#include <array>

template <typename T, std::size_t N>
class circular_buffer
{
public:
    circular_buffer() : head_(0), tail_(0), size_(0) {}

    // copy constructor
    circular_buffer(const circular_buffer &) = default;

    // copy assignment
    circular_buffer &operator=(const circular_buffer &) = default;

    // move constructor
    circular_buffer(circular_buffer &&) = default;

    // move assignment
    circular_buffer &operator=(circular_buffer &&) = default;

    void push(const T &val)
    {
        if (full())
            pop();

        buffer_[head_] = val;
        head_ = index(++head_);
        ++size_;
    }

    const T &peek() const
    {
        if (empty())
            throw std::out_of_range("no elements to peek");

        return buffer_[tail_];
    }

    void pop()
    {
        if (empty())
            throw std::out_of_range("no elements to pop");

        tail_ = index(++tail_);
        --size_;
    }

    int capacity() const noexcept { return N; }

    int size() const noexcept { return size_; }

private:
    bool empty() const noexcept { return 0 == size(); }
    bool full() const noexcept { return capacity() == size(); }

    int index(int ind) const noexcept
    {
        return ind % capacity();
    }
    // buffer of capacity N
    std::array<T, N> buffer_;

    // points to the position where next element will be pushed
    int head_;

    // points to position from where element will be popped
    int tail_;

    // number of elements in the buffer
    int size_;
};