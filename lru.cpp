#include <bits/stdc++.h>
using namespace std;
class LRUCache {
    int capacity;
    list<int> order;
    mutex mtx;
    unordered_map<int, pair<int, list<int>::iterator>> cache;
public:
    LRUCache(int capacity) {
        this->capacity = capacity;
    }

    int get(int key) {
        lock_guard<mutex> lock(mtx);
        if (cache.find(key) == cache.end()) {
            return -1;
        }
        order.erase(cache[key].second);
        order.push_front(key);
        cache[key].second = order.begin();
        return cache[key].first;
    }
    void put(int key, int value) {
        lock_guard<mutex> lock(mtx);
        if (cache.find(key) != cache.end()) {
            order.erase(cache[key].second);
        } else if (cache.size() == capacity) {
            int lruKey = order.back();
            order.pop_back();
            cache.erase(lruKey);
        }
        order.push_front(key);
        cache[key] = {value, order.begin()};
    }
};

void worker(LRUCache &cache) {
    for(int i = 0; i < 1000; i++) {
        cache.put(i % 3, i);
        cache.get(i % 3);
    }
}

int main() {
    LRUCache cache(2);
    vector<thread> threads;

    for(int i = 0; i < 5; i++)
        threads.emplace_back(worker, ref(cache));

    for(auto &t : threads)
        t.join();

    cout << "Done\n";
}
