#include <iostream>
#include "BrowserHistory.h"
#include <stdexcept>
#include <ctime>

using namespace std;

BrowserHistory::BrowserHistory(const string& homepage, int capacity) {
    if (homepage.empty()) {
        throw invalid_argument("URL cannot be empty");
    }

    if (capacity < 1) {
        throw invalid_argument("Capacity must be positive");
    }

    this->capacity = capacity;

    current = new Node(homepage, homepage, time(nullptr));
    history.append(current);
}

void BrowserHistory::visit(const string& url) {
    if (url.empty()) {
    throw invalid_argument("URL cannot be empty");
} 
    if (current != nullptr) {
    history.truncate_after(current);
}
    Node* newPage = new Node(url, url, time(nullptr));
    history.append(newPage);
    current = newPage;

    if (history.size() > capacity) {
    Node* oldest = history.remove_head();
    delete oldest;
}
}

string BrowserHistory::back(int steps) {
    if (steps < 0) {
        throw invalid_argument("Steps cannot be negative");
    }

    if (current == nullptr) {
        return "";
    }

    while (steps > 0 && current->prev() != nullptr) {
        current = current->prev();
        steps--;
    }

    return current->url;
}

string BrowserHistory::forward(int steps) {
    if (steps < 0) {
        throw invalid_argument("Steps cannot be negative");
    }

    if (current == nullptr) {
        return "";
    }

    while (steps > 0 && current->next() != nullptr) {
        current = current->next();
        steps--;
    }

    return current->url;
}

Node* BrowserHistory::current_page() const {
    return current;
}

void BrowserHistory::display_history() const {
    vector<Node*> pages = history.to_list();

    if (pages.empty()) {
        cout << "History is empty" << endl;
        return;
    }

    for (size_t i = 0; i < pages.size(); i++) {
        cout << i + 1 << ". " << pages[i]->url;

        if (pages[i] == current) {
            cout << " [<]";
        }

        cout << endl;
    }
}

void BrowserHistory::delete_history() {
    history.clear();
    current = nullptr;
}

Node* BrowserHistory::_find_node(const string& url) const {
    return history.find(url);
}
   
void BrowserHistory::_set_current(Node* node) {
    if (node == nullptr) {
        current = nullptr;
        return;
    }

    vector<Node*> pages = history.to_list();

    for (Node* page : pages) {
        if (page == node) {
            current = node;
            return;
        }
    }

    throw invalid_argument("Node not found in history");
}

vector<Node*> BrowserHistory::_all_nodes() const {
    return history.to_list();
}





