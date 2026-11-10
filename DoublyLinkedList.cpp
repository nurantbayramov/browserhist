#include "DoublyLinkedList.h"

using namespace std;

// Adds n at the end of the list. Works on an empty list too (n becomes head and tail).
void DoublyLinkedList::append(Node* n) {
    if (!n) return;
    n->prev_ = tail_;
    n->next_ = nullptr;
    if (tail_) tail_->next_ = n;
    else head_ = n; // list was empty
    tail_ = n;
    size_++;
}

// Adds n at the start of the list. Works on an empty list too.
void DoublyLinkedList::push_front(Node* n) {
    if (!n) return;
    n->next_ = head_;
    n->prev_ = nullptr;
    if (head_) head_->prev_ = n;
    else tail_ = n; // list was empty
    head_ = n;
    size_++;
}

// Puts n right after ref. If ref is null we just append.
void DoublyLinkedList::insert_after(Node* ref, Node* n) {
    if (!n) return;
    if (!ref) {
        append(n);
        return;
    }
    n->prev_ = ref;
    n->next_ = ref->next_;
    if (ref->next_) ref->next_->prev_ = n; // inserting in the middle
    else tail_ = n; // ref was the tail, so n is the new tail
    ref->next_ = n;
    size_++;
}

// Unlinks n from the list and gives it back. The caller must delete it or put it back in.
Node* DoublyLinkedList::remove_node(Node* n) {
    if (!n) return nullptr;
    if (n->prev_) n->prev_->next_ = n->next_;
    else head_ = n->next_; // n was the head
    if (n->next_) n->next_->prev_ = n->prev_;
    else tail_ = n->prev_; // n was the tail
    n->prev_ = nullptr;
    n->next_ = nullptr;
    size_--;
    return n;
}

// Unlinks and returns the oldest node (the head), or nullptr if the list is empty.
Node* DoublyLinkedList::remove_head() {
    return remove_node(head_);
}

// Deletes every node after n and makes n the new tail. Returns how many were deleted.
int DoublyLinkedList::truncate_after(Node* n) {
    if (!n) return 0;
    int count = 0;
    Node* cur = n->next_;
    while (cur) {
        Node* nxt = cur->next_; // save the next node before deleting this one
        delete cur;
        cur = nxt;
        count++;
    }
    n->next_ = nullptr;
    tail_ = n;
    size_ -= count;
    return count;
}

// Deletes all nodes and resets the list to empty.
void DoublyLinkedList::clear() {
    Node* cur = head_;
    while (cur) {
        Node* nxt = cur->next_; // save the next node before deleting this one
        delete cur;
        cur = nxt;
    }
    head_ = tail_ = nullptr;
    size_ = 0;
}

// Returns pointers to all nodes from head to tail (the list still owns them).
vector<Node*> DoublyLinkedList::to_list() const {
    vector<Node*> out;
    out.reserve(size_);
    for (Node* c = head_; c; c = c->next_) out.push_back(c);
    return out;
}

// Returns the first node with this url, or nullptr if there is none.
Node* DoublyLinkedList::find(const string& url) const {
    for (Node* c = head_; c; c = c->next_)
        if (c->url == url) return c;
    return nullptr;
}

// Checks that head, tail, size and every prev/next link agree. Used by the tests.
bool DoublyLinkedList::verify() const {
    // empty list: head, tail and size must all say empty
    if (!head_ || !tail_) return !head_ && !tail_ && size_ == 0;
    if (head_->prev_ || tail_->next_) return false;
    // walk forward: every next node must point back to the current one
    int count = 0;
    for (Node* c = head_; c; c = c->next_) {
        if (c->next_ && c->next_->prev_ != c) return false;
        if (++count > size_) return false; // cycle or bad size, bail
    }
    if (count != size_) return false;
    // walk backward the same way
    count = 0;
    for (Node* c = tail_; c; c = c->prev_) {
        if (c->prev_ && c->prev_->next_ != c) return false;
        if (++count > size_) return false;
    }
    return count == size_;
}