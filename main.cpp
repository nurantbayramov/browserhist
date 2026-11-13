#include <iostream>
#include <string>
#include <ctime>
#include <stdexcept>
#include "BrowserHistory.h"

using namespace std;

// Menu program for the browser history.
// Run it as:  ./browser          (capacity 50)
//        or:  ./browser 5        (capacity 5, easier to show pruning)

// removes spaces from the start and the end
string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) {
        return "";
    }
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// prints the prompt and reads one line. Returns false if input ended (Ctrl+D / Ctrl+Z)
bool read_line(const string& prompt, string& out) {
    cout << prompt;
    if (!getline(cin, out)) {
        return false;
    }
    out = trim(out);
    return true;
}

// turns text into an int, false if it is not a whole number like "3"
bool to_number(const string& text, int& number) {
    try {
        size_t used = 0;
        number = stoi(text, &used);
        return used == text.size();
    } catch (const exception&) {
        return false; // "abc" or a number that is too big
    }
}

bool ask_yes_no(const string& question) {
    string answer;
    if (!read_line(question + " (y/n): ", answer)) {
        return false;
    }
    return answer == "y" || answer == "Y" || answer == "yes";
}

string format_time(time_t t) {
    tm* local = localtime(&t);
    if (local == nullptr) {
        return "unknown";
    }
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", local);
    return buffer;
}

void print_menu(const BrowserHistory& browser) {
    Node* page = browser.current_page();
    cout << "\n========== Browser History ==========" << endl;
    if (page != nullptr) {
        cout << "You are on: " << page->url << endl;
    } else {
        cout << "You are on: (no page, history is empty)" << endl;
    }
    cout << "-------------------------------------" << endl;
    cout << " 1. Visit a page" << endl;
    cout << " 2. Go back" << endl;
    cout << " 3. Go forward" << endl;
    cout << " 4. Show current page" << endl;
    cout << " 5. Show history" << endl;
    cout << " 6. Delete a page from history" << endl;
    cout << " 7. Restore last deleted page" << endl;
    cout << " 8. Export history to a file" << endl;
    cout << " 9. Import history from a file" << endl;
    cout << "10. Delete all history" << endl;
    cout << " 0. Exit" << endl;
}

// option 2 and 3, they only differ in which way we move
void navigate(BrowserHistory& browser, bool goBack) {
    if (browser.current_page() == nullptr) {
        cout << "History is empty, visit a page first." << endl;
        return;
    }

    string text;
    if (!read_line("How many steps? ", text)) {
        return;
    }
    int steps;
    if (!to_number(text, steps)) {
        cout << "Please enter a whole number." << endl;
        return;
    }

    Node* before = browser.current_page();
    try {
        string url;
        if (goBack) {
            url = browser.back(steps);
        } else {
            url = browser.forward(steps);
        }

        if (steps > 0 && browser.current_page() == before) {
            if (goBack) {
                cout << "There is no page before this one." << endl;
            } else {
                cout << "There is no page after this one." << endl;
            }
        }
        cout << "Now on: " << url << endl;
    } catch (const invalid_argument& e) {
        cout << "Error: " << e.what() << endl;
    }
}

int main(int argc, char* argv[]) {
    int capacity = 50;
    if (argc > 1) {
        if (!to_number(argv[1], capacity) || capacity < 1) {
            cout << "Capacity must be a positive number, using 50." << endl;
            capacity = 50;
        }
    }

    string homepage;
    if (!read_line("Homepage URL (press Enter for google.com): ", homepage)) {
        return 0;
    }
    if (homepage.empty()) {
        homepage = "google.com";
    }

    BrowserHistory browser(homepage, capacity);
    cout << "Browser started on " << homepage << " (capacity: " << capacity << " pages)" << endl;

    while (true) {
        print_menu(browser);

        string choice;
        if (!read_line("Choose an option: ", choice)) {
            break; // input closed
        }

        if (choice == "1") {
            string url;
            if (!read_line("URL to visit: ", url)) {
                break;
            }
            try {
                browser.visit(url);
                cout << "Now on: " << url << endl;
            } catch (const invalid_argument& e) {
                cout << "Error: " << e.what() << endl;
            }
        }
        else if (choice == "2") {
            navigate(browser, true);
        }
        else if (choice == "3") {
            navigate(browser, false);
        }
        else if (choice == "4") {
            Node* page = browser.current_page();
            if (page == nullptr) {
                cout << "History is empty." << endl;
            } else {
                cout << "URL:     " << page->url << endl;
                cout << "Title:   " << page->title << endl;
                cout << "Visited: " << format_time(page->timestamp) << endl;
            }
        }
        else if (choice == "5") {
            browser.display_history();
        }
        else if (choice == "6") {
            if (browser.current_page() == nullptr) {
                cout << "History is empty, nothing to delete." << endl;
                continue;
            }
            browser.display_history();
            string url;
            if (!read_line("URL to delete: ", url)) {
                break;
            }
            try {
                if (browser.delete_url(url)) {
                    cout << "Deleted " << url << ". You can bring it back with option 7." << endl;
                } else {
                    cout << url << " is not in the history." << endl;
                }
            } catch (const invalid_argument& e) {
                cout << "Error: " << e.what() << endl;
            }
        }
        else if (choice == "7") {
            if (browser.restore_url()) {
                cout << "Page restored. Pages left to restore: " << browser.restore_count() << endl;
                browser.display_history();
            } else {
                cout << "Nothing to restore." << endl;
            }
        }
        else if (choice == "8") {
            string path;
            if (!read_line("File name (press Enter for history.txt): ", path)) {
                break;
            }
            if (path.empty()) {
                path = "history.txt";
            }
            if (browser.export_history(path)) {
                cout << "Saved " << browser._all_nodes().size() << " page(s) to " << path << endl;
            }
        }
        else if (choice == "9") {
            string path;
            if (!read_line("File name (press Enter for history.txt): ", path)) {
                break;
            }
            if (path.empty()) {
                path = "history.txt";
            }
            if (browser.current_page() != nullptr &&
                !ask_yes_no("This will replace your current history. Continue?")) {
                cout << "Import cancelled." << endl;
                continue;
            }
            if (browser.import_history(path)) {
                cout << "Loaded history from " << path << endl;
                browser.display_history();
            } else {
                cout << "Import failed, history was not changed." << endl;
            }
        }
        else if (choice == "10") {
            if (ask_yes_no("Delete ALL history? Deleted pages can't be restored after this.")) {
                browser.delete_history();
                cout << "All history deleted." << endl;
            } else {
                cout << "Cancelled." << endl;
            }
        }
        else if (choice == "0") {
            cout << "Bye!" << endl;
            break;
        }
        else {
            cout << "Unknown option, please type a number from the menu." << endl;
        }
    }

    return 0;
}
