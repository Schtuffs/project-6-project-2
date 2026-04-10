#pragma once

#include <cstring>
#include <format>
#include <memory>
#include <print>
#include <type_traits>

template <typename T>
class Array {
private:
    uint64_t mSize;
    std::shared_ptr<T[]> mData;

public:
    // ----- Creation ----- Destruction -----

    // Always creates at least one space
    Array(uint64_t n = 0) : mSize(n), mData(new T[mSize]) {
        // Nothing todo
    }
    
    Array(const Array<T>& other) : mSize(other.mSize), mData(other.mData) {
        // Nothing todo
    }
    
    ~Array() {
        // Nothing todo
    }
    
    // ----- Read -----
    
    // Bounds checked indexing
    // Throws string on out of bounds
    T& at(uint64_t index) const {
        if (index >= mSize) {
            throw std::string("ERROR: Array out of bounds"); 
        }
        return mData.get()[index];
    }
    
    // Returns the data pointer
    T* data() const {
        return mData.get();
    }
    
    // Returns the size of the array
    uint64_t size() const {
        return mSize;
    }
    
    // Not bounds checking array indexing
    T& operator[](uint64_t index) const {
        return mData.get()[index];
    }
    
    // ----- Update -----
    
    Array<T>& operator=(const Array<T>& other) {
        // Self assignment
        if (this == &other) {
            return *this;
        }
        
        // Create pointers
        mSize = other.mSize;
        mData = other.mData;

        return *this;
    }
};

template<typename T>
struct std::formatter<Array<T>> : std::formatter<std::string> {
    auto format(const Array<T>& arr, format_context& ctx) const {
        std::string str = "( ";
        for (uint64_t i = 0; i < arr.size(); i++) {
            str += std::to_string(arr[i]);
            if (i != arr.size() - 1) {
                str += ", ";
            }
        }
        str += " )";
        return std::formatter<std::string>::format(std::format("{}", str), ctx);
    }
};


