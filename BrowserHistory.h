#pragma once
#include "DoublyLinkedList.h"

class BrowserHistory {
private:
    DoublyLinkedList history;
    Node* current = nullptr;
    int capacity = 50;

public:
    // Constructor 
    BrowserHistory(const string& homepage, int capacity = 50);

    /** Opens a new page and removes forward history. */
    void visit(const string& url);

    /** Moves back by the given number of steps and returns the URL. */
    string back(int steps);

    /** Moves forward by the given number of steps and returns the URL. */
    string forward(int steps);

    /** Returns the current page, or nullptr if history is empty. */
    Node* current_page() const;

    /** Prints all visited pages and marks the current page. */
    void display_history() const;

    /** Deletes all pages from browser history. */
    void delete_history();

    /** Finds a page by its URL. Returns nullptr if not found. */
    Node* _find_node(const string& url) const;

    /** Changes the current page. */
    void _set_current(Node* node);

    /** Returns all pages in history. */
    vector<Node*> _all_nodes() const;

};
