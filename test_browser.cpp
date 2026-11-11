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

    cout << "All tests passed!" << endl;
    return 0;
}
