#ifndef TESTS_H
#define TESTS_H

#include <chrono>
#include <set>
#include <string>
#include <iostream>

#define STRING_LINKER_INTERNAL(x, y) x##y
#define STRING_LINKER(x, y) STRING_LINKER_INTERNAL(x, y)
#define UNIQUE_VAR_NAME STRING_LINKER(unique_name, __LINE__)
#define OPERATION_DURATION(measured_operation, string_ptr) LogDuration UNIQUE_VAR_NAME(measured_operation, string_ptr)

class LogDuration {
public:
    using Clock = std::chrono::steady_clock;

    LogDuration(const std::string& id, std::string& line)
        : id_(id)
        , output_(&line)
    {
    }

    ~LogDuration()
    {
        using namespace std::chrono;
        using namespace std::literals;

        const auto end_time = Clock::now();
        const auto duration = end_time - start_time_;

        *output_ = id_;
        *output_ += ": "s;
        *output_ += std::to_string(duration_cast<milliseconds>(duration).count());
        *output_+= " ms\n"s;
    }

private:
    const std::string id_;
    std::string* output_ = nullptr;
    const Clock::time_point start_time_ = Clock::now();
};


class Tests
{
public:
    Tests() = default;

    std::string GenerateBooks(size_t amount);

    std::string AddToLib();

private:
    std::set<std::string> pseudo_string_ = {};
};

#endif // TESTS_H
