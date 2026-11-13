# browserhist

Browser History Management System for CSCI 2304 (Data Structures & Algorithms).
History is kept in a doubly linked list written from scratch (no `std::list`,
`std::deque` or `std::stack`). Written in C++17.

## Files

| File | What is inside |
|------|----------------|
| `Node.h` | One page: url, title, timestamp, id, and the `prev`/`next` links |
| `DoublyLinkedList.h/.cpp` | The doubly linked list (append, insert_after, remove_node, truncate_after, ...) |
| `Stack.h` | Array based stack template, used for delete/restore |
| `BrowserHistory.h/.cpp` | The browser: visit, back, forward, capacity, display, delete/restore, export/import |
| `main.cpp` | Menu program |
| `demo.cpp` | Prepared demo that shows every feature step by step (for the video) |
| `test_linked_list.cpp` | Tests for the list and the stack |
| `test_browser.cpp` | Tests for visit/back/forward/capacity/display/delete_history |
| `test_features.cpp` | Tests for export/import and delete/restore |
| `test_integration.cpp` | Tests that use everything together (+ 3000 random operations) |

## Build and run

You need g++ (or clang++) and make.

```
make            # builds ./browser and ./demo
./browser       # menu program, capacity 50
./browser 5     # menu program with capacity 5 (easier to see pruning)
./demo          # demo, press Enter after each step
make test       # builds and runs all 4 test programs
make memcheck   # runs the tests with AddressSanitizer (Linux/macOS)
make clean
```

Without make:

```
g++ -std=c++17 -Wall main.cpp BrowserHistory.cpp DoublyLinkedList.cpp -o browser
```

## Menu

```
 1. Visit a page
 2. Go back
 3. Go forward
 4. Show current page
 5. Show history
 6. Delete a page from history
 7. Restore last deleted page
 8. Export history to a file
 9. Import history from a file
10. Delete all history
 0. Exit
```

The current page is marked with `[<]` when the history is shown.

## History file format

First line is the cursor (index of the current page, starting from 0, or -1 if
history is empty). Then one page per line, the 3 parts are separated by a TAB:

```
#CURSOR 1
google.com	google.com	1794591961
github.com	github.com	1794591975
youtube.com	youtube.com	1794591990
```

When importing:
- import **replaces** the current history (the menu asks before doing it)
- broken lines are skipped and a warning with the line number is printed
- missing or wrong cursor -> the cursor goes to the last page
- more pages than the capacity -> only the newest pages are loaded
- missing file or no valid pages -> import returns false and the old history stays

## Decisions we made

- **Deleting the current page:** cursor moves to the previous page, if there is none to the next page, if there is none the history is empty.
- **Where a restored page goes:** right after the page that was before it. If that page is gone, at the end. A page that was the first page goes back to the front.
- **Restore stack:** last deleted page is restored first (LIFO). The stack keeps a copy of the data (url, title, timestamp, id, id of the previous page), not a pointer, because the deleted node is freed.
- **Capacity and restore:** if restoring makes the history bigger than the capacity, the oldest page is removed (same as visit).
- **Delete all history** and **import** also clear the restore stack.
- `back()`/`forward()` on an empty history return an empty string. Negative steps and empty URLs throw `invalid_argument`.

## Team

- Nuran: Node, DoublyLinkedList, Stack
- Ayan: BrowserHistory core (visit, back, forward, capacity, display, delete_history)
- Huseyn: export/import, delete/restore, menu, demo, feature and integration tests
