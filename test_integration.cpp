#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "BrowserHistory.h"

using namespace std;

// Integration tests: use the list, the stack and BrowserHistory together,
// the way the menu program does.

static int fails = 0;

static void check(bool ok, const string& what) {
    if (!ok) {
        fails++;
        cout << "FAIL: " << what << "\n";
    }
}

static string urls(const BrowserHistory& b) {
    string out;
    for (Node* p : b._all_nodes()) {
        if (!out.empty()) out += " ";
        out += p->url;
    }
    return out;
}

static string cur(const BrowserHistory& b) {
    Node* p = b.current_page();
    if (p == nullptr) return "(none)";
    return p->url;
}

static bool links_ok(const BrowserHistory& b) {
    vector<Node*> pages = b._all_nodes();
    for (size_t i = 0; i < pages.size(); i++) {
        Node* before = (i == 0) ? nullptr : pages[i - 1];
        Node* after = (i + 1 == pages.size()) ? nullptr : pages[i + 1];
        if (pages[i]->prev() != before || pages[i]->next() != after) return false;
    }
    return true;
}

// position of the cursor in the list, -1 if there is no cursor
static int cursor_index(const BrowserHistory& b) {
    vector<Node*> pages = b._all_nodes();
    for (int i = 0; i < (int)pages.size(); i++) {
        if (pages[i] == b.current_page()) return i;
    }
    return -1;
}

static string display_text(const BrowserHistory& b) {
    ostringstream out;
    streambuf* old = cout.rdbuf(out.rdbuf());
    b.display_history();
    cout.rdbuf(old);
    return out.str();
}

const string TMP = "test_tmp_integration.txt";

// from the assignment: call back() 100 times on a history of 2 pages
void test_back_100_times() {
    BrowserHistory b("first.com");
    b.visit("second.com");
    for (int i = 0; i < 100; i++) {
        b.back(1);
    }
    check(cur(b) == "first.com", "back x100: stops at the first page");
    for (int i = 0; i < 100; i++) {
        b.forward(1);
    }
    check(cur(b) == "second.com", "forward x100: stops at the last page");
    check(urls(b) == "first.com second.com" && links_ok(b), "back/forward x100: list not changed");
}

// from the assignment: branching by visiting a new page from the middle
void test_branch_from_middle() {
    BrowserHistory b("a", 10);
    b.visit("b");
    b.visit("c");
    b.visit("d");
    b.visit("e");
    b.back(2); // on c
    b.visit("x");
    check(urls(b) == "a b c x", "branch: d and e are gone");
    check(b.forward(1) == "x", "branch: nothing after x");
    b.back(1);
    b.visit("y");
    check(urls(b) == "a b c y" && cur(b) == "y" && links_ok(b), "branch again from c");
}

// a whole session using every feature, like a user in the menu
void test_full_session() {
    BrowserHistory b("google.com", 5);
    b.visit("github.com");
    b.visit("youtube.com");
    b.visit("ada.edu.az");
    b.back(2);                 // github
    b.visit("stackoverflow.com");
    check(urls(b) == "google.com github.com stackoverflow.com", "session: branch");

    b.visit("wiki.org");
    b.visit("linkedin.com");
    b.visit("netflix.com");    // 6 pages -> google pruned
    check(urls(b) == "github.com stackoverflow.com wiki.org linkedin.com netflix.com", "session: capacity prune");

    b.back(1);                 // linkedin
    b.delete_url("linkedin.com");
    check(cur(b) == "wiki.org", "session: deleting current moves cursor back");
    b.delete_url("github.com");
    check(urls(b) == "stackoverflow.com wiki.org netflix.com", "session: two deletes");

    check(b.export_history(TMP), "session: export");
    b.delete_history();
    check(urls(b) == "" && b.restore_count() == 0, "session: delete_history empties everything");

    string text;
    check(b.import_history(TMP), "session: import");
    check(urls(b) == "stackoverflow.com wiki.org netflix.com" && cur(b) == "wiki.org", "session: import gives same state");

    // keep browsing after import
    b.visit("reddit.com");     // cuts netflix
    check(urls(b) == "stackoverflow.com wiki.org reddit.com", "session: visit after import");
    check(b.back(100) == "stackoverflow.com" && b.forward(100) == "reddit.com", "session: navigation after import");
    check(links_ok(b), "session: links ok at the end");
    remove(TMP.c_str());
}

void test_restore_then_navigate() {
    BrowserHistory b("a", 10);
    b.visit("b");
    b.visit("c");
    b.delete_url("b");
    b.restore_url();
    // back and forward must walk through the restored page
    check(b.back(1) == "b" && b.back(1) == "a", "restored page: reachable with back");
    check(b.forward(2) == "c", "restored page: reachable with forward");
}

void test_capacity_one() {
    BrowserHistory b("a", 1);
    b.visit("b");
    check(urls(b) == "b" && cur(b) == "b", "cap 1: visit replaces the page");
    b.delete_url("b");
    check(urls(b) == "" && b.current_page() == nullptr, "cap 1: delete the only page");
    check(b.restore_url() && urls(b) == "b" && cur(b) == "b", "cap 1: restore into empty history");
    b.visit("c");
    check(urls(b) == "c" && links_ok(b), "cap 1: visit after restore still keeps one page");
}

void test_display_marks_right_page() {
    BrowserHistory b("a", 10);
    b.visit("b");
    b.visit("c");
    b.back(1);
    b.delete_url("b");          // cursor -> a
    b.restore_url();            // b back, cursor stays on a
    string text = display_text(b);
    int marks = 0;
    for (size_t pos = text.find("[<]"); pos != string::npos; pos = text.find("[<]", pos + 1)) marks++;
    check(marks == 1, "display: exactly one [<]");
    check(text.find("1. a [<]") != string::npos, "display: [<] is on a");

    b.delete_history();
    check(display_text(b).find("empty") != string::npos, "display: empty message after delete_history");
}

// round trip in the middle of navigation, then compare step by step
void test_save_load_same_navigation() {
    BrowserHistory b("p0", 20);
    for (int i = 1; i < 10; i++) b.visit("p" + to_string(i));
    b.back(4); // on p5
    b.export_history(TMP);

    BrowserHistory c("x", 20);
    c.import_history(TMP);
    bool same = true;
    int moves[] = {1, 3, 2, 10, 4, 1, 7};
    for (int i = 0; i < 7; i++) {
        string one, two;
        if (i % 2 == 0) {
            one = b.back(moves[i]);
            two = c.back(moves[i]);
        } else {
            one = b.forward(moves[i]);
            two = c.forward(moves[i]);
        }
        if (one != two) same = false;
    }
    check(same, "save/load: same answers for the same back/forward calls");
    remove(TMP.c_str());
}

// lots of random operations; after each one the structure must still be correct
void test_random_operations() {
    srand(7);
    const int CAP = 7;
    BrowserHistory b("start.com", CAP);
    string pool[] = {"a.com", "b.com", "c.com", "d.com", "e.com", "f.com", "g.com", "h.com"};
    bool ok = true;
    string problem;

    for (int i = 0; i < 3000 && ok; i++) {
        int op = rand() % 7;
        string url = pool[rand() % 8];
        int steps = rand() % 5;

        if (op == 0 || op == 1) b.visit(url);
        else if (op == 2) b.back(steps);
        else if (op == 3) b.forward(steps);
        else if (op == 4) b.delete_url(url);
        else if (op == 5) b.restore_url();
        else if (rand() % 10 == 0) {
            // sometimes save and load, nothing should change
            string before = urls(b);
            int idx = cursor_index(b);
            if (b.export_history(TMP) && !before.empty()) {
                b.import_history(TMP);
                if (urls(b) != before || cursor_index(b) != idx) {
                    ok = false;
                    problem = "export/import changed the history";
                }
            }
        }

        vector<Node*> pages = b._all_nodes();
        if (!links_ok(b)) { ok = false; problem = "broken links"; }
        if ((int)pages.size() > CAP) { ok = false; problem = "over capacity"; }
        if (pages.empty() != (b.current_page() == nullptr)) { ok = false; problem = "cursor and empty history disagree"; }
        if (!pages.empty() && cursor_index(b) == -1) { ok = false; problem = "cursor not in the list"; }
        if (b.restore_count() < 0) { ok = false; problem = "bad stack size"; }
        if (!ok) cout << "  random test stopped at step " << i << ": " << problem << "\n";
    }
    check(ok, "random operations: history stays correct after 3000 steps");
    remove(TMP.c_str());
}

int main() {
    test_back_100_times();
    test_branch_from_middle();
    test_full_session();
    test_restore_then_navigate();
    test_capacity_one();
    test_display_marks_right_page();
    test_save_load_same_navigation();
    test_random_operations();
    if (fails) cout << fails << " check(s) failed\n";
    else cout << "all integration tests passed\n";
    return fails != 0;
}
