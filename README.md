# High Performance Rate Limiter & Concurrent LRU Cache

A C++ systems project exploring how high-traffic backends stay stable under
load: throttling excessive requests without dropping legitimate ones, and
caching hot data with O(1) access even when dozens of threads hit it at
once.

## What's in here

| File | Purpose |
|---|---|
| `main.cpp` | Simple usage demo — a few threads sharing one cache and one rate limiter, showing the system behaving correctly. |
| `lru.hpp` | Thread-safe O(1) LRU cache — hash map + doubly linked list, mutex-protected. |
| `token_bucket.hpp` | Thread-safe token bucket rate limiter with continuous, real-time-based refill. |
| `benchmark_cache.cpp` | Stress-tests the LRU cache with 50 concurrent threads, measures throughput and p50/p90/p99 latency. |
| `benchmark_ratelimiter.cpp` | Stress-tests the rate limiter with 50 concurrent threads, measures decision throughput, latency, and accuracy against theoretical token-bucket limits. |

## How it works

**LRU Cache** — a hash map gives O(1) key lookup, and a doubly linked list
tracks recency order so the least-recently-used entry can be evicted in
O(1) when the cache is full. A single mutex guards both structures for
thread-safe concurrent access.

**Token Bucket Rate Limiter** — each request consumes one token if
available; tokens refill continuously based on elapsed wall-clock time
rather than a separate timer thread, so refill rate stays accurate
regardless of request timing.

## Benchmark results (50 concurrent threads)

Measured with `-O2` optimization, 50 threads, on a typical development
machine:

**LRU Cache**
- Throughput: **3.7M ops/sec**
- Latency p50: <1 µs
- Latency p90: 0.5 µs
- Latency p99: 436 µs (reflects mutex contention under heavy concurrent load)

**Token Bucket Rate Limiter**
- Decision throughput: **3.02M decisions/sec**
- Latency p50: <1 µs
- Latency p99: 136 µs
- Allowed-request count landed within **0.2%** of the theoretical token-bucket maximum, confirming refill accuracy under load

Run the benchmarks yourself — see below.

## Build & run

Requires a C++17 compiler with pthread support (Linux/macOS g++, or on
Windows: WSL, MSYS2, or [w64devkit](https://github.com/skeeto/w64devkit)).

```bash
# Demo
g++ -O2 -std=c++17 -pthread main.cpp -o main_demo
./main_demo

# LRU cache benchmark
g++ -O2 -std=c++17 -pthread benchmark_cache.cpp -o benchmark_cache
./benchmark_cache

# Rate limiter benchmark
g++ -O2 -std=c++17 -pthread benchmark_ratelimiter.cpp -o benchmark_ratelimiter
./benchmark_ratelimiter
```

To observe lock contention directly, change `NUM_THREADS` at the top of
`benchmark_cache.cpp` (e.g. 1 vs 50) and compare throughput between runs.

## Tech

C++17 · STL (`unordered_map`, `list`, `mutex`, `thread`, `chrono`) · Linux