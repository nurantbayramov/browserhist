#pragma once
#include <vector>
#include "Node.h"

using namespace std;

/**
 * Owns every node it holds. Node* handed out are non-owning and
 * die when the list changes. remove_node() is the one exception: the
 * caller gets the node back and has to delete it (or re-insert it).
 */
class DoublyLinkedList {
public:
    DoublyLinkedList() = default;
    ~DoublyLinkedList() { clear(); }
    DoublyLinkedList(const DoublyLinkedList&) = delete;
    DoublyLinkedList& operator=(const DoublyLinkedList&) = delete;
    /** Add at the tail. Fine on an empty list. */
    void append(Node* n);
    /** Add at the head. Fine on an empty list. */
    void push_front(Node* n);
    /** Link n right after ref. ref == nullptr falls back to append. */
    void insert_after(Node* ref, Node* n);
    /** Unlink n and return it, caller deletes. nullptr in -> nullptr out. */
    Node* remove_node(Node* n);
    /** Oldest node, unlinked. nullptr if empty. */
    Node* remove_head();
    /** Deletes everything after n, returns how many. 0 if n is the tail. */
    int truncate_after(Node* n);
    void clear();
    /** Snapshot head -> tail. Not owning. */
    vector<Node*> to_list() const;
    /** First node with this url, or nullptr. */
    Node* find(const string& url) const;
    Node* head() const { return head_; }
    Node* tail() const { return tail_; }
    int size() const { return size_; }
    bool is_empty() const { return size_ == 0; }
    /** Walks both ways, true if links and size all agree. */
    bool verify() const;
private:
    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    int size_ = 0;
};