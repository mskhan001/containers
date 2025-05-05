#include "vector_base.cpp"

template <typename T, typename Alloc = std::allocator<T>> class vector {
  using Base = vector_base<T, Alloc>;

public:
  vector() : vector_base_() {}

  vector(size_t capacity, const T &init_val = T(), const Alloc &alloca = Alloc())
      : vector_base_(capacity, alloca) {
    vector_base_.uninitialized_fill(vector_base_.start_, vector_base_.end_, init_val);
    vector_base_.end_ = vector_base_.capacity_;
  }

  // copy constructor
  vector(const vector &oth) : vector_base_(oth.vector_base_.capacity(), oth.vector_base_.alloca_) {
    vector_base_.uninitialized_copy(oth.vector_base_.start_, oth.vector_base_.end_,
                                    vector_base_.start_);
    vector_base_.end_ = vector_base_.start_ + oth.vector_base_.size();
  }

  // copy-assignment
  vector &operator=(const vector &oth) {
    vector tmp(oth);  // copy
    swap(*this, tmp); // swap
    return *this;
  }

  // move constructor
  vector(vector &&oth) noexcept : vector_base_(std::move(oth.vector_base_)) {}

  // move assignment
  vector &operator=(vector &&oth) noexcept {
    // move and swap idiom
    vector tmp(std::move(oth)); // move
    swap(*this, tmp);           // swap
    return *this;
  }

  ~vector() { destroy_range(vector_base_.start_, vector_base_.end_); }

  friend void swap(vector<T> &lhs, vector<T> &rhs) noexcept {
    using std::swap;
    swap(lhs.vector_base_, rhs.vector_base_);
  }

  /* reserve() provides strong exception safety guarantee */
  void reserve(size_t new_capacity) {
    if (new_capacity <= vector_base_.capacity())
      return;

    Base new_base(new_capacity, vector_base_.alloca_);
    //    new_base.uninitialized_copy(vector_base_.start_, vector_base_.end_, new_base.start_);
    new_base.uninitialized_move(vector_base_.start_, vector_base_.end_, new_base.start_);
    new_base.end_ = new_base.start_ + vector_base_.size();

    destroy_range(vector_base_.start_, vector_base_.end_);
    swap(vector_base_, new_base);
  }

  /* resize() guarantees strong exception safety */
  void resize(size_t new_size, const T &init_value = T()) {

    if (new_size < vector_base_.size()) {
      destroy_range(vector_base_.start_ + new_size, vector_base_.end_);
      vector_base_.end_ = vector_base_.start_ + new_size;
      return;
    }

    if (new_size < vector_base_.capacity()) {
      vector_base_.uninitialized_fill(vector_base_.end_, vector_base_.start_ + new_size,
                                      init_value);
      vector_base_.end_ = vector_base_.start_ + new_size;
      return;
    }

    Base new_base(new_size, vector_base_.alloca_);
    //    new_base.uninitialized_copy(vector_base_.start_, vector_base_.end_, new_size.start_);
    new_base.uninitialized_move(vector_base_.start_, vector_base_.end_, new_base.start_);
    new_base.uninitialized_fill(new_base.start_ + vector_base_.size(), new_base.start_ + new_size,
                                init_value);
    new_base.end_ = new_base.start_ + new_size;

    destroy_range(vector_base_.start_, vector_base_.end_);
    swap(vector_base_, new_base);
  }

  /* push_back guarantees strong exception safety gurantee */
  void push_back(const T &val) {
    if (vector_base_.size() < vector_base_.capacity()) {
      Base::AllocatorTraits::construct(vector_base_.alloca_, vector_base_.end_, val);
      ++vector_base_.end_;
      return;
    }

    vector tmp(*this);
    tmp.reserve(1 + tmp.size() * 2);
    tmp.push_back(val);

    swap(*this, tmp);
  }

  template <typename... Args> void emplace_back(const Args &...args) {
    if (vector_base_.size() < vector_base_.capacity()) {
      Base::AllocatorTraits::construct(vector_base_.alloca_, vector_base_.end_, args...);
      ++vector_base_.end_;
      return;
    }

    vector tmp(*this);
    tmp.reserve(1 + tmp.size() * 2);
    tmp.emplace_back(args...);
    swap(*this, tmp);
  }

  size_t size() const noexcept { return vector_base_.size(); }
  size_t capacity() const noexcept { return vector_base_.capacity(); }

private:
  void destroy_range(T *start, T *end) {
    for (T *ptr = start; ptr != end; ++ptr)
      Base::AllocatorTraits::destroy(vector_base_.alloca_, ptr);
  }

  Base vector_base_;
};