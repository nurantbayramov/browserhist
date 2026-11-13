#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <stdexcept>
#include "BrowserHistory.h"

using namespace std;

// Prepared demo for the video. It goes through every feature step by step.
// Press Enter to go to the next step (so we can talk while recording).

void title(const string& text) {
    cout << "\n==================================================" << endl;
    cout << text << endl;
    cout << "==================================================" << endl;
}

void wait_enter() {
    cout << "\n(press Enter to continue)";
    string dummy;
    getline(cin, dummy);
}

void show(const BrowserHistory& b) {
    b.display_history();
    Node* page = b.current_page();
    if (page != nullptr) {
        cout << "current page: " << page->url << endl;
    } else {
        cout << "current page: none" << endl;
    }
}

// prints a text file line by line, tabs shown as -> so they are visible
void print_file(const string& path) {
    ifstream file(path);
    string line;
    while (getline(file, line)) {
        string shown;
        for (char c : line) {
            if (c == '\t') {
                shown += "  ->  ";
            } else {
                shown += c;
            }
        }
        cout << "    " << shown << endl;
    }
}

int main() {
    title("1. Start the browser (homepage google.com, capacity 5)");
    BrowserHistory b("google.com", 5);
    show(b);
    wait_enter();

    title("2. Visit some pages");
    b.visit("github.com");
    b.visit("youtube.com");
    b.visit("ada.edu.az");
    show(b);
    wait_enter();

    title("3. back(2) and forward(1)");
    cout << "back(2)    -> " << b.back(2) << endl;
    cout << "forward(1) -> " << b.forward(1) << endl;
    show(b);
    wait_enter();

    title("4. Going too far: back(100) and forward(100) stop at the ends");
    cout << "back(100)    -> " << b.back(100) << endl;
    cout << "forward(100) -> " << b.forward(100) << endl;
    cout << "back(0)      -> " << b.back(0) << "  (0 steps = stay)" << endl;
    wait_enter();

    title("5. Branching: go back 2 pages, then visit a new page");
    cout << "back(2) -> " << b.back(2) << endl;
    show(b);
    cout << "\nvisit(stackoverflow.com)" << endl;
    b.visit("stackoverflow.com");
    show(b);
    cout << "youtube.com and ada.edu.az are gone (forward history was cleared)" << endl;
    cout << "forward(1) -> " << b.forward(1) << " (nothing after it)" << endl;
    wait_enter();

    title("6. Capacity 5: visit more pages, the oldest one is removed");
    b.visit("wikipedia.org");
    b.visit("linkedin.com");
    show(b);
    cout << "\nvisit(netflix.com) makes it 6 pages, so google.com is pruned:" << endl;
    b.visit("netflix.com");
    show(b);
    wait_enter();

    title("7. current_page() shows url and metadata, it does not move anything");
    Node* page = b.current_page();
    cout << "url: " << page->url << ", title: " << page->title
         << ", timestamp: " << page->timestamp << ", id: " << page->id << endl;
    wait_enter();

    title("8. Delete pages (they go on a stack)");
    cout << "back(1) -> " << b.back(1) << endl;
    cout << "delete stackoverflow.com (middle page)" << endl;
    b.delete_url("stackoverflow.com");
    show(b);
    cout << "\ndelete linkedin.com (this is the current page, cursor moves back)" << endl;
    b.delete_url("linkedin.com");
    show(b);
    cout << "\ndelete facebook.com (not in history) -> "
         << (b.delete_url("facebook.com") ? "true" : "false") << endl;
    cout << "pages on the restore stack: " << b.restore_count() << endl;
    wait_enter();

    title("9. Restore (last deleted comes back first, in its old place)");
    b.restore_url();
    cout << "restore -> linkedin.com is back after wikipedia.org" << endl;
    show(b);
    b.restore_url();
    cout << "\nrestore -> stackoverflow.com is back after github.com" << endl;
    show(b);
    cout << "\nrestore with an empty stack -> " << (b.restore_url() ? "true" : "false") << endl;
    wait_enter();

    title("10. Export history to demo_history.txt");
    b.export_history("demo_history.txt");
    cout << "file content (-> is a TAB):" << endl;
    print_file("demo_history.txt");
    wait_enter();

    title("11. Delete all history");
    b.delete_history();
    show(b);
    cout << "back(1) on empty history returns \"" << b.back(1) << "\"" << endl;
    wait_enter();

    title("12. Import the file back");
    b.import_history("demo_history.txt");
    show(b);
    cout << "same pages and same cursor as before export" << endl;
    wait_enter();

    title("13. Wrong input is rejected");
    try {
        b.visit("");
    } catch (const invalid_argument& e) {
        cout << "visit(\"\")  -> error: " << e.what() << endl;
    }
    try {
        b.back(-1);
    } catch (const invalid_argument& e) {
        cout << "back(-1)   -> error: " << e.what() << endl;
    }
    cout << "import of a missing file:" << endl;
    bool ok = b.import_history("no_such_file.txt");
    cout << "import returned " << (ok ? "true" : "false") << ", history did not change:" << endl;
    show(b);

    remove("demo_history.txt");
    cout << "\nDemo finished." << endl;
    return 0;
}
