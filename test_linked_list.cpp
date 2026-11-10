#include <cstdlib>
#include <iostream>
#include <string>
#include "DoublyLinkedList.h"

using namespace std;

static int fails = 0;

// Replaced macro with an inline function
inline void check(bool condition, const string& expr, int line) {
    if (!condition) {
        fails++;
        cout << "FAIL line " << line << ": " << expr << "\n";
    }
}

static Node* mk(const string& url) { return new Node(url, url, 0); }

// builds a, b, c, ... up to n pages
static void fill(DoublyLinkedList& l, int n) {
    for (int i = 0; i < n; i++) l.append(mk(string(1, 'a' + i)));
}

void test_insert_after() {
    DoublyLinkedList l;
    fill(l, 3); // a b c

    Node* z = mk("z");
    l.insert_after(l.tail(), z);
    check(l.tail() == z && z->prev()->url == "c" && l.verify(),
          "l.tail() == z && z->prev()->url == \"c\" && l.verify()", __LINE__);

    Node* y = mk("y");
    l.insert_after(l.head(), y);
    check(l.head()->next() == y && y->next()->url == "b" && l.verify(),
          "l.head()->next() == y && y->next()->url == \"b\" && l.verify()", __LINE__);
    check(l.size() == 5, "l.size() == 5", __LINE__);

    DoublyLinkedList e;
    e.insert_after(nullptr, mk("only"));
    check(e.size() == 1 && e.verify(), "e.size() == 1 && e.verify()", __LINE__);
}

void test_push_front() {
    DoublyLinkedList l;
    l.push_front(mk("b"));
    l.push_front(mk("a"));
    check(l.head()->url == "a" && l.tail()->url == "b" && l.verify(),
          "l.head()->url == \"a\" && l.tail()->url == \"b\" && l.verify()", __LINE__);
}

void test_find_and_ids() {
    DoublyLinkedList l;
    l.append(mk("dup"));
    l.append(mk("dup"));
    check(l.find("dup") == l.head(), "l.find(\"dup\") == l.head()", __LINE__); // first one wins
    check(l.head()->id != l.tail()->id, "l.head()->id != l.tail()->id", __LINE__);
}

int main() {
    test_insert_after();
    test_push_front();
    test_find_and_ids();

    if (fails) cout << fails << " check(s) failed\n";
    else cout << "all good\n";
    return fails != 0;
}