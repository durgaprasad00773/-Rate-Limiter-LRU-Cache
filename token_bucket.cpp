#include <bits/stdc++.h>
using namespace std;
class token_bucket {
    int capacity;
    int tokens;
    mutex mtx;
public:
    token_bucket(int capacity) {
        this->capacity = capacity;
        this->tokens = capacity;
    }

    void consume() {
        lock_guard<mutex> lock(mtx);
        if (tokens > 0) {
            tokens--;
        }
    }

    void refill() {
        lock_guard<mutex> lock(mtx);
        tokens = capacity;
    }
};