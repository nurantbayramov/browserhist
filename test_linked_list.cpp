#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "DoublyLinkedList.h"
#include "Stack.h"

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
    check(e.size() == 1 && e.verify(), "e.size() == 1 && e.verify()", __LINE__);
}

void test_push_front() {
    DoublyLinkedList l;
    l.push_front(mk("b"));
    l.push_front(mk("a"));
    check(l.head()->url == "a" && l.tail()->url == "b" && l.verify(), 
          "l.head()->url == \"a\" && l.tail()->url == \"b\" && l.verify()", __LINE__);
}

void test_truncate() {
    DoublyLinkedList l;
    fill(l, 5);
    check(l.truncate_after(l.tail()) == 0, "l.truncate_after(l.tail()) == 0", __LINE__);
    check(l.size() == 5 && l.verify(), "l.size() == 5 && l.verify()", __LINE__);

    check(l.truncate_after(l.find("c")) == 2, "l.truncate_after(l.find(\"c\")) == 2", __LINE__);
    check(l.tail()->url == "c" && l.size() == 3 && l.verify(), 
          "l.tail()->url == \"c\" && l.size() == 3 && l.verify()", __LINE__);

    check(l.truncate_after(l.head()) == 2, "l.truncate_after(l.head()) == 2", __LINE__);
    check(l.tail() == l.head() && l.size() == 1 && l.verify(), 
          "l.tail() == l.head() && l.size() == 1 && l.verify()", __LINE__);
}

void test_find_and_ids() {
    DoublyLinkedList l;
    l.append(mk("dup"));
    l.append(mk("dup"));
    check(l.find("dup") == l.head(), "l.find(\"dup\") == l.head()", __LINE__); // first one wins
    check(l.head()->id != l.tail()->id, "l.head()->id != l.tail()->id", __LINE__);
}

void test_big() {
    DoublyLinkedList l;
    vector<Node*> mirror;
    for (int i = 0; i < 1000; i++) {
        Node* n = mk(to_string(i));
        l.append(n);
        mirror.push_back(n);
    }
    check(l.size() == 1000, "l.size() == 1000", __LINE__);

    srand(42);
    for (int i = 0; i < 500; i++) {
        int k = rand() % mirror.size();
        delete l.remove_node(mirror[k]);
        mirror.erase(mirror.begin() + k);
        if (l.size() != (int)l.to_list().size()) { 
            check(false, "l.size() != (int)l.to_list().size()", __LINE__); 
            break; 
        }
    }
    check(l.size() == 500, "l.size() == 500", __LINE__);
    check(l.verify(), "l.verify()", __LINE__);
    check(l.to_list() == mirror, "l.to_list() == mirror", __LINE__); // same order as what we kept
}

void test_stack() {
    Stack<int> s;
    check(s.is_empty(), "s.is_empty()", __LINE__);
    check(!s.pop().has_value(), "!s.pop().has_value()", __LINE__);
    check(!s.peek().has_value(), "!s.peek().has_value()", __LINE__);

    for (int i = 0; i < 20; i++) s.push(i); // forces a few grows
    check(s.size() == 20, "s.size() == 20", __LINE__);
    check(s.peek().value() == 19, "s.peek().value() == 19", __LINE__);
    for (int i = 19; i >= 0; i--) check(s.pop().value() == i, "s.pop().value() == i", __LINE__);
    check(s.is_empty(), "s.is_empty()", __LINE__);

    s.push(1);
    s.push(2);
    s.clear();
    check(s.is_empty() && !s.pop().has_value(), "s.is_empty() && !s.pop().has_value()", __LINE__);
}

int main() {
    test_empty();
    test_single();
    test_remove_spots();
    test_insert_after();
    test_push_front();
    test_truncate();
    test_find_and_ids();
    test_big();
    test_stack();

    if (fails) cout << fails << " check(s) failed\n";
    else cout << "all good\n";
    return fails != 0;
}