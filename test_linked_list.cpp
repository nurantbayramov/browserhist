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

void test_null_inputs() {
    DoublyLinkedList l;
    fill(l, 2); // a b
    l.append(nullptr);
    l.push_front(nullptr);
    l.insert_after(l.head(), nullptr);
    check(l.size() == 2 && l.verify(), "null: null nodes are ignored");
    check(l.truncate_after(nullptr) == 0, "null: truncate_after(nullptr) removes nothing");
    check(l.size() == 2 && l.verify(), "null: list unchanged after truncate_after(nullptr)");
}

void test_insert_after_null_ref() {
    DoublyLinkedList l;
    fill(l, 2); // a b
    l.insert_after(nullptr, mk("z"));
    check(l.tail()->url == "z" && l.size() == 3 && l.verify(), "insert_after: nullptr ref appends to a non-empty list");
}

void test_remove_head_until_empty() {
    DoublyLinkedList l;
    fill(l, 3); // a b c
    for (char c = 'a'; c <= 'c'; c++) {
        Node* h = l.remove_head();
        check(h != nullptr && h->url == string(1, c), "remove_head: oldest comes out first");
        delete h;
        check(l.verify(), "remove_head: verify after each removal");
    }
    check(l.is_empty() && l.head() == nullptr && l.tail() == nullptr, "remove_head: list is empty at the end");
    check(l.remove_head() == nullptr, "remove_head: nullptr once empty");
    l.append(mk("x"));
    check(l.size() == 1 && l.verify(), "remove_head: list reusable after emptying");
}

void test_clear_and_reuse() {
    DoublyLinkedList l;
    fill(l, 4);
    l.clear();
    check(l.is_empty() && l.head() == nullptr && l.tail() == nullptr && l.verify(), "clear: list is empty");
    l.clear(); // a second clear must be harmless
    check(l.verify(), "clear: second clear is harmless");
    fill(l, 2);
    check(l.size() == 2 && l.head()->url == "a" && l.tail()->url == "b" && l.verify(), "clear: list reusable");
}

// what delete + restore does: take a node out, then put the same node back
void test_restore_same_node() {
    DoublyLinkedList l;
    fill(l, 3); // a b c
    Node* b = l.find("b");
    int oldId = b->id;
    Node* before = b->prev();
    l.remove_node(b);
    check(l.size() == 2 && l.verify(), "restore: size and links after removing b");
    l.insert_after(before, b);
    vector<Node*> v = l.to_list();
    check(v.size() == 3 && v[0]->url == "a" && v[1]->url == "b" && v[2]->url == "c", "restore: b is back between a and c");
    check(b->id == oldId, "restore: node keeps its id");
    check(l.verify(), "restore: verify after putting b back");
    Node* a = l.remove_head(); // head goes back with push_front
    l.push_front(a);
    check(l.head() == a && l.size() == 3 && l.verify(), "restore: head goes back at the front");
    Node* c = l.remove_node(l.tail()); // tail goes back after the new tail
    l.insert_after(l.tail(), c);
    check(l.tail() == c && l.size() == 3 && l.verify(), "restore: tail goes back at the end");
}

void test_ids_never_repeat() {
    DoublyLinkedList l;
    fill(l, 3);
    int lastId = l.tail()->id;
    delete l.remove_node(l.tail());
    Node* n = mk("new");
    l.append(n);
    check(n->id > lastId, "ids: a new node gets a bigger id than every earlier one");
    vector<Node*> v = l.to_list();
    bool ok = true;
    for (size_t i = 0; i < v.size(); i++)
        for (size_t j = i + 1; j < v.size(); j++)
            if (v[i]->id == v[j]->id) ok = false;
    check(ok, "ids: no two nodes share an id");
}

void test_stack_growth() {
    Stack<int> s;
    for (int i = 0; i < 100; i++) s.push(i * 2);
    check(s.size() == 100, "stack growth: size after 100 pushes");
    bool ok = true;
    for (int i = 99; i >= 0; i--) {
        optional<int> v = s.pop();
        if (!v.has_value() || v.value() != i * 2) ok = false;
    }
    check(ok, "stack growth: all 100 values come back in reverse order");
    check(s.is_empty(), "stack growth: empty after popping everything");
    s.push(7); // still usable after emptying
    check(s.size() == 1 && s.peek().value() == 7, "stack growth: reusable after emptying");
}

void test_stack_other_types() {
    Stack<string> s;
    s.push("a");
    s.push("b");
    check(s.peek().value() == "b", "stack<string>: peek returns the top");
    check(s.size() == 2, "stack<string>: peek does not remove");
    check(s.pop().value() == "b" && s.pop().value() == "a", "stack<string>: LIFO order");
    // the delete/restore stack may hold node pointers
    Stack<Node*> p;
    Node* x = mk("x");
    Node* y = mk("y");
    p.push(x);
    p.push(y);
    Node* top = p.pop().value();
    check(top == y, "stack<Node*>: last pushed comes out first");
    delete top;
    delete p.pop().value();
    check(p.is_empty(), "stack<Node*>: empty after popping both");
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
    test_null_inputs();
    test_insert_after_null_ref();
    test_remove_head_until_empty();
    test_clear_and_reuse();
    test_restore_same_node();
    test_ids_never_repeat();
    test_stack_growth();
    test_stack_other_types();
    if (fails) cout << fails << " check(s) failed\n";
    else cout << "all good\n";
    return fails != 0;
}