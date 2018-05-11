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
        FixedArray() : managed_(false), array_(nullptr), size_(0) {}
        
        FixedArray(int32_t size) : managed_(false), array_(nullptr), size_(size) {
            if (size > 0) {
                array_ = new T[size_];
            }
        }
        
        FixedArray(const FixedArray &arr) : managed_(false), array_(nullptr), size_(0) {
            if (arr.array_ && arr.size_) {
                managed_ = arr.managed_;
                size_ = arr.size_;
                if (managed_){
                    array_ = arr.array_;
                } else {
                    array_ = new T[arr.size_];
                    if (std::is_pod<T>::value) {
                        memcpy(array_, arr.array_, sizeof(T) * size_);
                    } else {
                        std::copy(&arr.array_[0], &arr.array_[size_], array_);
                    }
                }
            }
        }

        FixedArray(FixedArray &&arr) : managed_(false), array_(nullptr), size_(0) {
            managed_ = arr.managed_;
            array_ = arr.array_;
            size_ = arr.size_;

            arr.managed_ = false;
            arr.array_ = nullptr;
            arr.size_ = 0;
        }

        FixedArray(const std::vector<T>& vec) : managed_(false), array_(nullptr), size_(0) {
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
        
        FixedArray(const std::vector<const std::vector<T> *>& vec) : managed_(false), array_(nullptr), size_(0) {
            int32_t total = 0;
            
            for (auto const &v : vec) {
                total += v.size();
            }
            
            if (total) {
                array_ = new T[total];
                size_ = total;
                
                if (std::is_pod<T>::value) {
                    auto span = 0;
                    
                    for (auto const &v : vec) {
                        memcpy(&array_[span], v.begin(), sizeof(T) * v.size());
                        span += v.size();
                    }
                } else {
                    auto span = 0;
                    
                    for (auto const &v : vec) {
                        std::copy(v.begin(), v.end(), &array_[span]);
                        span += v.size();
                    }
                }
            }
        }
        
        FixedArray(const T *array, int32_t size, bool managed = false) : managed_(managed), array_(nullptr), size_(0) {
            if (array && size > 0) {
                size_ = size;
                if (managed_) {
                    array_ = array;
                } else {
                    array_ = new T[size];
                    if (std::is_pod<T>::value) {
                        memcpy(array_, array, sizeof(T) * size);
                    } else {
                        std::copy(&array[0], &array[size], array_);
                    }
                }
            } else {
                managed_ = false;
            }
        }

        FixedArray(T *array, int32_t size, bool managed = false) : managed_(managed), array_(nullptr), size_(0) {
            if (array && size > 0) {
                size_ = size;
                if (managed) {
                    array_ = array;
                } else {
                    array_ = new T[size];
                    if (std::is_pod<T>::value) {
                        memcpy(array_, array, sizeof(T) * size);
                    } else {
                        std::copy(&array[0], &array[size], array_);
                    }
                }
            } else {
                managed_ = false;
            }
        }
        
        virtual ~FixedArray() {
            if (!managed_ && array_) {
                delete [] array_;
            }
        }
        
        void clear() {
            if (!managed_ && array_) {
                delete [] array_;
            }

            managed_ = false;
            array_ = nullptr;
            size_ = 0;
        }
        
        int32_t size() const {
            return size_;
        }
        
        FixedArray& operator=(const FixedArray& op) {
            if (this != &op) {
                if (!managed_ && array_) {
                    delete [] array_;
                }

                if (op.array_ && op.size_) {
                    managed_ = op.managed_;
                    size_ = op.size_;
                    if (managed_) {
                        array_ = op.array_;
                    } else {
                        array_ = new T[op.size_];
                        if (std::is_pod<T>::value) {
                            memcpy(array_, op.array_, sizeof(T) * size_);
                        } else {
                            std::copy(&op.array_[0], &op.array_[size_], array_);
                        }
                    }
                } else {
                    managed_ = false;
                    array_ = nullptr;
                    size_ = 0;
                }
            }
            
            return *this;
        }
        
        FixedArray& operator=(FixedArray&& op) {
            if (this != &op) {
                if (!managed_ && array_) {
                    delete [] array_;
                }

                managed_ = op.managed_;
                array_ = op.array_;
                size_ = op.size_;

                op.managed_ = false;
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
        
        T *baseAddress() const {
            return &array_[0];
        }
        
        T *safeBaseAddress() const {
            if (size_ > 0) {
                return &array_[0];
            } else {
                return nullptr;
            }
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
        
        int32_t getSize() const { return size_; }
        int32_t getElementSize() const { return sizeof(T); }
        
        size_t getMemoryUsage() const { return size_ * sizeof(T); }
        
        FixedArrayIterator<T> begin () const {
            return FixedArrayIterator<T>(this, 0);
        }
        
        FixedArrayIterator<T> end () const {
            return FixedArrayIterator<T>(this, size_);
        }
        
    protected:
        bool    managed_;
        T       *array_;
        int32_t  size_;
    };
}

#endif /* Array_h */
