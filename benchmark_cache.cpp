#include <bits/stdc++.h>
#include <thread>
#include <atomic>
#include "lru.hpp"

using namespace std;
using namespace std::chrono;

static const int NUM_THREADS    = 50;
static const int OPS_PER_THREAD = 50000;   // put+get pairs per thread
static const int CACHE_CAPACITY = 1000;

LRUCache cache(CACHE_CAPACITY);
atomic<long long> totalOpsCompleted{0};
vector<vector<long long>> perThreadLatencies(NUM_THREADS);

void worker(int id) {
    perThreadLatencies[id].reserve(OPS_PER_THREAD);
    for (int i = 0; i < OPS_PER_THREAD; i++) {
        int key = (id * 37 + i) % (CACHE_CAPACITY * 2); // mix of hits/misses across threads

        auto opStart = high_resolution_clock::now();
        cache.put(key, id);
        cache.get(key);
        auto opEnd = high_resolution_clock::now();

        // each put+get pair counts as 2 ops
        perThreadLatencies[id].push_back(
            duration_cast<nanoseconds>(opEnd - opStart).count() / 2
        );
        totalOpsCompleted += 2;
    }
}

int main() {
    vector<thread> threads;
    threads.reserve(NUM_THREADS);

    auto wallStart = high_resolution_clock::now();
    for (int i = 0; i < NUM_THREADS; i++)
        threads.emplace_back(worker, i);
    for (auto &t : threads)
        t.join();
    auto wallEnd = high_resolution_clock::now();

    double wallSeconds = duration<double>(wallEnd - wallStart).count();

    vector<long long> allLatencies;
    allLatencies.reserve((size_t)NUM_THREADS * OPS_PER_THREAD);
    for (auto &v : perThreadLatencies)
        allLatencies.insert(allLatencies.end(), v.begin(), v.end());
    sort(allLatencies.begin(), allLatencies.end());

    auto percentile = [&](double p) -> double {
        size_t idx = (size_t)(p * (allLatencies.size() - 1));
        return allLatencies[idx] / 1000.0; // ns -> us
    };

    long long totalOps = totalOpsCompleted.load();
    double opsPerSec = totalOps / wallSeconds;

    cout << fixed << setprecision(2);
    cout << "\n===== LRU CACHE CONCURRENCY BENCHMARK =====\n";
    cout << "Threads:        " << NUM_THREADS << "\n";
    cout << "Total ops:      " << totalOps << " (put+get pairs)\n";
    cout << "Wall time:      " << wallSeconds << " s\n";
    cout << "Throughput:     " << opsPerSec << " ops/sec\n";
    cout << "Latency p50:    " << percentile(0.50) << " us\n";
    cout << "Latency p90:    " << percentile(0.90) << " us\n";
    cout << "Latency p99:    " << percentile(0.99) << " us\n";
    cout << "=============================================\n";
    return 0;
}