//
//  FixedArray.h
//  BenGameEngine
//
//  Created by Benjamin Lee on 6/2/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef FixedArray_h
#define FixedArray_h

#include <stdio.h>
#include <cassert>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace BGE {
    int32_t const NullPtrIndex = -1;
    
    template <typename T>
    class FixedArray;

    template <typename T>
    class FixedArrayIterator {
    public:
        FixedArrayIterator(const FixedArray<T> *array, int32_t pos) : pos_(pos), array_(array) {
        }

        bool operator !=(const FixedArrayIterator<T>& other) const {
            return pos_ != other.pos_;
        }
        
        T& operator* () const {
            return (T&) (*array_)[pos_];
        }
        
        const FixedArrayIterator& operator++() {
            pos_++;
            
            return *this;
        }
        
    private:
        int32_t              pos_;
        const FixedArray<T> *array_;
    
    };
    
    template <typename T>
    class FixedArray {
    public:
        FixedArray() : array_(nullptr), size_(0) {}
        
        FixedArray(const FixedArray &arr) : array_(nullptr), size_(0) {
            if (arr.array_ && arr.size_) {
                array_ = new T[arr.size_];
                size_ = arr.size_;
                
                if (std::is_pod<T>::value) {
                    memcpy(array_, arr.array_, sizeof(T) * size_);
                } else {
                    std::copy(&arr.array_[0], &arr.array_[size_], array_);
                }
            }
        }

        FixedArray(FixedArray &&arr) : array_(nullptr), size_(0) {
            array_ = arr.array_;
            size_ = arr.size_;
            
            arr.array_ = nullptr;
            arr.size_ = 0;
        }

        FixedArray(const std::vector<T>& vec) : array_(nullptr), size_(0) {
            if (vec.size()) {
                array_ = new T[vec.size];
                size_ = vec.size;
                
                if (std::is_pod<T>::value) {
                    memcpy(array_, vec.begin(), sizeof(T) * size_);
                } else {
                    std::copy(vec.begin(), vec.end(), array_);
                }
            }
        }
        
        FixedArray(const std::vector<const std::vector<T> *>& vec) : array_(nullptr), size_(0) {
            int32_t total = 0;
            
            for (auto v : vec) {
                total += v.size();
            }
            
            if (total) {
                array_ = new T[total];
                size_ = total;
                
                if (std::is_pod<T>::value) {
                    auto span = 0;
                    
                    for (auto v : vec) {
                        memcpy(&array_[span], v.begin(), sizeof(T) * v.size());
                        span += v.size();
                    }
                } else {
                    auto span = 0;
                    
                    for (auto v : vec) {
                        std::copy(v.begin(), v.end(), &array_[span]);
                        span += v.size();
                    }
                }
            }
        }
        
        FixedArray(const T *array, int32_t size) : array_(nullptr), size_(0) {
            if (array && size > 0) {
                array_ = new T[size];
                size_ = size;
                if (std::is_pod<T>::value) {
                    memcpy(array_, array, sizeof(T) * size);
                } else {
                    std::copy(&array[0], &array[size], array_);
                }
            }
        }

        FixedArray(T *array, int32_t size) : array_(nullptr), size_(0) {
            if (array && size > 0) {
                array_ = new T[size];
                size_ = size;
                if (std::is_pod<T>::value) {
                    memcpy(array_, array, sizeof(T) * size);
                } else {
                    std::copy(&array[0], &array[size], array_);
                }
            }
        }
        
        virtual ~FixedArray() {
            if (array_) {
                delete [] array_;
            }
        }
        
        void clear() {
            if (array_) {
                delete [] array_;
            }
            
            array_ = nullptr;
            size_ = 0;
        }
        
        int32_t size() const {
            return size_;
        }
        
        FixedArray& operator=(const FixedArray& op) {
            if (this != &op) {
                if (array_) {
                    delete [] array_;
                }
                
                if (op.array_ && op.size_) {
                    array_ = new T[op.size_];
                    size_ = op.size_;
                    if (std::is_pod<T>::value) {
                        memcpy(array_, op.array_, sizeof(T) * size_);
                    } else {
                        std::copy(&op.array_[0], &op.array_[size_], array_);
                    }
                } else {
                    array_ = nullptr;
                    size_ = 0;
                }
            }
            
            return *this;
        }
        
        FixedArray& operator=(FixedArray&& op) {
            if (this != &op) {
                if (array_) {
                    delete [] array_;
                }
                
                array_ = op.array_;
                size_ = op.size_;
                
                op.array_ = nullptr;
                op.size_ = 0;
            }
            
            return *this;
        }
        
        T& operator[](int32_t index) {
            return array_[index];
        }
        
        const T& operator[](int32_t index) const {
            return array_[index];
        }
        
        T *addressOf(int32_t index) const {
            return &array_[index];
        }
        
        T *safeAddressOf(int32_t index) const {
            if (index == NullPtrIndex || index >= size_) {
                return nullptr;
            } else {
                return addressOf(index);
            }
        }
        
        FixedArrayIterator<T> begin () const {
            return FixedArrayIterator<T>(this, 0);
        }
        
        FixedArrayIterator<T> end () const {
            return FixedArrayIterator<T>(this, size_);
        }
        
    protected:
        T       *array_;
        int32_t  size_;
    };
}

#endif /* Array_h */
