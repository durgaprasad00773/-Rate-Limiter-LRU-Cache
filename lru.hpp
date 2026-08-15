#pragma once
#include <mutex>
#include <list>
#include <unordered_map>

class LRUCache {
    int capacity;
    std::list<int> order;
    std::mutex mtx;
    std::unordered_map<int, std::pair<int, std::list<int>::iterator>> cache;

public:
    LRUCache(int capacity) : capacity(capacity) {}

    int get(int key) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = cache.find(key);
        if (it == cache.end()) return -1;
        order.erase(it->second.second);
        order.push_front(key);
        it->second.second = order.begin();
        return it->second.first;
    }

    void put(int key, int value) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = cache.find(key);
        if (it != cache.end()) {
            order.erase(it->second.second);
        } else if ((int)cache.size() == capacity) {
            int lruKey = order.back();
            order.pop_back();
            cache.erase(lruKey);
        }
        order.push_front(key);
        cache[key] = {value, order.begin()};
    }
};