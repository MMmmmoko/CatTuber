#ifndef _ObjectPool_h
#define _ObjectPool_h

#include<iostream>
#include<vector>
//对象池
//主要为了在紧凑内存中使用大量Button和Axis对象
//无线程安全 比如如果管理Button 就主要在输入相关的那个线程使用，不要在其他地方使用
template<typename T>
class ObjectPool
{
public:
    struct Handle {
        uint32_t index= 0xFFFFFFFF;
        uint32_t generation;
        ObjectPool* _pPool;

        explicit operator bool() const noexcept { return index!= 0xFFFFFFFF; }
        T* operator->()
        {
            return _pPool->get(*this);
        }
    };
    Handle create(T value) {
        if (!free_.empty()) {
            uint32_t i = free_.back(); free_.pop_back();//如果有空置的位置，使用一个空置位置（free数组的back）。
            storage_[i] = std::move(value);
            ++generations_[i];
            return { i, generations_[i] };
        }
        //没有空位时，直接pushback
        storage_.push_back(std::move(value));
        generations_.push_back(0);
        return { uint32_t(storage_.size() - 1), 0,this };
    }
    //不可长期保存
    T* get(const Handle& h) {
        if (h.index >= storage_.size()) return nullptr;
        if (generations_[h.index] != h.generation) return nullptr;
        return &storage_[h.index];
    }
    void destroy(const Handle& h) {
        if (h.index >= storage_.size()) return;
        if (generations_[h.index] != h.generation) return;
        ++generations_[h.index]; // 提升代号，旧句柄失效
        free_.push_back(h.index);
    }


    void reserve(size_t n) {
        storage_.reserve(n);
        generations_.reserve(n);
        // free_ 不一定要 reserve，除非你预计频繁 destroy
    }


    //遍历迭代器
    struct iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        ObjectPool* pool = nullptr;
        size_t idx = 0;

        iterator(ObjectPool* p, size_t i) : pool(p), idx(i) {
            skip_invalid();
        }

        reference operator*() { return pool->storage_[idx]; }
        pointer operator->() { return &pool->storage_[idx]; }

        iterator& operator++() {
            ++idx;
            skip_invalid();
            return *this;
        }
        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        bool operator==(const iterator& other) const { return idx == other.idx && pool == other.pool; }
        bool operator!=(const iterator& other) const { return !(*this == other); }

    private:
        void skip_invalid() {
            while (idx < pool->storage_.size()) {
                // 如果在 free_ 中，或者 generation 不匹配，则跳过
                if (pool->generations_[idx] != uint32_t(-1)) break;
                ++idx;
            }
        }
    };
    // 遍历接口
    iterator begin() { return iterator(this, 0); }
    iterator end() { return iterator(this, storage_.size()); }
private:
    std::vector<T> storage_;//存储对象
    std::vector<uint32_t> generations_;//存储代数
    std::vector<uint32_t> free_;//存储空闲位置索引
};



#endif