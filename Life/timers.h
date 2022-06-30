#pragma once

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#define PROFILE_CONCAT_INTERNAL(X, Y) X##Y
#define PROFILE_CONCAT(X, Y) PROFILE_CONCAT_INTERNAL(X, Y)
#define UNIQUE_VAR_NAME_PROFILE PROFILE_CONCAT(profileGuard, __LINE__)
#define LOG_DURATION(x) LogDuration UNIQUE_VAR_NAME_PROFILE(x)
#define LOG_DURATION_STREAM(x, y) LogDuration UNIQUE_VAR_NAME_PROFILE(x, y)

static const std::chrono::duration<double, std::milli> kPeriod { 10'000 };

inline void Wait() {
    std::this_thread::sleep_for(kPeriod);
}

class LogDuration {
public:
    using Clock = std::chrono::steady_clock;

    LogDuration(const std::string id, std::ostream &dst_stream = std::cerr)
        : id_(id)
        , dst_stream_(dst_stream) {
    }

    ~LogDuration() {
        using namespace std::chrono;
        using namespace std::literals;

        const auto end_time = Clock::now();
        const auto dur = end_time - start_time_;
        dst_stream_ << id_ << ": "s << duration_cast<microseconds>(dur).count() << " us    "s << std::endl;
    }

private:
    const std::string id_;
    const Clock::time_point start_time_ = Clock::now();
    std::ostream &dst_stream_;
};
