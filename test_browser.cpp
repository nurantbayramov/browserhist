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

    return 0;
}
