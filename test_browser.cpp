#include "BrowserHistory.h"
#include <iostream>
using namespace std;

int main() {
    BrowserHistory browser("ada.edu.az", 5);

    browser.visit("github.com");
    browser.visit("spotify.com");

    cout << "Back 1: " << browser.back(1) << endl;
    cout << "Back 1: " << browser.back(1) << endl;
    cout << "Back 100: " << browser.back(100) << endl;

    return 0;
}
