#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "DoublyLinkedList.h"

using namespace std;

static int fails = 0;

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

void test_empty() {
    DoublyLinkedList l;
    check(l.remove_head() == nullptr, "l.remove_head() == nullptr", __LINE__);
    check(l.remove_node(nullptr) == nullptr, "l.remove_node(nullptr) == nullptr", __LINE__);
    l.clear();
    check(l.to_list().empty(), "l.to_list().empty()", __LINE__);
    check(l.is_empty(), "l.is_empty()", __LINE__);
    check(l.find("x") == nullptr, "l.find(\"x\") == nullptr", __LINE__);
    check(l.verify(), "l.verify()", __LINE__);
}

void test_single() {
    DoublyLinkedList l;
    Node* n = mk("a");
    l.append(n);
    check(l.head() == n && l.tail() == n && l.size() == 1, 
          "l.head() == n && l.tail() == n && l.size() == 1", __LINE__);
    check(l.verify(), "l.verify()", __LINE__);

    Node* out = l.remove_node(n);
    check(out == n, "out == n", __LINE__);
    check(out->prev() == nullptr && out->next() == nullptr, 
          "out->prev() == nullptr && out->next() == nullptr", __LINE__);
    check(l.head() == nullptr && l.tail() == nullptr && l.size() == 0, 
          "l.head() == nullptr && l.tail() == nullptr && l.size() == 0", __LINE__);
    delete out;

    // still usable afterwards
    l.append(mk("b"));
    check(l.size() == 1 && l.verify(), "l.size() == 1 && l.verify()", __LINE__);
}

void test_remove_spots() {
    DoublyLinkedList l;
    fill(l, 5); // a b c d e

    Node* h = l.remove_head();
    check(h->url == "a" && l.head()->url == "b" && l.verify(), 
          "h->url == \"a\" && l.head()->url == \"b\" && l.verify()", __LINE__);
    delete h;

    Node* t = l.remove_node(l.tail());
    check(t->url == "e" && l.tail()->url == "d" && l.verify(), 
          "t->url == \"e\" && l.tail()->url == \"d\" && l.verify()", __LINE__);
    delete t;

    Node* m = l.remove_node(l.find("c"));
    check(m->url == "c" && l.size() == 2 && l.verify(), 
          "m->url == \"c\" && l.size() == 2 && l.verify()", __LINE__);
    check(l.head()->next() == l.tail() && l.tail()->prev() == l.head(), 
          "l.head()->next() == l.tail() && l.tail()->prev() == l.head()", __LINE__);
    delete m;
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
    check(e.size() == 1