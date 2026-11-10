CXX = g++
FLAGS = -std=c++17 -Wall -Wextra -g -fsanitize=address,undefined

test_linked_list: test_linked_list.cpp DoublyLinkedList.cpp
	$(CXX) $(FLAGS) $^ -o $@

test: test_linked_list
	./test_linked_list

clean:
	rm -f test_linked_list