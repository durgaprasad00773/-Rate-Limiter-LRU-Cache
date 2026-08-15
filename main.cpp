#include <bits/stdc++.h>
#include <thread>
#include "lru.hpp"
#include "token_bucket.hpp"

using namespace std;

// Simple usage demo: a few worker threads sharing one cache and one
// rate limiter, showing the system behaving correctly under light
// concurrent load. For real performance numbers, see benchmark_cache.cpp
// and benchmark_ratelimiter.cpp instead.
void worker(int id, LRUCache &cache, TokenBucket &rl) {
    for (int i = 0; i < 20; i++) {
        if (rl.allow()) {
            cache.put(i, id);
            int val = cache.get(i);
            cout << "Thread " << id << " allowed (cached value=" << val << ")\n";
        } else {
            cout << "Thread " << id << " blocked\n";
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

int main() {
    LRUCache cache(5);
    TokenBucket rl(5, 2); // capacity=5, refills 2 tokens/sec

    vector<thread> threads;
    for (int i = 0; i < 3; i++)
        threads.emplace_back(worker, i, ref(cache), ref(rl));

    for (auto &t : threads)
        t.join();

    cout << "System working!\n";
    return 0;
}