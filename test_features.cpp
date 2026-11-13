#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "BrowserHistory.h"

using namespace std;

// Tests for export/import and delete/restore.

static int fails = 0;

// prints a message and counts a failure when ok is false
static void check(bool ok, const string& what) {
    if (!ok) {
        fails++;
        cout << "FAIL: " << what << "\n";
    }
}

// all urls head -> tail in one string, like "a b c", easy to compare
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

// same idea as verify(): every page must point to its real neighbours
static bool links_ok(const BrowserHistory& b) {
    vector<Node*> pages = b._all_nodes();
    for (size_t i = 0; i < pages.size(); i++) {
        Node* before = (i == 0) ? nullptr : pages[i - 1];
        Node* after = (i + 1 == pages.size()) ? nullptr : pages[i + 1];
        if (pages[i]->prev() != before || pages[i]->next() != after) return false;
    }
    return true;
}

static void write_file(const string& path, const string& text) {
    ofstream f(path, ios::binary);
    f << text;
}

static string read_file(const string& path) {
    ifstream f(path, ios::binary);
    stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

// runs import but catches what it writes to cerr, so we can check the warnings
static bool import_quiet(BrowserHistory& b, const string& path, string& errors) {
    ostringstream captured;
    streambuf* old = cerr.rdbuf(captured.rdbuf());
    bool ok = b.import_history(path);
    cerr.rdbuf(old);
    errors = captured.str();
    return ok;
}

static int count_word(const string& text, const string& word) {
    int n = 0;
    size_t pos = text.find(word);
    while (pos != string::npos) {
        n++;
        pos = text.find(word, pos + 1);
    }
    return n;
}

const string TMP = "test_tmp_history.txt";

// ---------- export ----------

void test_export_format() {
    BrowserHistory b("a.com", 10);
    b.visit("b.com");
    b.visit("c.com");
    b.back(1); // cursor on b.com, index 1
    check(b.export_history(TMP), "export: returns true");

    string text = read_file(TMP);
    check(text.substr(0, 10) == "#CURSOR 1\n", "export: first line is #CURSOR 1");
    check(text.find("a.com\ta.com\t") != string::npos, "export: page line is url<TAB>title<TAB>time");
    check(count_word(text, "\n") == 4, "export: header + 3 page lines");
    check(text.find("a.com") < text.find("b.com") && text.find("b.com") < text.find("c.com"),
          "export: pages are in order oldest to newest");
    check(urls(b) == "a.com b.com c.com" && cur(b) == "b.com", "export: does not change the history");
    remove(TMP.c_str());
}

void test_export_empty_history() {
    BrowserHistory b("a.com");
    b.delete_history();
    check(b.export_history(TMP), "export empty: returns true");
    check(read_file(TMP) == "#CURSOR -1\n", "export empty: only the header with -1");

    // importing that file has nothing to load, so the other history must stay the same
    BrowserHistory other("x.com");
    other.visit("y.com");
    string errors;
    check(!import_quiet(other, TMP, errors), "export empty: importing it returns false");
    check(urls(other) == "x.com y.com" && cur(other) == "y.com", "export empty: import did not touch the history");
    remove(TMP.c_str());
}

void test_export_bad_path() {
    BrowserHistory b("a.com");
    ostringstream captured;
    streambuf* old = cerr.rdbuf(captured.rdbuf());
    bool ok = b.export_history("no_such_folder/history.txt");
    cerr.rdbuf(old);
    check(!ok, "export bad path: returns false");
    check(captured.str().find("Error") != string::npos, "export bad path: prints an error");
}

// ---------- import ----------

void test_round_trip() {
    BrowserHistory b("a.com", 10);
    b.visit("b.com");
    b.visit("c.com");
    b.visit("d.com");
    b.back(2); // cursor on b.com
    b.export_history(TMP);

    BrowserHistory c("other.com", 10);
    string errors;
    check(import_quiet(c, TMP, errors), "round trip: import returns true");
    check(errors.empty(), "round trip: no warnings for a clean file");
    check(urls(c) == "a.com b.com c.com d.com", "round trip: same urls in the same order");
    check(cur(c) == "b.com", "round trip: same cursor");
    check(links_ok(c), "round trip: links ok");

    vector<Node*> before = b._all_nodes();
    vector<Node*> after = c._all_nodes();
    bool same = before.size() == after.size();
    for (size_t i = 0; same && i < before.size(); i++) {
        if (before[i]->title != after[i]->title || before[i]->timestamp != after[i]->timestamp) same = false;
    }
    check(same, "round trip: titles and timestamps kept");

    // navigation works the same after loading
    check(c.forward(1) == "c.com" && c.back(100) == "a.com" && c.forward(100) == "d.com",
          "round trip: back and forward work on the loaded history");
    remove(TMP.c_str());
}

void test_import_missing_file() {
    BrowserHistory b("a.com");
    b.visit("b.com");
    string errors;
    check(!import_quiet(b, "this_file_does_not_exist.txt", errors), "missing file: returns false");
    check(errors.find("Error") != string::npos, "missing file: prints an error");
    check(urls(b) == "a.com b.com" && cur(b) == "b.com", "missing file: history unchanged");
}

void test_import_empty_file() {
    BrowserHistory b("a.com");
    b.visit("b.com");
    string errors;

    write_file(TMP, "");
    check(!import_quiet(b, TMP, errors), "empty file: returns false");
    check(urls(b) == "a.com b.com" && cur(b) == "b.com", "empty file: history unchanged");

    write_file(TMP, "#CURSOR 0\n\n\n");
    check(!import_quiet(b, TMP, errors), "only header: returns false");
    check(urls(b) == "a.com b.com", "only header: history unchanged");
    remove(TMP.c_str());
}

void test_import_corrupted_lines() {
    // line 1 header, lines 3, 4, 6, 7 are broken
    write_file(TMP,
        "#CURSOR 3\n"
        "a.com\tA\t100\n"
        "this line is broken\n"
        "b.com\tB\tnot_a_number\n"
        "c.com\tC\t300\n"
        "\tno url\t5\n"
        "d.com\tD\t400\textra\n"
        "e.com\tE\t500\n");
    BrowserHistory b("x.com");
    string errors;
    check(import_quiet(b, TMP, errors), "corrupted: still returns true (some lines are fine)");
    check(urls(b) == "a.com c.com e.com", "corrupted: only good lines loaded");
    check(cur(b) == "c.com", "corrupted: cursor 3 still points to c.com (bad lines are counted)");
    check(count_word(errors, "Warning") == 4, "corrupted: one warning per bad line");
    check(errors.find("line 3") != string::npos, "corrupted: warning says the line number");
    check(b._all_nodes()[1]->title == "C" && b._all_nodes()[1]->timestamp == 300, "corrupted: title and time read");
    check(links_ok(b), "corrupted: links ok");
    remove(TMP.c_str());
}

void test_import_cursor_fallbacks() {
    BrowserHistory b("x.com");
    string errors;

    write_file(TMP, "a.com\tA\t1\nb.com\tB\t2\n");
    check(import_quiet(b, TMP, errors) && cur(b) == "b.com", "no header: cursor goes to last page");
    check(errors.find("no #CURSOR") != string::npos, "no header: warning printed");

    write_file(TMP, "#CURSOR abc\na.com\tA\t1\nb.com\tB\t2\n");
    check(import_quiet(b, TMP, errors) && cur(b) == "b.com", "bad cursor text: last page");

    write_file(TMP, "#CURSOR 9\na.com\tA\t1\nb.com\tB\t2\n");
    check(import_quiet(b, TMP, errors) && cur(b) == "b.com", "cursor out of range: last page");

    write_file(TMP, "#CURSOR 0\nbroken\nb.com\tB\t2\nc.com\tC\t3\n");
    check(import_quiet(b, TMP, errors) && cur(b) == "c.com", "cursor on a broken line: last page");

    write_file(TMP, "#CURSOR 0\na.com\tA\t1\nb.com\tB\t2\n");
    check(import_quiet(b, TMP, errors) && cur(b) == "a.com", "cursor 0: first page");
    remove(TMP.c_str());
}

void test_import_windows_line_endings() {
    write_file(TMP, "#CURSOR 0\r\na.com\tA\t1\r\nb.com\tB\t2\r\n");
    BrowserHistory b("x.com");
    string errors;
    check(import_quiet(b, TMP, errors), "windows file: returns true");
    check(urls(b) == "a.com b.com", "windows file: no \\r left in the urls");
    check(cur(b) == "a.com", "windows file: cursor read");
    check(b._all_nodes()[1]->timestamp == 2, "windows file: timestamp read");
    remove(TMP.c_str());
}

void test_import_over_capacity() {
    string text = "#CURSOR 4\n";
    for (int i = 1; i <= 6; i++) {
        text += "p" + to_string(i) + ".com\tP\t" + to_string(i) + "\n";
    }
    write_file(TMP, text);

    BrowserHistory b("x.com", 3);
    string errors;
    check(import_quiet(b, TMP, errors), "over capacity: returns true");
    check(urls(b) == "p4.com p5.com p6.com", "over capacity: only the newest 3 kept");
    check(cur(b) == "p5.com", "over capacity: cursor still on p5");
    check(errors.find("capacity") != string::npos, "over capacity: warning printed");

    // cursor on a page that gets dropped -> oldest kept page
    write_file(TMP, "#CURSOR 0\n" + text.substr(text.find('\n') + 1));
    check(import_quiet(b, TMP, errors) && cur(b) == "p4.com", "over capacity: dropped cursor goes to oldest kept page");
    check(links_ok(b), "over capacity: links ok");
    remove(TMP.c_str());
}

void test_import_replaces_and_clears_stack() {
    write_file(TMP, "#CURSOR 0\nnew1.com\tN\t1\nnew2.com\tN\t2\n");
    BrowserHistory b("a.com");
    b.visit("b.com");
    b.visit("c.com");
    b.delete_url("b.com");
    check(b.restore_count() == 1, "replace: one page on the stack before import");

    string errors;
    check(import_quiet(b, TMP, errors), "replace: import ok");
    check(urls(b) == "new1.com new2.com", "replace: old pages are gone");
    check(b.restore_count() == 0 && !b.restore_url(), "replace: restore stack cleared");
    remove(TMP.c_str());
}

// ---------- delete ----------

void test_delete_spots() {
    BrowserHistory b("a", 10);
    b.visit("b");
    b.visit("c");
    b.visit("d");
    b.visit("e");

    check(b.delete_url("a") && urls(b) == "b c d e" && links_ok(b), "delete: head");
    check(b.delete_url("c") && urls(b) == "b d e" && links_ok(b), "delete: middle");
    check(b.delete_url("e") && urls(b) == "b d" && links_ok(b), "delete: tail");
    check(b.restore_count() == 3, "delete: 3 records on the stack");
}

void test_delete_current_page() {
    BrowserHistory b("a");
    b.visit("b");
    b.visit("c");
    b.back(1); // on b
    b.delete_url("b");
    check(cur(b) == "a", "delete current: cursor moves to the previous page");

    BrowserHistory h("a");
    h.visit("b");
    h.visit("c");
    h.back(2); // on a, the first page
    h.delete_url("a");
    check(cur(h) == "b", "delete current head: cursor moves to the next page");

    BrowserHistory o("a");
    o.visit("b");
    o.visit("c");
    o.delete_url("a");
    check(cur(o) == "c", "delete other page: cursor does not move");
}

void test_delete_only_page() {
    BrowserHistory b("a");
    check(b.delete_url("a"), "delete only page: returns true");
    check(b.current_page() == nullptr && urls(b) == "", "delete only page: history empty, cursor nullptr");
    check(b.back(1) == "" && b.forward(1) == "", "delete only page: back/forward return empty");
    b.visit("z");
    check(urls(b) == "z" && cur(b) == "z", "delete only page: can visit again after");
}

void test_delete_missing_url() {
    BrowserHistory b("a");
    b.visit("b");
    b.delete_url("b");
    check(!b.delete_url("nope.com"), "delete missing: returns false");
    check(b.restore_count() == 1, "delete missing: stack unchanged");
    check(urls(b) == "a", "delete missing: history unchanged");

    bool thrown = false;
    try {
        b.delete_url("");
    } catch (const invalid_argument&) {
        thrown = true;
    }
    check(thrown, "delete empty url: throws invalid_argument");
    check(b.restore_count() == 1, "delete empty url: stack unchanged");
}

void test_delete_duplicate_url() {
    BrowserHistory b("a");
    b.visit("b");
    b.visit("a"); // same url twice
    b.delete_url("a");
    check(urls(b) == "b a" && cur(b) == "a", "duplicate: only the first (oldest) one is deleted");
}

// ---------- restore ----------

void test_restore_lifo_positions() {
    BrowserHistory b("a", 10);
    b.visit("b");
    b.visit("c");
    b.visit("d");
    b.visit("e");
    b.delete_url("b");
    b.delete_url("d");
    b.delete_url("a");
    check(urls(b) == "c e", "restore lifo: after 3 deletes");

    check(b.restore_url() && urls(b) == "a c e", "restore lifo: a (last deleted) comes back first, at the front");
    check(b.restore_url() && urls(b) == "a c d e", "restore lifo: d goes back after c");
    check(b.restore_url() && urls(b) == "a b c d e", "restore lifo: b goes back after a");
    check(links_ok(b) && b.restore_count() == 0, "restore lifo: links ok, stack empty");
    check(cur(b) == "e", "restore lifo: cursor did not move");
}

void test_restore_chain() {
    // c is deleted while b is its prev, then b is deleted too.
    // after b comes back, c must find b again (that's why the id is kept)
    BrowserHistory b("a", 10);
    b.visit("b");
    b.visit("c");
    b.visit("d");
    b.delete_url("c");
    b.delete_url("b");
    check(urls(b) == "a d", "restore chain: after deletes");
    b.restore_url();
    b.restore_url();
    check(urls(b) == "a b c d", "restore chain: both back in the original order");
    check(links_ok(b), "restore chain: links ok");
}

void test_restore_empty_stack() {
    BrowserHistory b("a");
    b.visit("b");
    check(!b.restore_url(), "restore empty: returns false");
    check(urls(b) == "a b" && cur(b) == "b", "restore empty: nothing changes");
}

void test_restore_keeps_data() {
    BrowserHistory b("a");
    b.visit("b");
    Node* page = b._find_node("b");
    string title = page->title;
    time_t ts = page->timestamp;
    b.delete_url("b");
    b.restore_url();
    page = b._find_node("b");
    check(page != nullptr && page->title == title && page->timestamp == ts, "restore: title and timestamp kept");
}

void test_restore_predecessor_gone() {
    BrowserHistory b("a", 10);
    b.visit("b");
    b.visit("c");
    b.delete_url("c"); // prev was b
    b.back(1);         // on a
    b.visit("x");      // cuts b away
    check(urls(b) == "a x", "pred gone: b was removed by the visit");
    check(b.restore_url() && urls(b) == "a x c", "pred gone: c is put at the end");
    check(cur(b) == "x" && links_ok(b), "pred gone: cursor stays, links ok");
}

void test_restore_over_capacity() {
    BrowserHistory b("a", 3);
    b.visit("b");
    b.visit("c");
    b.delete_url("b"); // prev a
    b.visit("d");
    b.visit("e");      // a c d e -> a is pruned -> c d e
    check(urls(b) == "c d e", "restore full: a was pruned");
    check(b.restore_url(), "restore full: returns true");
    // prev a is gone so b goes to the end -> c d e b -> 4 pages, c is pruned
    check(urls(b) == "d e b", "restore full: oldest page pruned after restore");
    check(cur(b) == "e" && links_ok(b), "restore full: cursor ok, links ok");
}

void test_restore_prunes_current_head() {
    BrowserHistory b("a", 3);
    b.visit("b");
    b.visit("c");
    b.delete_url("b"); // a c
    b.visit("d");      // a c d
    b.back(2);         // on a, the oldest page
    b.restore_url();   // a b c d -> a is pruned, but we were on it
    check(urls(b) == "b c d", "restore prune current: a pruned");
    check(cur(b) == "b", "restore prune current: cursor moves to the new first page");
    check(links_ok(b), "restore prune current: links ok");
}

void test_delete_everything_then_restore() {
    BrowserHistory b("a");
    b.visit("b");
    b.visit("c");
    b.delete_url("a");
    b.delete_url("b");
    b.delete_url("c");
    check(urls(b) == "" && b.current_page() == nullptr, "delete all one by one: empty");
    check(b.restore_url() && urls(b) == "c" && cur(b) == "c", "delete all: first restore becomes current");
    b.restore_url();
    b.restore_url();
    check(urls(b) == "a b c" && cur(b) == "c" && links_ok(b), "delete all: everything back in order");
}

void test_delete_history_clears_stack() {
    BrowserHistory b("a");
    b.visit("b");
    b.delete_url("b");
    b.delete_history();
    check(b.restore_count() == 0, "delete_history: restore stack cleared");
    check(!b.restore_url() && b.current_page() == nullptr, "delete_history: nothing to restore");
}

void test_restore_into_full_history_at_front() {
    // a page that was first goes back to the front, but if history is full
    // it is the oldest page so it gets pruned again right away
    BrowserHistory b("a", 2);
    b.visit("b");
    b.delete_url("a");
    b.visit("c"); // b c (full)
    check(b.restore_url(), "restore front when full: returns true");
    check(urls(b) == "b c" && cur(b) == "c" && links_ok(b), "restore front when full: oldest (a) pruned again");
    check(b.restore_count() == 0, "restore front when full: record used");
}

int main() {
    test_export_format();
    test_export_empty_history();
    test_export_bad_path();
    test_round_trip();
    test_import_missing_file();
    test_import_empty_file();
    test_import_corrupted_lines();
    test_import_cursor_fallbacks();
    test_import_windows_line_endings();
    test_import_over_capacity();
    test_import_replaces_and_clears_stack();
    test_delete_spots();
    test_delete_current_page();
    test_delete_only_page();
    test_delete_missing_url();
    test_delete_duplicate_url();
    test_restore_lifo_positions();
    test_restore_chain();
    test_restore_empty_stack();
    test_restore_keeps_data();
    test_restore_predecessor_gone();
    test_restore_over_capacity();
    test_restore_prunes_current_head();
    test_delete_everything_then_restore();
    test_delete_history_clears_stack();
    test_restore_into_full_history_at_front();
    if (fails) cout << fails << " check(s) failed\n";
    else cout << "all feature tests passed\n";
    return fails != 0;
}
