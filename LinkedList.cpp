#include <iostream>
#include <initializer_list>

struct Node
{
    int data_;   // data stored
    Node *next_; // pointer to the next element
    Node(int data, Node *next = nullptr) : data_(data), next_(next) {}
};

class LinkedList
{
private:
    Node *sentinel_;   // dummy node which points to head(0th node)
    Node *last_;       // last node of the LinkedList
    std::size_t size_; // number of nodes in the LinkedList

    // returns a pointer to the Kth node of the LinkedList
    // indexing starts from 0
    Node *GetKthNode(std::size_t index)
    {
        // if (index < 0 || index >= size())
        //     throw std::out_of_range(std::format("index {} out of range", index));
        Node *node = sentinel_;
        while (index--)
            node = node->next_;

        return node;
    }

public:
    // default constructor
    LinkedList() : sentinel_(new Node(-1)), last_(nullptr), size_(0) {}

    LinkedList(std::initializer_list<int> list) : LinkedList()
    {
        Node *head = sentinel_;
        for (const auto &e : list)
        {
            head->next_ = new Node(e);
            ++size_;
            head = head->next_;
        }
        last_ = head;
    }

    Node *begin() { return sentinel_->next_; } // first node
    Node *end() { return last_; }              // last node

    // insert a Node(data) at the end of the LinkedList
    void push_back(int data)
    {
        // if linked list is empty
        if (empty())
            sentinel_->next_ = last_ = new Node(data);
        else
            last_->next_ = new Node(data);

        ++size_;
    }

    // insert a Node at the Kth position
    void insert(std::size_t index, int data)
    {
        if (index <= 0 || index > size() + 1)
            return;

        // node after which, new node is to be inserted
        Node *prev_node = sentinel_;
        if (index > 1)
            prev_node = GetKthNode(index - 1);

        // insert the new node
        prev_node->next_ = new Node(data, prev_node->next_);

        // check if the inserted node is the last node
        if (index == size() + 1)
            last_ = prev_node->next_;

        ++size_;
    }

    void reverse()
    {
        Node *temp = nullptr;
        Node *head = last_ = sentinel_->next_;

        while (head)
        {
            std::swap(head->next_, temp);
            std::swap(temp, head);
        }

        sentinel_->next_ = temp;
    }
    std::size_t size() { return size_; }
    bool empty() { return 0 == size(); }
};

void printLinkedList(Node *head)
{
    while (head)
    {
        std::cout << head->data_ << "\t";
        head = head->next_;
    }
    std::cout << "\n";
}

int main()
{
    LinkedList l{43};
    // printLinkedList(l.begin());
    // l.insert(1, 10);
    // printLinkedList(l.begin());
    // l.insert(5, 786);
    // printLinkedList(l.begin());
    l.reverse();
    printLinkedList(l.begin());
}