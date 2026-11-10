CXX = g++
CXXFLAGS = -std=c++17 -Wall

test_linked_list: test_linked_list.cpp DoublyLinkedList.cpp
	$(CXX) $(FLAGS) $^ -o $@

test: test_linked_list
	./test_linked_list

clean:
	rm -f test_linked_list