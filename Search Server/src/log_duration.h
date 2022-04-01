#pragma once

#include <chrono>
#include <iostream>

#define STRING_LINKER_INTERNAL(x, y) x ## y
#define STRING_LINKER(x, y) STRING_LINKER_INTERNAL(x, y)
#define UNIQUE_VAR_NAME STRING_LINKER(unique_name, __LINE__)
#define OPERATION_DURATION(measured_operation, stream) LogDuration UNIQUE_VAR_NAME(measured_operation, stream)

class LogDuration {
public:
    using Clock = std::chrono::steady_clock;

    LogDuration(const std::string_view &id, std::ostream &stream = std::cerr) : id_(id), outstream_type_(stream) {
    }

    ~LogDuration() {
        using namespace std::chrono;
        using namespace std::literals;

        const auto end_time = Clock::now();
        const auto duration = end_time - start_time_;
        outstream_type_ << id_ << ": "s << duration_cast<milliseconds>(duration).count() << " ms"s << std::endl;
    }

private:
    const std::string_view id_;
    std::ostream &outstream_type_;
    const Clock::time_point start_time_ = Clock::now();
};
