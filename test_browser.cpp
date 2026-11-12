#include <sstream>
#include "BrowserHistory.h"
#include <iostream>
#include <cassert>
#include <stdexcept>
using namespace std;

int main() {
    // Test 1: Constructor
    BrowserHistory browser("ada.edu.az", 3);
    assert(browser.current_page()->url == "ada.edu.az");
    cout << "Test 1 passed" << endl;

    // Test 2: Visit
    browser.visit("github.com");
    browser.visit("spotify.com");
    assert(browser.current_page()->url == "spotify.com");
    cout << "Test 2 passed" << endl;

    // Test 3: Back and forward
    assert(browser.back(1) == "github.com");
    assert(browser.forward(1) == "spotify.com");
    assert(browser.back(100) == "ada.edu.az");
    assert(browser.forward(100) == "spotify.com");
    cout << "Test 3 passed" << endl;

    // Test 4: Branching
    browser.back(1);
    browser.visit("youtube.com");
    assert(browser.current_page()->url == "youtube.com");
    assert(browser.forward(1) == "youtube.com");
    cout << "Test 4 passed" << endl;

    // Test 5: Capacity
    browser.visit("reddit.com");
    assert(browser.back(100) == "github.com");
    cout << "Test 5 passed" << endl;

    // Test 6: Current page
    Node* page = browser.current_page();
    assert(page != nullptr);
    assert(page->url == "github.com");
    assert(browser.current_page() == page);
    cout << "Test 6 passed" << endl;

    // Test 7: Delete history
    browser.delete_history();
    assert(browser.current_page() == nullptr);
    cout << "Test 7 passed" << endl;

    // Test 8: Visit after delete
    browser.visit("ada.edu.az");
    assert(browser.current_page()->url == "ada.edu.az");
    cout << "Test 8 passed" << endl;

    // Test 9: Invalid URL
    bool error = false;
    try {
        browser.visit("");
    } catch (const invalid_argument&) {
        error = true;
    }
    assert(error);
    cout << "Test 9 passed" << endl;

    // Test 10: Negative steps
    error = false;
    try {
        browser.back(-1);
    } catch (const invalid_argument&) {
        error = true;
    }
    assert(error);
    cout << "Test 10 passed" << endl;

    // Test 11: Capacity 1
    BrowserHistory smallBrowser("ada.edu.az", 1);
    smallBrowser.visit("github.com");
    smallBrowser.visit("spotify.com");

    assert(smallBrowser.current_page()->url == "spotify.com");
    assert(smallBrowser.back(100) == "spotify.com");
    cout << "Test 11 passed" << endl;

    // Test 12: Negative forward steps
    error = false;
    try {
        browser.forward(-1);
    } catch (const invalid_argument&) {
        error = true;
    }
    assert(error);
    cout << "Test 12 passed" << endl;

    // Test 13: Empty history
    browser.delete_history();

    assert(browser.current_page() == nullptr);
    assert(browser.back(1) == "");
    assert(browser.forward(1) == "");
    cout << "Test 13 passed" << endl;

    // Test 14: Visit after deleting history
    browser.visit("github.com");

    assert(browser.current_page()->url == "github.com");
    cout << "Test 14 passed" << endl;

    // Test 15: Zero steps
    assert(browser.back(0) == "github.com");
    assert(browser.forward(0) == "github.com");
    cout << "Test 15 passed" << endl;


    // Test 16: Display history
    BrowserHistory testBrowser("ada.edu.az", 5);
    testBrowser.visit("github.com");
    testBrowser.visit("spotify.com");
    testBrowser.back(1);

    ostringstream output;
    streambuf* old = cout.rdbuf(output.rdbuf());

    testBrowser.display_history();

    cout.rdbuf(old);

    string result = output.str();

    assert(result.find("1. ada.edu.az") != string::npos);
    assert(result.find("2. github.com [<]") != string::npos);
    assert(result.find("3. spotify.com") != string::npos);

    cout << "Test 16 passed" << endl;


    // Test 17: Find node
    BrowserHistory b("ada.edu.az", 5);
    b.visit("github.com");
    b.visit("spotify.com");

    assert(b._find_node("github.com") != nullptr);
    assert(b._find_node("youtube.com") == nullptr);
    cout << "Test 17 passed" << endl;

    // Test 18: Set current
    Node* found = b._find_node("github.com");
    b._set_current(found);

    assert(b.current_page()->url == "github.com");
    cout << "Test 18 passed" << endl;

    // Test 19: All nodes
    vector<Node*> pages = b._all_nodes();

    assert(pages.size() == 3);
    assert(pages[0]->url == "ada.edu.az");
    assert(pages[1]->url == "github.com");
    assert(pages[2]->url == "spotify.com");

    cout << "Test 19 passed" << endl;

    return 0;
}
