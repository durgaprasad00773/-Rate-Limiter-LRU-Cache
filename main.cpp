#include<bits/stdc++.h>
#include "lru.cpp"
#include "token_bucket.cpp"

using namespace std;

void worker(int id, LRUCache &cache, TokenBucket &rl) {
    for(int i = 0; i < 20; i++) {

        if(rl.allow()) {
            cache.put(i, id);
            int val = cache.get(i);

            cout << "Thread " << id << " allowed\n";
        } else {
            cout << "Thread " << id << " blocked\n";
        }

        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

int main() {
    LRUCache cache(5);
    TokenBucket rl(5, 2); // capacity=5, refill 2/sec

    vector<thread> threads;

    for(int i = 0; i < 3; i++) {
        threads.emplace_back(worker, i, ref(cache), ref(rl));
    }

    for(auto &t : threads)
        t.join();

    cout << "System working!\n";
}