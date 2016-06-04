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

    template <typename T>
    class FixedArray;

    template <typename T>
    class FixedArrayIterator {
    public:
        FixedArrayIterator(const FixedArray<T> *array, size_t pos) : pos_(pos), array_(array) {
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
        size_t              pos_;
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
                memcpy(array_, arr.array_, sizeof(T) * size_);
            }
        }

        FixedArray(FixedArray &&arr) : array_(nullptr), size_(0) {
            array_ = arr.array_;
            size_ = arr.size_;
            
            arr.array_ = nullptr;
            arr.size_ = 0;
        }

        FixedArray(const T *array, size_t size) : array_(nullptr), size_(0) {
            if (array && size > 0) {
                array_ = new T[size];
                size_ = size;
                memcpy(array_, array, sizeof(T) * size);
            }
        }

        FixedArray(T *array, size_t size) : array_(nullptr), size_(0) {
            if (array && size > 0) {
                array_ = new T[size];
                size_ = size;
                memcpy(array_, array, sizeof(T) * size);
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
        
        size_t size() const {
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
                    memcpy(array_, op.array_, sizeof(T) * size_);
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
        
        T& operator[](size_t index) {
            return array_[index];
        }
        
        const T& operator[](size_t index) const {
            return array_[index];
        }
        
        T *addressOf(size_t index) const {
            return &array_[index];
        }
        
        FixedArrayIterator<T> begin () const {
            return FixedArrayIterator<T>(this, 0);
        }
        
        FixedArrayIterator<T> end () const {
            return FixedArrayIterator<T>(this, size_);
        }
        
    protected:
        T       *array_;
        size_t  size_;
    };
}

#endif /* Array_h */
