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
    
};
