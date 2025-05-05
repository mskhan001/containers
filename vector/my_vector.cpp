#include <memory>

/*
    constructs elements in the range [first, last) with val
*/
template <typename Iter, typename T>
void myuninitialized_fill(Iter first, Iter last, const T &val) {
  Iter p;
  try {
    for (p = first; p != last; ++p)
      new (&*p) T(val);
  } catch (...) {
    for (Iter q = first; q != p; ++q)
      q->~T();
    throw;
  }
}

template <typename Iter, typename T>
void myuninitialized_copy(Iter begin, Iter end, Iter d_begin) {
  Iter src = begin, dst = d_begin;
  try {
    for (; src != end; ++src, ++dst)
      new (&*dst) T(*src);
  } catch (...) {
    for (Iter q = d_begin; q != dst; ++q)
      q->~T();
    throw;
  }
}

template <typename Iter, typename T>
void myuninitialized_move(Iter begin, Iter end, Iter d_begin) {
  // no way to reover if move throws
  for (; src != end; ++src, ++dst)
    new (&*dst) T(std::move(*dst));
}

/*
    auxiallary class which deals with allocation and deallocation of memory
    hold pointer to end regions of allocated memory
*/
template <typename T, typename A = std::allocator<T>> class myvector_base {
public:
  using size_type = std::size_t;

  A alloca_; // allocator
  T *elem_;  // pointer to first byte of memory
  T *space_; // pointer to last constructed byte + 1
  T *last_;  // pointer to last allocated byte + 1

  // default constructor
  myvector_base(size_type n, const A &alloca = A())
      : alloca_(alloca), elem_(alloca_.allocate(n)), last_(elem_ + n),
        space_(elem_ + n) {}

  myvector_base(const myvector_base &) = delete; // delete copy constructor
  myvector_base &
  operator=(const myvector_base &) = delete; // delete copy assignment

  // move constructor
  myvector_base(myvector_base &&oth)
      : alloca_(std::move(oth.alloca_)), elem_(oth.elem_), last_(oth.last_),
        space_(oth.space_) {
    oth.elem_ = oth.last_ = oth.space_ = nullptr;
  }

  myvector_base &operator=(myvector_base &&oth) // move assignment
  {
    swap(*this, oth);
    return *this;
  }

  ~myvector_base() { alloca_.deallocate(elem_, last_ - elem_); }
  size_type size() const noexcept { return last_ - elem_; }

  friend void swap(myvector_base &lhs, myvector_base &rhs) {
    std::swap(lhs.alloca_, rhs.alloca_);
    std::swap(lhs.elem_, rhs.elem_);
    std::swap(lhs.last_, rhs.last_);
    std::swap(lhs.space_, rhs.space_);
  }
};

template <typename T, typename A = std::allocator<T>> class myvector {
public:
  using size_type = std::size_t;

  // default constructor
  explicit myvector(size_type n, const T &init_val = T(), const A &alloca = A())
      : vb_(n, alloca) {
    myuninitialized_fill(vb_.elem_, vb_.last_, init_val);
  }

  // copy constructor
  myvector(const myvector &oth) : vb_(oth.vb_.size(), oth.vb_.alloca_) {
    myuninitialized_copy<decltype(vb_.elem_), T>(oth.vb_.elem_, oth.vb_.last_,
                                                 vb_.elem_);
  }

  // copy assignment
  myvector &operator=(const myvector &oth) {
    size_type new_size = oth.size();

    // if current capacity is small, create a new vector and swap
    if (new_size > capacity()) {
      myvector tmp(oth);
      swap(*this, tmp);
      return *this;
    }

    // check for self-assignment
    if (this == &oth)
      return *this;

    if (new_size > size()) {
      // 1. copy till size - calls copy assignment operator
      mycopy(oth.vb_.elem_, oth.vb_.elem_ + size(), vb_.elem);

      //  uninitialized from from size to new_size - calls copy constructor
      myuninitialized_copy(oth.vb_.elem_ + size(), oth.vb_.last_,
                           vb_.elem_ + size());
    } else {
      // 1. copy till new size
      copy(oth.vb_.elem_, oth.vb_.last_, vb_.elem_);

      // 2. destory from new_size to size
      for (T *ptr = vb_.elem_ + new_size; ptr != vb_.last_; ++ptr)
        ptr->~T();
    }

    vb_.last_ = vb_.elem_ + new_size;
    return *this;
  }

  // move constructor
  myvector(myvector &&oth) : vb_(std::move(oth.vb_)) {}

  // move assignment
  myvector &operator=(myvector &&oth) {
    swap(*this, oth);
    return *this;
  }

  // destructor
  ~myvector() { destroy_elements(); }

  size_type size() const noexcept { return vb_.space_ - vb_.elem_; }
  size_type capacity() const noexcept { return vb_.space_ - vb_.elem_; }

  /* the capacity of the vector should be atleast as much as reserved */
  void reserve(size_type new_capacity) {
    if (new_capacity <= capacity())
      return;

    myvector_base<T, A> new_vb(new_capacity, vb_.alloca_);
    myuninitialized_move(vb_.elem_, vb_.last_, new_vb.elem_);

    swap(vb_, new_vb);
  }

  /* set the number of elements to n*/
  void resize(size_type n) {
    reserve(n);

    if (n < size())
      destroy_range(vb_.elem_ + n, vb_.elem_ + size());
    else
      myuninitialized_fill(vb_.elem_ + size(), vb_.elem_ + n);
    vb_.space_ = vb_.elem_ + n;
  }

  // Fix: Remove default arguments that depend on instance members
  void destroy_elements() { destroy_range(vb_.elem_, vb_.elem_ + size()); }

  // If you need a version with custom range, make it a separate function
  void destroy_range(T *begin, T *end) {
    for (T *ptr = begin; ptr != end; ++ptr)
      ptr->~T();
  }

  friend void swap(myvector &lhs, myvector &rhs) {
    std::swap(lhs.vb_, rhs.vb_);
  }

private:
  myvector_base<T, A> vb_;
};

int main() {
  myvector<int> a(5, 1);
  myvector<int> b(a);

  myvector<int> c(std::move(b));

  //    myvector<int> d = std::move(a);
  return 0;
}