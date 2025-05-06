#include "stack_base.cpp"

template <typename T> class stack {
public:
  stack(size_t capacity = 16) : stack_base_(capacity) {}

  // copy constructor
  stack(const stack &oth) : stack_base_(oth.stack_base_.capacity()) {
    stack_base_.uninitialized_copy(oth.stack_base_.start_, oth.stack_base_.end_,
                                   stack_base_.start_);
    stack_base_.end_ = stack_base_.start_ + oth.stack_base_.size();
  }

  // copy assignment
  stack &operator=(const stack &oth) {
    // copy-and-swap idiom
    stack tmp(oth);   // copy
    swap(*this, tmp); // swap
    return *this;
  }

  // move constructor
  stack(stack &&oth) noexcept : stack_base_(std::move(oth.stack_base_)) {}

  // move assignment
  stack &operator=(stack &&oth) noexcept {
    // move-and-swap idiom
    stack tmp(std::move(oth)); // move
    swap(*this, tmp);          // swap
    return *this;
  }

  // destructor
  ~stack() { stack_base_.destroy_range(stack_base_.start_, stack_base_.end_); }

  void push(const T &val) {
    if (full())
      throw "empty stack";

    stack_base_.construct(stack_base_.end_, val);
    ++stack_base_.end_;
  }

  void pop() {
    if (empty())
      throw "empty stack";
    stack_base_.destroy(stack_base_.end_ - 1);
    --stack_base_.end_;
  }

  T &top() {
    if (empty())
      throw "empty stack";

    return *(stack_base_.end_ - 1);
  }

  const T &top() const {
    if (empty())
      throw "empty stack";

    return *(stack_base_.end_ - 1);
  }

  friend void swap(stack<T> &lhs, stack<T> &rhs) {
    using std::swap;
    swap(lhs.stack_base_, rhs.stack_base_);
  }

  size_t count() const noexcept { return stack_base_.size(); }
  size_t size() const noexcept { return stack_base_.size(); }
  size_t capacity() const noexcept { return stack_base_.capacity(); }

  bool empty() const noexcept { return stack_base_.empty(); }
  bool full() const noexcept { return stack_base_.full(); }

private:
  stack_base<T> stack_base_;
};