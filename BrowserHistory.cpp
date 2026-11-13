#include <iostream>
#include "BrowserHistory.h"
#include <stdexcept>
#include <ctime>
#include <fstream>

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
    deleted.clear(); // "delete everything" also empties the restore stack (team decision)
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


// ---------- Delete and restore ----------

Node* BrowserHistory::find_by_id(int id) const {
    for (Node* page = history.head(); page != nullptr; page = page->next()) {
        if (page->id == id) {
            return page;
        }
    }
    return nullptr;
}

bool BrowserHistory::delete_url(const string& url) {
    if (url.empty()) {
        throw invalid_argument("URL cannot be empty");
    }

    Node* page = history.find(url);
    if (page == nullptr) {
        return false; // not in history, nothing goes on the stack
    }

    // if we delete the page we are on, the cursor has to move somewhere else
    if (page == current) {
        if (page->prev() != nullptr) {
            current = page->prev();
        } else {
            current = page->next(); // becomes nullptr if this was the only page
        }
    }

    // copy the data before the node is deleted
    SavedPage saved;
    saved.url = page->url;
    saved.title = page->title;
    saved.timestamp = page->timestamp;
    saved.id = page->id;
    if (page->prev() != nullptr) {
        saved.prev_id = page->prev()->id;
    } else {
        saved.prev_id = 0; // it was the first page
    }

    history.remove_node(page); // only unlinks it, we still have to free it
    delete page;

    deleted.push(saved);
    return true;
}

bool BrowserHistory::restore_url() {
    optional<SavedPage> top = deleted.pop();
    if (!top.has_value()) {
        return false; // stack is empty
    }
    SavedPage saved = top.value();

    Node* page = new Node(saved.url, saved.title, saved.timestamp);
    // give it back its old id, so pages deleted before it can still find it as their prev
    page->id = saved.id;

    if (saved.prev_id == 0) {
        history.push_front(page); // it was the first page
    } else {
        Node* before = find_by_id(saved.prev_id);
        if (before != nullptr) {
            history.insert_after(before, page);
        } else {
            // the page before it is gone (deleted, pruned or cut by a visit), so put it at the end
            history.append(page);
        }
    }

    // if the history was empty, the restored page becomes the current page
    if (current == nullptr) {
        current = page;
    }

    // restoring can go over the capacity, then remove the oldest page like visit() does
    if (history.size() > capacity) {
        Node* oldest = history.remove_head();
        if (oldest == current) {
            current = history.head();
        }
        delete oldest;
    }

    return true;
}

int BrowserHistory::restore_count() const {
    return deleted.size();
}


// ---------- Export and import ----------

bool BrowserHistory::export_history(const string& path) const {
    ofstream file(path);
    if (!file.is_open()) {
        cerr << "Error: could not open " << path << " for writing" << endl;
        return false;
    }

    vector<Node*> pages = history.to_list();

    // index of the current page, -1 if history is empty
    int cursor = -1;
    for (int i = 0; i < (int)pages.size(); i++) {
        if (pages[i] == current) {
            cursor = i;
        }
    }

    file << "#CURSOR " << cursor << "\n";
    for (Node* page : pages) {
        file << page->url << "\t" << page->title << "\t" << page->timestamp << "\n";
    }

    file.close();
    if (file.fail()) {
        cerr << "Error: something went wrong while writing " << path << endl;
        return false;
    }
    return true;
}

// Splits one line "url<TAB>title<TAB>timestamp" into a SavedPage.
// Returns false if the line is not in that format.
static bool parse_page_line(const string& line, SavedPage& page) {
    size_t tab1 = line.find('\t');
    if (tab1 == string::npos) {
        return false;
    }
    size_t tab2 = line.find('\t', tab1 + 1);
    if (tab2 == string::npos) {
        return false;
    }
    if (line.find('\t', tab2 + 1) != string::npos) {
        return false; // more than 3 columns
    }

    string url = line.substr(0, tab1);
    string title = line.substr(tab1 + 1, tab2 - tab1 - 1);
    string timeText = line.substr(tab2 + 1);

    if (url.empty() || timeText.empty()) {
        return false;
    }

    long long ts = 0;
    try {
        size_t used = 0;
        ts = stoll(timeText, &used);
        if (used != timeText.size()) {
            return false; // like "123abc"
        }
    } catch (const exception&) {
        return false; // not a number at all, or too big
    }
    if (ts < 0) {
        return false;
    }

    page.url = url;
    if (title.empty()) {
        page.title = url;
    } else {
        page.title = title;
    }
    page.timestamp = (time_t)ts;
    return true;
}

bool BrowserHistory::import_history(const string& path) {
    ifstream file(path);
    if (!file.is_open()) {
        cerr << "Error: could not open " << path << endl;
        return false;
    }

    vector<SavedPage> pages;  // valid pages we read, in file order
    bool hasHeader = false;
    int cursorLine = -1;      // cursor index written in the header
    int cursorPos = -1;       // where that page ended up in "pages"
    int pageLineCount = 0;    // counts page lines (also the bad ones) so the index matches the export
    int lineNumber = 0;
    string line;

    while (getline(file, line)) {
        lineNumber++;

        // a file saved on Windows has \r at the end of each line
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) {
            continue;
        }

        // header line: #CURSOR <index>
        if (line.substr(0, 7) == "#CURSOR") {
            if (hasHeader || pageLineCount > 0) {
                cerr << "Warning: line " << lineNumber << ": extra #CURSOR line, skipped" << endl;
                continue;
            }
            hasHeader = true;
            string number = line.substr(7);
            try {
                size_t used = 0;
                cursorLine = stoi(number, &used);
                if (used != number.size()) {
                    throw invalid_argument("bad cursor");
                }
            } catch (const exception&) {
                cerr << "Warning: line " << lineNumber << ": bad cursor value, will use the last page" << endl;
                cursorLine = -1;
            }
            continue;
        }

        // page line
        int thisPage = pageLineCount;
        pageLineCount++;

        SavedPage page;
        if (!parse_page_line(line, page)) {
            cerr << "Warning: line " << lineNumber << " is not url<TAB>title<TAB>timestamp, skipped" << endl;
            continue;
        }
        if (thisPage == cursorLine) {
            cursorPos = pages.size();
        }
        pages.push_back(page);
    }

    if (pages.empty()) {
        cerr << "Error: no valid pages in " << path << ", history was not changed" << endl;
        return false;
    }

    if (!hasHeader) {
        cerr << "Warning: no #CURSOR line, cursor goes to the last page" << endl;
    } else if (cursorPos == -1 && cursorLine != -1) {
        cerr << "Warning: cursor does not point to a valid page, cursor goes to the last page" << endl;
    }
    if (cursorPos == -1) {
        cursorPos = pages.size() - 1;
    }

    // too many pages for the capacity: drop the oldest ones
    int skip = 0;
    if ((int)pages.size() > capacity) {
        skip = pages.size() - capacity;
        cerr << "Warning: file has " << pages.size() << " pages but capacity is " << capacity
             << ", the oldest " << skip << " were not loaded" << endl;
    }
    if (cursorPos < skip) {
        cursorPos = skip; // the cursor page was dropped, use the oldest page we kept
    }

    // everything is read, now replace the old history
    delete_history();
    for (int i = skip; i < (int)pages.size(); i++) {
        Node* node = new Node(pages[i].url, pages[i].title, pages[i].timestamp);
        history.append(node);
        if (i == cursorPos) {
            current = node;
        }
    }

    return true;
}





