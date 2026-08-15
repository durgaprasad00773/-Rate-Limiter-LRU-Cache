#include <bits/stdc++.h>
#include <thread>
#include <atomic>
#include "token_bucket.hpp"

using namespace std;
using namespace std::chrono;

static const int NUM_THREADS    = 50;
static const int OPS_PER_THREAD = 20000;
static const int BUCKET_CAPACITY = 1000;
static const double REFILL_PER_SEC = 5000.0;

TokenBucket limiter(BUCKET_CAPACITY, REFILL_PER_SEC);
atomic<long long> totalAllowed{0};
atomic<long long> totalBlocked{0};
vector<vector<long long>> perThreadLatencies(NUM_THREADS);

void worker(int id) {
    perThreadLatencies[id].reserve(OPS_PER_THREAD);
    for (int i = 0; i < OPS_PER_THREAD; i++) {
        auto opStart = high_resolution_clock::now();
        bool ok = limiter.allow();
        auto opEnd = high_resolution_clock::now();

        if (ok) totalAllowed++; else totalBlocked++;
        perThreadLatencies[id].push_back(
            duration_cast<nanoseconds>(opEnd - opStart).count()
        );
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

    long long total = totalAllowed.load() + totalBlocked.load();
    double opsPerSec = total / wallSeconds;
    double expectedAllowed = BUCKET_CAPACITY + REFILL_PER_SEC * wallSeconds;

    cout << fixed << setprecision(2);
    cout << "\n===== TOKEN BUCKET RATE LIMITER BENCHMARK =====\n";
    cout << "Threads:            " << NUM_THREADS << "\n";
    cout << "Total requests:     " << total << "\n";
    cout << "Allowed:            " << totalAllowed.load()
         << "  (theoretical max ~" << (long long)expectedAllowed << ")\n";
    cout << "Blocked:            " << totalBlocked.load() << "\n";
    cout << "Wall time:          " << wallSeconds << " s\n";
    cout << "Decision throughput:" << opsPerSec << " decisions/sec\n";
    cout << "Latency p50:        " << percentile(0.50) << " us\n";
    cout << "Latency p99:        " << percentile(0.99) << " us\n";
    cout << "=================================================\n";
    return 0;
}