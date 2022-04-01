#pragma once

#include <map>
#include <mutex>
#include <vector>

using namespace std::string_literals;

template<typename Key, typename Value>
class ConcurrentMap {
    struct Bucket;

public:
    static_assert(std::is_integral_v<Key>, "ConcurrentMap supports only integer keys");

    struct Access {
        Access(const Key &key, Bucket &bucket) : guard(bucket.private_mutex), ref_to_value(bucket.private_map[key]) {
        }
        std::lock_guard<std::mutex> guard;
        Value &ref_to_value;
    };

    explicit ConcurrentMap(size_t bucket_count) : buckets_(bucket_count) {
    }

    Access operator[](const Key &key) {
        auto &bucket = buckets_[static_cast<uint64_t>(key) % buckets_.size()];
        return {key, bucket};
    }

    void Erase(const Key &key) {
        std::lock_guard wait(buckets_[static_cast<uint64_t>(key) % buckets_.size()].private_mutex);
        buckets_[static_cast<uint64_t>(key) % buckets_.size()].private_map.erase(key);
    }

    std::map<Key, Value> BuildOrdinaryMap() {
        std::map<Key, Value> result;
        for (auto &part : buckets_) {
            std::lock_guard wait(part.private_mutex);
            result.merge(part.private_map);
        }
        return result;
    }

private:
    struct Bucket {
        std::mutex private_mutex;
        std::map<Key, Value> private_map;
    };

private:
    std::vector<Bucket> buckets_;
};
