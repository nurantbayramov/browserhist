#pragma once
#include <optional>

using namespace std;

/*
 * LIFO on a plain array. T needs a default constructor.
 * If T is a pointer (e.g. Node*), clear() and the destructor do NOT delete what
 * it points to. Whoever stores the pointers has to delete them.
 */
template <typename T>
class Stack {
public:
    Stack() : data_(new T[4]), cap_(4), n_(0) {}
    ~Stack() { delete[] data_; }
    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;
    void push(const T& x) {
        if (n_ == cap_) grow();
        data_[n_++] = x;
    }
    /** Empty optional if the stack is empty. */
    optional<T> pop() {
        if (n_ == 0) return nullopt;
        return data_[--n_];
    }
    optional<T> peek() const {
        if (n_ == 0) return nullopt;
        return data_[n_ - 1];
    }
    void clear() { n_ = 0; }
    bool is_empty() const { return n_ == 0; }
    int size() const { return n_; }
private:
    T* data_;
    int cap_;
    int n_;
    void grow() {
        T* bigger = new T[cap_ * 2];
        for (int i = 0; i < n_; i++) bigger[i] = data_[i];
        delete[] data_;
        data_ = bigger;
        cap_ *= 2;
    }
};