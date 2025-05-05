#include <memory>

template <typename T, typename Alloc = std::allocator<T>> class vector_base {
public:
  using AllocatorTraits = std::allocator_traits<Alloc>;

  vector_base(size_t capacity, const Alloc &alloca = Alloc())
      : alloca_(alloca), start_(AllocatorTraits::allocate(alloca_, capacity)), end_(start_),
        capacity_(start_ + capacity) {}

  vector_base() : start_(nullptr), end_(nullptr), capacity_(nullptr) {}

  ~vector_base() {
    if (start_)
      AllocatorTraits::deallocate(alloca_, start_, capacity_ - start_);
  }

  // delete copy operations
  vector_base(const vector_base &) = delete;
  vector_base &operator=(const vector_base &) = delete;

  // mov constructor
  vector_base(vector_base &&oth) noexcept
      : alloca_(std::move(oth.alloca_)), start_(oth.start_), end_(oth.end_),
        capacity_(oth.capacity_) {
    oth.start_ = oth.end_ = oth.capacity_ = nullptr;
  }

  // move assignment
  vector_base &operator=(vector_base &&oth) noexcept {
    // move and swap idiom
    vector_base tmp(std::move(oth)); // move
    swap(*this, tmp);                // swap
    return *this;
  }

  friend void swap(vector_base<T> &lhs, vector_base<T> &rhs) noexcept {
    /*
    1. allows ADL to find a specialized lookup
      - using std::swap
      - swap(x, y)

    2. calls only std::swap
     - std::swap(x,y)
    */
    using std::swap;
    swap(lhs.alloca_, rhs.alloca_);
    swap(lhs.start_, rhs.start_);
    swap(lhs.end_, rhs.end_);
    swap(lhs.capacity_, rhs.capacity_);
  }

  size_t size() const noexcept { return end_ - start_; }

  size_t capacity() const noexcept { return capacity_ - start_; }

  /* initializeds range [dst_start, dst_end) with init_value */
  void uninitialized_fill(T *dst_start, T *dst_end, const T &init_value) {
    T *p;
    try {
      for (p = dst_start; p != dst_end; ++p)
        AllocatorTraits::construct(alloca_, p, init_value);
    } catch (...) {
      for (T *q = dst_start; q != p; ++q)
        AllocatorTraits::destroy(alloca_, q);
      throw;
    }
  }

  /* copies from [src_start, src_end) to memory beginning from dst*/
  void uninitialized_copy(T *src_start, T *src_end, T *dst_start) {
    T *dst = dst_start;
    T *src = src_start;
    try {
      for (; src != src_end; ++src, ++dst)
        AllocatorTraits::construct(alloca_, dst, *src);
    } catch (...) {
      for (T *q = dst_start; q != dst; ++q)
        AllocatorTraits::destroy(alloca_, q);
      throw;
    }
  }

  /* moves from [src_start, src_end) to memory beginning from dst*/
  void uninitialized_move(T *src_start, T *src_end, T *dst_start) {
    T *dst = dst_start;
    T *src = src_start;
    try {
      for (; src != src_end; ++src, ++dst)
        AllocatorTraits::construct(alloca_, dst, std::move(*src));
    } catch (...) {
      for (T *q = dst_start; q != dst; ++q)
        AllocatorTraits::destroy(alloca_, q);
    }
  }

  Alloc alloca_; // allocator
  T *start_;     // points to the beginning of the memory region
  T *end_;       // points to the position where next element will be pushed
  T *capacity_;  // points to last valid position + 1
};
