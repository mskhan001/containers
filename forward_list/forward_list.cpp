#include <cstddef>
#include <iostream>
#include "./../debug.cpp"

template <typename T>
class forward_list
{
private:
    struct Node
    {
        T data_;
        Node *next_;

        Node(const T &data, Node *next = nullptr) : data_(data), next_(next) {}
    };

public:
    class iterator
    {
    public:
        iterator(Node *ptr) : ptr_(ptr) {}

        T &operator*() { return ptr_->data_; }
        T *operator->() { return &(ptr_->data_); }

        iterator &operator++()
        {
            ptr_ = ptr_->next_;
            return *this;
        }

        iterator operator++(int)
        {
            iterator tmp(ptr_);
            ptr_ = ptr_->next_;
            return tmp;
        }

        friend bool operator==(const iterator &lhs, const iterator &rhs) { return lhs.ptr_ == rhs.ptr_; }
        friend bool operator!=(const iterator &lhs, const iterator &rhs) { return lhs.ptr_ != rhs.ptr_; }

        friend class forward_list;

    private:
        Node *ptr_;
    };

    iterator begin() const { return iterator(head_); }
    iterator end() const { return iterator(nullptr); }

    forward_list() : head_(nullptr), tail_(nullptr), size_(0) {}

    // copy constructor
    forward_list(const forward_list &oth) : head_(nullptr), tail_(nullptr), size_(0)
    {
        try
        {
            for (const auto &n : oth)
                push_back(n);
        }
        catch (...)
        {
            destroy_all(head_);
            throw;
        }
    }

    forward_list &operator=(const forward_list &oth)
    {
        // copy-and-swap idiom
        forward_list tmp(oth);
        swap(*this, tmp);
        return *this;
    }

    // destructor
    ~forward_list()
    {
        destroy_all(head_);
    }

    template <typename U>
    friend void swap(forward_list<U> &lhs, forward_list<U> &rhs)
    {
        std::swap(lhs.head_, rhs.head_);
        std::swap(lhs.tail_, rhs.tail_);
        std::swap(lhs.size_, rhs.size_);
    }

    const T &front() const
    {
        if (empty())
            throw "empty list";
        return head_->data_;
    }

    void push_front(const T &val)
    {
        Node *node_to_be_inserted = construct(val, head_);
        if (empty())
            tail_ = node_to_be_inserted;
        head_ = node_to_be_inserted;
        ++size_;
    }

    void pop_front()
    {
        if (empty())
            throw "empty list";

        Node *new_head = head_->next_;
        destroy(head_);
        head_ = new_head;
        --size_;

        if (empty())
            tail_ = nullptr;
    }

    void push_back(const T &val)
    {
        if (empty())
        {
            push_front(val);
            return;
        }

        Node *new_node_to_be_inserted = construct(val);
        tail_->next_ = new_node_to_be_inserted;
        tail_ = new_node_to_be_inserted;
        ++size_;
    }

    void insert_after(iterator pos, const T &val)
    {
        Node *prior_node = pos.ptr_;

        if (nullptr == prior_node)
            throw "end of list";

        Node *after_node = prior_node->next_;
        Node *new_node_to_be_inserted = construct(val, after_node);
        prior_node->next_ = new_node_to_be_inserted;

        if (prior_node == tail_)
            tail_ = new_node_to_be_inserted;

        ++size_;
    }

    void reverse()
    {
        Node *tmp = nullptr;
        Node *start = head_;
        Node *end = tail_;

        while (start)
        {
            std::swap(start->next_, tmp);
            std::swap(start, tmp);
        }

        std::swap(head_, tail_);
    }

    size_t size() const { return size_; }
    bool empty() const { return nullptr == head_; }

private:
    Node *construct(const T &val, Node *next = nullptr)
    {
        return new Node(val, next);
    }

    void destroy(Node *ptr) noexcept
    {
        delete ptr;
    }

    void destroy_all(Node *ptr) noexcept
    {
        while (ptr)
        {
            Node *next_node_to_delete = ptr->next_;
            delete ptr;
            ptr = next_node_to_delete;
        }
    }

    Node *head_;  // points to the front of the list
    Node *tail_;  // points to end of list
    size_t size_; // number of elements in the list
};