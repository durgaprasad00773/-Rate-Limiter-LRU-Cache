#pragma once
#include <mutex>
#include <chrono>

// Thread-safe token bucket rate limiter.
// Tokens refill continuously based on elapsed wall-clock time,
// rather than needing an external refill() call on a timer.
class TokenBucket {
    int capacity;
    double tokens;
    double refillRatePerSec;
    std::chrono::steady_clock::time_point lastRefill;
    std::mutex mtx;

public:
    TokenBucket(int capacity, double refillRatePerSec)
        : capacity(capacity), tokens(capacity), refillRatePerSec(refillRatePerSec) {
        lastRefill = std::chrono::steady_clock::now();
    }

    bool allow() {
        std::lock_guard<std::mutex> lock(mtx);
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(now - lastRefill).count();
        tokens = std::min((double)capacity, tokens + elapsed * refillRatePerSec);
        lastRefill = now;

        if (tokens >= 1.0) {
            tokens -= 1.0;
            return true;
        }
        return false;
    }
};