CXX = g++
CXXFLAGS = -std=c++17 -Wall

HEADERS = Node.h DoublyLinkedList.h Stack.h BrowserHistory.h
BROWSER_SRC = BrowserHistory.cpp DoublyLinkedList.cpp
TESTS = test_linked_list test_browser test_features test_integration

# "make" builds the menu program and the demo
all: browser demo

browser: main.cpp $(BROWSER_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(filter %.cpp,$^) -o $@

demo: demo.cpp $(BROWSER_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(filter %.cpp,$^) -o $@

test_linked_list: test_linked_list.cpp DoublyLinkedList.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(filter %.cpp,$^) -o $@

test_browser: test_browser.cpp $(BROWSER_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(filter %.cpp,$^) -o $@

test_features: test_features.cpp $(BROWSER_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(filter %.cpp,$^) -o $@

test_integration: test_integration.cpp $(BROWSER_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(filter %.cpp,$^) -o $@

# "make test" builds and runs every test, stops at the first one that fails
test: $(TESTS)
	./test_linked_list
	./test_browser
	./test_features
	./test_integration

# same tests with AddressSanitizer, it fails on memory leaks and bad pointers
# (works with g++/clang on Linux and macOS, not with MinGW on Windows)
SAN = -g -fsanitize=address,undefined
memcheck:
	$(CXX) $(CXXFLAGS) $(SAN) test_linked_list.cpp DoublyLinkedList.cpp -o test_linked_list_asan
	$(CXX) $(CXXFLAGS) $(SAN) test_browser.cpp $(BROWSER_SRC) -o test_browser_asan
	$(CXX) $(CXXFLAGS) $(SAN) test_features.cpp $(BROWSER_SRC) -o test_features_asan
	$(CXX) $(CXXFLAGS) $(SAN) test_integration.cpp $(BROWSER_SRC) -o test_integration_asan
	./test_linked_list_asan
	./test_browser_asan
	./test_features_asan
	./test_integration_asan

clean:
	rm -f browser demo $(TESTS) *_asan *.exe test_tmp_*.txt demo_history.txt

.PHONY: all test memcheck clean
