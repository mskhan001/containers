#include <cstddef>
#include <utility>

template <typename T> class forward_list {
private:
  struct Node {
    T data_;
    Node *next_;

    Node(const T &data = T(), Node *next = nullptr) : data_(data), next_(next) {}
  };

public:
  class iterator {
  public:
    iterator(Node *ptr) : ptr_(ptr) {}

    T &operator*() { return ptr_->data_; }
    T *operator->() { return &(ptr_->data_); }

    iterator &operator++() {
      ptr_ = ptr_->next_;
      return *this;
    }

    iterator operator++(int) {
      iterator tmp(ptr_);
      ptr_ = ptr_->next_;
      return tmp;
    }

    friend bool operator==(const iterator &lhs, const iterator &rhs) {
      return lhs.ptr_ == rhs.ptr_;
    }

    friend bool operator!=(const iterator &lhs, const iterator &rhs) {
      return !(operator==(lhs, rhs));
    }
    friend class forward_list;

  private:
    Node *ptr_;
  };

  iterator begin() const { return head_->next_; }
  iterator end() const { return nullptr; }

  forward_list() : head_(construct()), tail_(head_), size_(0) {}

  // copy constructor
  forward_list(const forward_list &oth) : head_(construct()), tail_(head_), size_(0) {
    uninitialized_copy(oth.head_, head_);
  }

  // copy assignment
  forward_list &operator=(const forward_list &oth) {
    if (&oth == this)
      return *this;

    // copy-and-swap-idiom
    forward_list tmp(oth); // copy
    swap(*this, tmp);
    return *this;
  }

  // move constructor
  forward_list(forward_list &&oth) noexcept : head_(oth.head_), tail_(oth.tail_), size_(oth.size_) {
    oth.head_ = oth.tail_ = nullptr;
    oth.size_ = 0;
  }

  // move assignment
  forward_list &operator=(forward_list &&oth) noexcept {
    if (&oth == this)
      return *this;

    // move and swap idiom
    forward_list tmp(std::move(oth));
    swap(*this, tmp);
    return *this;
  }

  ~forward_list() { destroy_all(head_); }

  T &front() {
    if (empty())
      throw "empty list";

    return head_->next_->data_;
  }

  const T &front() const {
    if (empty())
      throw "empty_list";
    return head_->next_->data_;
  }

  void push_front(const T &val) {
    if (empty()) {
      push_back(val);
      return;
    }

    Node *new_node = construct(val, nullptr);
    new_node->next_ = head_->next_;
    head_->next_ = new_node;
    ++size_;
  }

  void pop_front() {
    if (empty())
      throw "empty list";

    Node *node_to_delete = head_->next_;
    head_->next_ = node_to_delete->next_;

    destroy(node_to_delete);
    --size_;

    if (empty())
      tail_ = head_;
  }

  void push_back(const T &val) {
    tail_->next_ = construct(val);
    tail_ = tail_->next_;
    ++size_;
  }

  void insert_after(const iterator iter, const T &val) {
    Node *it = iter.ptr_;
    if (it == tail_) {
      push_back(val);
      return;
    }

    Node *node_to_insert = construct(val, nullptr);
    node_to_insert->next_ = it->next_;
    it->next_ = node_to_insert;
    ++size_;
  }

  size_t size() const { return size_; }
  bool empty() const { return 0 == size_; }

  friend void swap(forward_list<T> &lhs, forward_list<T> &rhs) noexcept {
    using std::swap;
    swap(lhs.head_, rhs.head_);
    swap(lhs.tail_, rhs.tail_);
    swap(lhs.size_, rhs.size_);
  }

  void reverse() {
    tail_ = head_->next_;

    Node *tmp = nullptr;
    Node *head = head_->next_;

    while (head) {
      std::swap(tmp, head->next_);
      std::swap(tmp, head);
    }

    head_->next_ = tmp;
  }

  friend class iterator;

private:
  Node *construct(const T &val = T(), Node *next = nullptr) { return new Node(val, next); }
  void destroy(Node *ptr) { delete ptr; }

  void destroy_all(Node *src_begin) {
    Node *node_to_delete = src_begin;

    while (node_to_delete) {
      Node *next_node = node_to_delete->next_;
      destroy(node_to_delete);
      node_to_delete = next_node;
    }
  }

  void uninitialized_copy(Node *src_head, Node *dst_head) {
    Node *src = src_head;
    Node *dst = dst_head;

    try {
      while (src->next_) {
        push_back(src->next_->data_);
        src = src->next_;
      }
    } catch (...) {
      destroy_all(dst_head->next_);
      size_ = 0;
    }
  }

  /*
  push() - node->next;
  pop() - tail
  */
  Node *head_; // points to a sentinel node
  Node *tail_; // points to thse last node
  size_t size_;
};