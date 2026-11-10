#include "DoublyLinkedList.h"

using namespace std;

void DoublyLinkedList::append(Node* n) {
    if (!n) return;
    n->prev_ = tail_;
    n->next_ = nullptr;
    if (tail_) tail_->next_ = n;
    else head_ = n;
    tail_ = n;
    size_++;
}

void DoublyLinkedList::push_front(Node* n) {
    if (!n) return;
    n->next_ = head_;
    n->prev_ = nullptr;
    if (head_) head_->prev_ = n;
    else tail_ = n;
    head_ = n;
    size_++;
}

void DoublyLinkedList::insert_after(Node* ref, Node* n) {
    if (!n) return;
    if (!ref) {
        append(n);
        return;
    }
    n->prev_ = ref;
    n->next_ = ref->next_;
    if (ref->next_) ref->next_->prev_ = n;
    else tail_ = n;
    ref->next_ = n;
    size_++;
}

Node* DoublyLinkedList::remove_node(Node* n) {
    if (!n) return nullptr;
    if (n->prev_) n->prev_->next_ = n->next_;
    else head_ = n->next_;
    if (n->next_) n->next_->prev_ = n->prev_;
    else tail_ = n->prev_;
    n->prev_ = nullptr;
    n->next_ = nullptr;
    size_--;
    return n;
}

Node* DoublyLinkedList::remove_head() {
    return remove_node(head_);
}

void DoublyLinkedList::clear() {
    Node* cur = head_;
    while (cur) {
        Node* nxt = cur->next_;
        delete cur;
        cur = nxt;
    }
    head_ = tail_ = nullptr;
    size_ = 0;
}

vector<Node*> DoublyLinkedList::to_list() const {
    vector<Node*> out;
    out.reserve(size_);
    for (Node* c = head_; c; c = c->next_) out.push_back(c);
    return out;
}

Node* DoublyLinkedList::find(const string& url) const {
    for (Node* c = head_; c; c = c->next_)
        if (c->url == url) return c;
    return nullptr;
}

bool DoublyLinkedList::verify() const {
    if (!head_ || !tail_) return !head_ && !tail_ && size_ == 0;
    if (head_->prev_ || tail_->next_) return false;

    int count = 0;
    for (Node* c = head_; c; c = c->next_) {
        if (c->next_ && c->next_->prev_ != c) return false;
        if (++count > size_) return false; // cycle or bad size, bail
    }
    if (count != size_) return false;

    count = 0;
    for (Node* c = tail_; c; c = c->prev_) {
        if (c->prev_ && c->prev_->next_ != c) return false;
        if (++count > size_) return false;
    }
    return count == size_;
}