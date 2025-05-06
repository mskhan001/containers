#include <new>
#include <utility>

template <typename T> class stack_base {
public:
  stack_base() : start_(nullptr), end_(nullptr), capacity_(nullptr) {}

  stack_base(size_t capacity)
      : start_(static_cast<T *>(::operator new(capacity * sizeof(T)))), end_(start_),
        capacity_(start_ + capacity) {}

  // delete copy operations
  stack_base(const stack_base &) = delete;
  stack_base &operator=(const stack_base &) = delete;

  // move constructor
  stack_base(stack_base &&oth) noexcept
      : start_(oth.start_), end_(oth.end_), capacity_(oth.capacity_) {
    oth.start_ = oth.capacity_ = oth.end_ = nullptr;
  }

  // move assignment
  stack_base &operator=(stack_base &&oth) noexcept {
    // move and swap idiom
    stack_base tmp{std::move(oth)}; // move
    swap(*this, tmp);               // swap
    return *this;
  }

  // destructor
  ~stack_base() {
    if (start_)
      ::operator delete(start_, capacity());
  }

  // noexcept swap
  friend void swap(stack_base<T> &lhs, stack_base<T> &rhs) noexcept {
    using std::swap;
    swap(lhs.start_, rhs.start_);
    swap(lhs.end_, rhs.end_);
    swap(lhs.capacity_, rhs.capacity_);
  }

  void construct(T *dst, const T &val) { new (&*dst) T(val); }
  void destroy(T *src) { src->~T(); }

  void destroy_range(T *src, T *dst) {
    for (T *p = src; p != dst; ++p)
      destroy(p);
  }

  size_t size() const noexcept { return end_ - start_; }
  size_t capacity() const noexcept { return capacity_ - start_; }

  bool empty() const noexcept { return 0 == size(); }
  bool full() const noexcept { return size() == capacity(); }

  /* copies T from [src_begin, src_end) to memory starting from dst_begin */
  void uninitialized_copy(T *src_begin, T *src_end, T *dst_begin) {
    T *dst = dst_begin;
    T *src = src_begin;

    try {
      for (; src != src_end; ++src, ++dst)
        construct(dst, *src);
    } catch (...) {
      for (T *q = dst_begin; q != dst; ++q)
        destroy(q);
      throw;
    }
  }

  /* moves T from [src_begin, src_end) to memory starting from dst_begin */
  void uninitialized_move(T *src_begin, T *src_end, T *dst_begin) {
    T *src = src_begin;
    T *dst = dst_begin;

    try {
      for (; src != src_end; ++src, ++dst)
        construct(dst, std::move(*src));
    } catch (...) {
      for (T *q = dst_begin; q != dst; ++q)
        destroy(q);
      throw;
    }
  }

  T *start_;    // points to area of memory
  T *end_;      // points to where next element will be inserted
  T *capacity_; // points to the last memory region + 1
};