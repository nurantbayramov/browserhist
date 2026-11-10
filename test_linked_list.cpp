#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "DoublyLinkedList.h"
#include "Stack.h"

using namespace std;

static int fails = 0;

// prints a message and counts a failure when ok is false
static void check(bool ok, const string& what) {
    if (!ok) {
        fails++;
        cout << "FAIL: " << what << "\n";
    }
}

static Node* mk(const string& url) { return new Node(url, url, 0); }

// builds a, b, c, ... up to n pages
static void fill(DoublyLinkedList& l, int n) {
    for (int i = 0; i < n; i++) l.append(mk(string(1, 'a' + i)));
}

void test_empty() {
    DoublyLinkedList l;
    check(l.remove_head() == nullptr, "empty: remove_head returns nullptr");
    check(l.remove_node(nullptr) == nullptr, "empty: remove_node(nullptr) returns nullptr");
    l.clear();
    check(l.to_list().empty(), "empty: to_list is empty");
    check(l.is_empty(), "empty: is_empty");
    check(l.find("x") == nullptr, "empty: find returns nullptr");
    check(l.verify(), "empty: verify");
}

void test_single() {
    DoublyLinkedList l;
    Node* n = mk("a");
    l.append(n);
    check(l.head() == n && l.tail() == n && l.size() == 1, "single: head, tail and size after append");
    check(l.verify(), "single: verify after append");
    Node* out = l.remove_node(n);
    check(out == n, "single: remove_node returns the same node");
    check(out->prev() == nullptr && out->next() == nullptr, "single: removed node has no links");
    check(l.head() == nullptr && l.tail() == nullptr && l.size() == 0, "single: list is empty after remove");
    delete out;
    // still usable afterwards
    l.append(mk("b"));
    check(l.size() == 1 && l.verify(), "single: list reusable after remove");
}

void test_remove_spots() {
    DoublyLinkedList l;
    fill(l, 5); // a b c d e
    Node* h = l.remove_head();
    check(h->url == "a" && l.head()->url == "b" && l.verify(), "remove: head");
    delete h;
    Node* t = l.remove_node(l.tail());
    check(t->url == "e" && l.tail()->url == "d" && l.verify(), "remove: tail");
    delete t;
    Node* m = l.remove_node(l.find("c"));
    check(m->url == "c" && l.size() == 2 && l.verify(), "remove: middle");
    check(l.head()->next() == l.tail() && l.tail()->prev() == l.head(), "remove: neighbors relinked");
    delete m;
}

void test_insert_after() {
    DoublyLinkedList l;
    fill(l, 3); // a b c
    Node* z = mk("z");
    l.insert_after(l.tail(), z);
    check(l.tail() == z && z->prev()->url == "c" && l.verify(), "insert_after: tail");
    Node* y = mk("y");
    l.insert_after(l.head(), y);
    check(l.head()->next() == y && y->next()->url == "b" && l.verify(), "insert_after: middle");
    check(l.size() == 5, "insert_after: size");
    DoublyLinkedList e;
    e.insert_after(nullptr, mk("only"));
    check(e.size() == 1 && e.verify(), "insert_after: nullptr ref on empty list");
}

void test_push_front() {
    DoublyLinkedList l;
    l.push_front(mk("b"));
    l.push_front(mk("a"));
    check(l.head()->url == "a" && l.tail()->url == "b" && l.verify(), "push_front: order");
}

void test_truncate() {
    DoublyLinkedList l;
    fill(l, 5);
    check(l.truncate_after(l.tail()) == 0, "truncate: tail removes nothing");
    check(l.size() == 5 && l.verify(), "truncate: list unchanged after tail");
    check(l.truncate_after(l.find("c")) == 2, "truncate: after c removes 2");
    check(l.tail()->url == "c" && l.size() == 3 && l.verify(), "truncate: c is the new tail");
    check(l.truncate_after(l.head()) == 2, "truncate: after head removes 2");
    check(l.tail() == l.head() && l.size() == 1 && l.verify(), "truncate: only head left");
}

void test_find_and_ids() {
    DoublyLinkedList l;
    l.append(mk("dup"));
    l.append(mk("dup"));
    check(l.find("dup") == l.head(), "find: first duplicate wins");
    check(l.head()->id != l.tail()->id, "ids: unique per node");
}

void test_big() {
    DoublyLinkedList l;
    vector<Node*> mirror;
    for (int i = 0; i < 1000; i++) {
        Node* n = mk(to_string(i));
        l.append(n);
        mirror.push_back(n);
    }
    check(l.size() == 1000, "big: size after 1000 appends");
    srand(42);
    for (int i = 0; i < 500; i++) {
        int k = rand() % mirror.size();
        delete l.remove_node(mirror[k]);
        mirror.erase(mirror.begin() + k);
        if (l.size() != (int)l.to_list().size()) {
            check(false, "big: size matches to_list while removing");
            break;
        }
    }
    check(l.size() == 500, "big: size after 500 removals");
    check(l.verify(), "big: verify");
    check(l.to_list() == mirror, "big: same order as the nodes we kept");
}

void test_stack() {
    Stack<int> s;
    check(s.is_empty(), "stack: starts empty");
    check(!s.pop().has_value(), "stack: pop on empty");
    check(!s.peek().has_value(), "stack: peek on empty");
    for (int i = 0; i < 20; i++) s.push(i); // forces a few grows
    check(s.size() == 20, "stack: size after 20 pushes");
    check(s.peek().value() == 19, "stack: peek returns the top");
    for (int i = 19; i >= 0; i--) check(s.pop().value() == i, "stack: pops in LIFO order");
    check(s.is_empty(), "stack: empty after popping everything");
    s.push(1);
    s.push(2);
    s.clear();
    check(s.is_empty() && !s.pop().has_value(), "stack: clear empties it");
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