#include "BrowserHistory.h"
#include <iostream>
using namespace std;

int main() {
    BrowserHistory browser("google.com", 3);

    browser.visit("youtube.com");
    browser.visit("instagram.com");
    browser.visit("github.com");

    cout << "Visit test completed!" << endl;
    return 0;
}
