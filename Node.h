#pragma once
#include <ctime>
#include <string>

using namespace std;

class DoublyLinkedList;

class Node {
public:
    string url;
    string title;
    time_t timestamp;
    int id;
    Node(string u, string t, time_t ts)
        : url(u), title(t), timestamp(ts), id(counter++) {}
    /** Read-only. Only the list can change links. */
    Node* prev() const { return prev_; }
    Node* next() const { return next_; }
private:
    friend class DoublyLinkedList;
    Node* prev_ = nullptr;
    Node* next_ = nullptr;
    // ids never get reused, so the restore stack can't match a wrong node
    static inline int counter = 1;
};