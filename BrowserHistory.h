#pragma once
#include "DoublyLinkedList.h"
#include "Stack.h"

// Copy of a page's data. Used for the restore stack (a deleted node is freed,
// so we keep its data here instead of a pointer) and while reading a file.
struct SavedPage {
    string url;
    string title;
    time_t timestamp = 0;
    int id = 0;       // id the page had before it was deleted
    int prev_id = 0;  // id of the page that was right before it, 0 = it was the first page
};

class BrowserHistory {
private:
    DoublyLinkedList history;
    Node* current = nullptr;
    int capacity = 50;
    Stack<SavedPage> deleted;   // deleted pages, last deleted on top

    // finds a page by its id, nullptr if it is not in the history anymore
    Node* find_by_id(int id) const;

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

    /**
     * Saves history to a text file.
     * First line is "#CURSOR <index>" (index starts from 0, -1 if history is empty),
     * then one page per line: url<TAB>title<TAB>timestamp.
     * Returns false if the file can't be written.
     */
    bool export_history(const string& path) const;

    /**
     * Loads history from a text file made by export_history and REPLACES the
     * current history (restore stack is cleared too).
     * Bad lines are skipped with a warning. If the cursor in the header is
     * missing or wrong, the cursor goes to the last page. If the file has more
     * pages than the capacity, only the newest ones are kept.
     * Returns false if the file can't be opened or has no valid pages,
     * in that case the old history is not touched.
     */
    bool import_history(const string& path);

    /**
     * Deletes the first page with this URL and pushes it on the restore stack.
     * If it was the current page, the cursor moves to the previous page
     * (or the next one if it was the first page, or nullptr if it was the only one).
     * Returns false if the URL is not in history. Throws invalid_argument for an empty URL.
     */
    bool delete_url(const string& url);

    /**
     * Puts back the last deleted page. It goes right after the page that was
     * before it, if that page still exists, otherwise at the end. A page that was
     * first goes back to the front. If this makes history bigger than capacity,
     * the oldest page is removed. Returns false if there is nothing to restore.
     */
    bool restore_url();

    /** How many deleted pages can still be restored. */
    int restore_count() const;

};
