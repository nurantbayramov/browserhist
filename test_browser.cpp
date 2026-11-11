
#include "BrowserHistory.h"
#include <iostream>
using namespace std;

int main() {
    BrowserHistory browser("ada.edu.az", 5);

    browser.visit("github.com");
    browser.visit("spotify.com");

    browser.back(2);

    cout << "Forward 1: " << browser.forward(1) << endl;
    cout << "Forward 1: " << browser.forward(1) << endl;
    cout << "Forward 100: " << browser.forward(100) << endl;

    cout << "Current page: " << browser.current_page()->url << endl;

    browser.back(1);

    cout << "After back: " << browser.current_page()->url << endl;

    cout << "\nBrowser history:" << endl;
    browser.display_history();

    browser.delete_history();

    cout << "\nAfter deleting history:" << endl;
    browser.display_history();

    if (browser.current_page() == nullptr) {
        cout << "Current page is empty" << endl;
    }

    return 0;
}
