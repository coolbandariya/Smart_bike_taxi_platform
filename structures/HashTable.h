#ifndef SMART_BIKE_TAXI_HASH_TABLE_H
#define SMART_BIKE_TAXI_HASH_TABLE_H

#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <vector>

// Educational open-addressing hash table with linear probing.
// This implementation supports insert/update and lookup (no deletion).
template <typename Value>
class HashTable {
    struct Entry {
        std::string key;
        Value value;
        bool occupied = false;
    };

public:
    explicit HashTable(std::size_t capacity = 101)
        : entries_(capacity == 0 ? 1 : capacity) {}

    bool insert(const std::string& key, const Value& value) {
        if (key.empty()) return false;
        const std::size_t start = std::hash<std::string>{}(key) % entries_.size();
        for (std::size_t step = 0; step < entries_.size(); ++step) {
            Entry& entry = entries_[(start + step) % entries_.size()];
            if (!entry.occupied) {
                entry.key = key;
                entry.value = value;
                entry.occupied = true;
                ++size_;
                return true;
            }
            if (entry.key == key) {
                entry.value = value;
                return true;
            }
        }
        return false; // Table is full.
    }

    Value* find(const std::string& key) {
        if (key.empty()) return nullptr;
        const std::size_t start = std::hash<std::string>{}(key) % entries_.size();
        for (std::size_t step = 0; step < entries_.size(); ++step) {
            Entry& entry = entries_[(start + step) % entries_.size()];
            if (!entry.occupied) return nullptr;
            if (entry.key == key) return &entry.value;
        }
        return nullptr;
    }

    const Value* find(const std::string& key) const {
        if (key.empty()) return nullptr;
        const std::size_t start = std::hash<std::string>{}(key) % entries_.size();
        for (std::size_t step = 0; step < entries_.size(); ++step) {
            const Entry& entry = entries_[(start + step) % entries_.size()];
            if (!entry.occupied) return nullptr;
            if (entry.key == key) return &entry.value;
        }
        return nullptr;
    }

    std::size_t size() const { return size_; }

private:
    std::vector<Entry> entries_;
    std::size_t size_ = 0;
};

#endif
