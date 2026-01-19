#pragma once

#include<hgl/type/DataType.h>
#include<hgl/type/ArrayRearrangeHelper.h>
#include<hgl/type/ArrayItemProcess.h>
#include<type_traits>
#include<utility>

namespace hgl
{
    // ============================================================================
    // 前置声明
    // ============================================================================
    
    template<typename T> class DataArrayBase;
    template<typename T> class TrivialDataArrayImpl;
    template<typename T> class NonTrivialDataArrayImpl;

    // ============================================================================
    // DataArrayBase - 公共接口和基础功能
    // ============================================================================
    
    /**
     * 数据阵列基类 - 定义公共接口
     */
    template<typename T> 
    class DataArrayBase : public Comparator<DataArrayBase<T>>
    {
    protected:
        T *items;
        int64 count;
        int64 alloc_count;

    public:
        DataArrayBase():items(nullptr),count(0),alloc_count(0){}
        // Query methods
        int64   GetCount()       const { return count; }
        int64   GetAllocCount()  const { return alloc_count; }
        int64   GetTotalBytes()  const { return count * sizeof(T); }
        int64   GetAllocBytes()  const { return alloc_count * sizeof(T); }
        bool    IsEmpty()        const { return count == 0; }
        T*      GetData()        const { return items; }
        T*      data()           const { return items; }

        // Iterator interface
        T*      begin()          const { return items; }
        T*      end()            const { return items + count; }
        T*      last()           const { return (count > 0) ? (items + (count - 1)) : nullptr; }

        // Element access
        T&      operator[](int64 n)       { return items[n]; }
        const T& operator[](int64 n) const { return items[n]; }
        
        T*      At(int64 n) const
        {
            return (n < 0 || n >= count) ? nullptr : items + n;
        }

        operator T*() const { return items; }
        T* operator->() const { return items; }

        // Comparison
        const int compare(const DataArrayBase<T> &other) const override
        {
            if(count == other.count)
                return mem_compare<T>(items, other.items, count);

            int cmp_count = hgl_min(count, other.count);
            int result = mem_compare<T>(items, other.items, cmp_count);

            if(result)
                return result;

            return count - other.count;
        }

        // Search
        int64 Find(const T &data, const int64 start = 0, int64 find_count = -1) const
        {
            if(!items || count <= 0 || start < 0 || start >= count) 
                return -1;

            if(find_count < 0 || find_count > count - start)
                find_count = count - start;

            return FindDataPositionInArray<T>(items + start, find_count, data);
        }

        bool Contains(const T &data) const
        {
            return Find(data) >= 0;
        }

        // ReadAt/WriteAt
        bool ReadAt(T &obj, const int64 index) const
        {
            if(index < 0 || index >= count) return false;
            mem_copy(obj, items[index]);
            return true;
        }

        bool ReadAt(T *obj, const int64 start, const int64 num) const
        {
            if(!obj || start < 0 || start + num > count) return false;
            mem_copy(obj, items + start, num);
            return true;
        }

        bool WriteAt(const T &obj, const int64 index)
        {
            if(index < 0 || index >= count) return false;
            mem_copy(items[index], obj);
            return true;
        }

        bool WriteAt(const T *obj, const int64 start, const int64 num)
        {
            if(!obj || start < 0 || start + num > count) return false;
            mem_copy(items + start, obj, num);
            return true;
        }

        // Expand
        int64 Expand(int64 size)
        {
            return Resize(count + size);
        }

        // Exchange
        void Exchange(int64 a, int64 b)
        {
            hgl_swap(items[a], items[b]);
        }

        // SetData
        void SetData(T *data, int64 data_count)
        {
            Free();
            items = data;
            alloc_count = count = data_count;
        }

        // Virtual interface - 必须由子类实现
        virtual bool Reserve(int64 size) = 0;
        virtual int64 Resize(int64 size) = 0;
        virtual void Free() = 0;
        virtual void Clear() = 0;
        virtual void Append(const T &obj) = 0;
        virtual bool Delete(int64 start, int64 delete_count = 1) = 0;
        virtual bool DeleteShift(int64 start, int64 delete_count = 1) = 0;
        virtual bool Insert(int64 pos, const T *data, int64 data_number) = 0;
        virtual void Move(int64 new_index, int64 old_index, int64 move_number = 1) = 0;

        // Assignment operators (implemented in base)
        void operator=(const DataArrayBase<T> &da)
        {
            if(da.count <= 0)
            {
                count = 0;
                return;
            }

            Resize(da.GetCount());
            mem_copy<T>(items, da.items, count);
        }

        void operator=(const std::initializer_list<T> &l)
        {
            Resize((int64)l.size());
            mem_copy<T>(items, l.begin(), count);
        }

        // Set operations
        void WithoutList(DataArrayBase<T> &result_list, const DataArrayBase<T> &without_list)
        {
            result_list.Clear();
            const int64 local_count = this->GetCount();

            if(local_count <= 0) return;

            result_list.Clear();
            result_list.Reserve(local_count);
            for(T &sp : *this)
            {
                if(!without_list.Contains(sp))
                    result_list.Append(sp);
            }
        }

        virtual ~DataArrayBase() {}
    };
}


template<typename T>
class hgl::TrivialDataArrayImpl : public hgl::DataArrayBase<T>
{
protected:
    using DataArrayBase<T>::items;
    using DataArrayBase<T>::count;
    using DataArrayBase<T>::alloc_count;

public:
    // 显式继承基类的公共方法（解决模板类名称查找问题）
    using DataArrayBase<T>::Exchange;
    using DataArrayBase<T>::Expand;
    using DataArrayBase<T>::SetData;
    using DataArrayBase<T>::ReadAt;
    using DataArrayBase<T>::WriteAt;
    using DataArrayBase<T>::operator=;  // 继承赋值运算符

    // 构造函数（初始化成员变量）
    TrivialDataArrayImpl() : DataArrayBase<T>() {}
    
    TrivialDataArrayImpl(int64 size) : DataArrayBase<T>()
    {
        if(size > 0)
            Resize(size);
    }

    virtual ~TrivialDataArrayImpl()
    {
        Free();
    }

    virtual bool Reserve(int64 size) override
    {
        if(size <= 0) return false;

        if(size != this->alloc_count)
        {
            T *new_items = hgl_align_malloc<T>(size);

            if(this->items)
            {
                if(this->count > size) this->count = size;

                mem_copy(new_items, this->items, this->count);

                hgl_free(this->items);
            }

            this->items = new_items;
            this->alloc_count = size;
        }

        return true;
    }

    virtual int64 Resize(int64 size) override
    {
        if(size <= 0)
        {
            Free();
            count = 0;
            return 0;
        }

        if(size > this->alloc_count)
            Reserve(size);

        this->count = size;

        return this->count;
    }

    virtual void Free() override
    {
        if(this->items)
        {
            hgl_free(this->items);
            this->items = nullptr;
            this->alloc_count = 0;
        }
        this->count = 0;
    }

    virtual void Clear() override
    {
        this->count = 0;
    }

    virtual void Append(const T &obj) override
    {
        if(this->count >= this->alloc_count)
            Reserve(this->alloc_count + 1);

        this->items[this->count] = obj;
        ++this->count;
    }

    virtual bool Delete(int64 start, int64 delete_count = 1) override
    {
        if(start < 0 || start >= this->count) return false;

        if(delete_count <= 0) return true;

        if(start + delete_count > this->count)
            delete_count = this->count - start;

        // 析构被删除的对象
        destroy_range<T>(this->items + start, delete_count);

        // 将后续元素前移
        for(int64 i = start + delete_count; i < this->count; ++i)
        {
            this->items[i - delete_count] = std::move(this->items[i]);
        }

        this->count -= delete_count;
        destroy_range<T>(this->items + this->count, delete_count);

        return true;
    }

    virtual bool DeleteShift(int64 start, int64 delete_count = 1) override
    {
        if(start < 0 || start >= this->count) return false;

        if(delete_count <= 0) return true;

        if(start + delete_count > this->count)
            delete_count = this->count - start;

        destroy_range<T>(this->items + start, delete_count);

        for(int64 i = start + delete_count; i < this->count; ++i)
        {
            this->items[i - delete_count] = std::move(this->items[i]);
        }

        this->count -= delete_count;
        destroy_range<T>(this->items + this->count, delete_count);

        return true;
    }

    virtual bool Insert(int64 pos, const T *data, int64 data_number) override
    {
        if(pos < 0 || pos > this->count) return false;
        if(data_number <= 0) return true;
        if(!data) return false;

        if(this->count + data_number > this->alloc_count)
            Reserve(this->count + data_number);

        if(pos < this->count)
        {
            int64 move_count = this->count - pos;
            for(int64 i = move_count - 1; i >= 0; --i)
            {
                int64 src = pos + i;
                int64 dst = pos + data_number + i;

                if(dst >= this->count)
                {
                    new (this->items + dst) T(std::move(this->items[src]));
                    this->items[src].~T();
                }
                else
                {
                    this->items[dst] = std::move(this->items[src]);
                }
            }
        }

        for(int64 i = 0; i < data_number; ++i)
        {
            int64 idx = pos + i;
            if(idx < this->count)
                this->items[idx] = data[i];
            else
                new (this->items + idx) T(data[i]);
        }

        this->count += data_number;

        return true;
    }

    void Unlink()
    {
        items = nullptr;
        count = 0;
        alloc_count = 0;
    }

    // Trivial 优化：Move 操作使用 ArrayRearrange
    virtual void Move(int64 new_index, int64 old_index, int64 move_number = 1) override
    {
        if(!items) return;
        if(new_index == old_index) return;
        if(old_index < 0 || old_index >= count) return;

        if(new_index < 0) new_index = 0;
        if(new_index >= count) new_index = count;

        if(old_index + move_number > count)
            move_number = count - old_index;

        if(move_number <= 0) return;

        // Trivial 类型：直接创建新缓冲区复制
        const int64 new_alloc_count = power_to_2(count);
        T *new_items = hgl_align_malloc<T>(new_alloc_count);

        bool result = false;

        if(old_index == 0)
        {
            if(new_index >= count)
            {
                result = ArrayRearrange(new_items, items, count,
                                      {move_number, count - move_number},
                                      {1, 0});
            }
            else
            {
                result = ArrayRearrange(new_items, items, count,
                                      {move_number, new_index - move_number},
                                      {1, 0, 2});
            }
        }
        else if(old_index + move_number >= count)
        {
            if(new_index == 0)
            {
                result = ArrayRearrange(new_items, items, count,
                                      {old_index, move_number},
                                      {1, 0});
            }
            else
            {
                result = ArrayRearrange(new_items, items, count,
                                      {new_index, old_index - new_index},
                                      {0, 2, 1});
            }
        }
        else
        {
            if(new_index == 0)
            {
                result = ArrayRearrange(new_items, items, count,
                                      {old_index, move_number},
                                      {1, 0, 2});
            }
            else if(new_index >= count)
            {
                result = ArrayRearrange(new_items, items, count,
                                      {old_index, move_number},
                                      {0, 2, 1});
            }
            else if(new_index < old_index)
            {
                result = ArrayRearrange(new_items, items, count,
                                      {new_index, old_index - new_index, move_number},
                                      {0, 2, 1, 3});
            }
            else
            {
                result = ArrayRearrange(new_items, items, count,
                                      {old_index, move_number, new_index - (old_index + move_number)},
                                      {0, 2, 1, 3});
            }
        }

        if(result)
        {
            hgl_free(items);
            items = new_items;
            alloc_count = new_alloc_count;
        }
        else
        {
            hgl_free(new_items);
        }
    }
};


template<typename T>
class hgl::NonTrivialDataArrayImpl : public hgl::DataArrayBase<T>
{
protected:
    using DataArrayBase<T>::items;
    using DataArrayBase<T>::count;
    using DataArrayBase<T>::alloc_count;

public:
    // 显式继承基类的公共方法（解决模板类名称查找问题）
    using DataArrayBase<T>::Exchange;
    using DataArrayBase<T>::Expand;
    using DataArrayBase<T>::SetData;
    using DataArrayBase<T>::ReadAt;
    using DataArrayBase<T>::WriteAt;
    using DataArrayBase<T>::operator=;  // 继承赋值运算符

    // 构造函数（初始化成员变量）
    NonTrivialDataArrayImpl() : DataArrayBase<T>() {}
    
    NonTrivialDataArrayImpl(int64 size) : DataArrayBase<T>()
    {
        if(size > 0)
            Resize(size);
    }

    virtual ~NonTrivialDataArrayImpl()
    {
        Free();
    }

    virtual bool Reserve(int64 size) override
    {
        if(size <= 0) return false;

        if(size <= this->alloc_count)
            return true;

        const int64 new_alloc = power_to_2(size);
        T *new_items = hgl_align_malloc<T>(new_alloc);

        // 移动构造已有元素到新缓冲区
        move_construct_range(new_items, this->items, this->count);

        destroy_range<T>(this->items, this->count);
        hgl_free(this->items);

        this->items = new_items;
        this->alloc_count = new_alloc;
        return true;
    }

    virtual int64 Resize(int64 size) override
    {
        if(size <= 0)
        {
            Free();
            return 0;
        }

        if(size > this->alloc_count)
            Reserve(size);

        if(size > this->count)
        {
            for(int64 i = this->count; i < size; ++i)
                new (this->items + i) T();
        }
        else if(size < this->count)
        {
            destroy_range<T>(this->items, size, this->count);
        }

        this->count = size;

        return this->count;
    }

    virtual void Free() override
    {
        if(this->items)
        {
            destroy_range<T>(this->items, this->count);
            hgl_free(this->items);
            this->items = nullptr;
            this->alloc_count = 0;
        }
        this->count = 0;
    }

    virtual void Clear() override
    {
        destroy_range<T>(this->items, this->count);
        this->count = 0;
    }

    virtual void Append(const T &obj) override
    {
        if(this->count >= this->alloc_count)
            Reserve(this->count + 1);

        new (this->items + this->count) T(obj);
        ++this->count;
    }

    virtual bool Delete(int64 start, int64 delete_count = 1) override
    {
        if(start < 0 || start >= this->count) return false;

        if(delete_count <= 0) return true;

        if(start + delete_count > this->count)
            delete_count = this->count - start;

        destroy_range<T>(this->items + start, delete_count);

        for(int64 i = start + delete_count; i < this->count; ++i)
        {
            this->items[i - delete_count] = std::move(this->items[i]);
        }

        this->count -= delete_count;
        destroy_range<T>(this->items + this->count, delete_count);

        return true;
    }

    virtual bool DeleteShift(int64 start, int64 delete_count = 1) override
    {
        if(start < 0 || start >= this->count) return false;

        if(delete_count <= 0) return true;

        if(start + delete_count > this->count)
            delete_count = this->count - start;

        destroy_range<T>(this->items + start, delete_count);

        for(int64 i = start + delete_count; i < this->count; ++i)
        {
            this->items[i - delete_count] = std::move(this->items[i]);
        }

        this->count -= delete_count;
        destroy_range<T>(this->items + this->count, delete_count);

        return true;
    }

    virtual bool Insert(int64 pos, const T *data, int64 data_number) override
    {
        if(pos < 0 || pos > this->count) return false;
        if(data_number <= 0) return true;
        if(!data) return false;

        if(this->count + data_number > this->alloc_count)
            Reserve(this->count + data_number);

        if(pos < this->count)
        {
            int64 move_count = this->count - pos;
            for(int64 i = move_count - 1; i >= 0; --i)
            {
                int64 src = pos + i;
                int64 dst = pos + data_number + i;

                if(dst >= this->count)
                {
                    new (this->items + dst) T(std::move(this->items[src]));
                    this->items[src].~T();
                }
                else
                {
                    this->items[dst] = std::move(this->items[src]);
                }
            }
        }

        for(int64 i = 0; i < data_number; ++i)
        {
            int64 idx = pos + i;
            if(idx < this->count)
                this->items[idx] = data[i];
            else
                new (this->items + idx) T(data[i]);
        }

        this->count += data_number;

        return true;
    }

    void Unlink()
    {
        items = nullptr;
        count = 0;
        alloc_count = 0;
    }

    // Non-trivial: Move 操作需要移动构造
    virtual void Move(int64 new_index, int64 old_index, int64 move_number = 1) override
    {
        if(!items) return;
        if(count <= 0) return;
        if(new_index == old_index) return;
        if(old_index < 0 || old_index >= count) return;

        if(new_index < 0) new_index = 0;
        if(new_index >= count) new_index = count;

        if(old_index + move_number > count)
            move_number = count - old_index;

        if(move_number <= 0) return;

        // Non-trivial 类型：需要小心处理对象生命周期
        const int64 new_alloc_count = power_to_2(count);
        T *new_items = hgl_align_malloc<T>(new_alloc_count);

        bool result = false;

        if(old_index == 0)
        {
            if(new_index >= count)
            {
                result = ArrayRearrange(new_items, items, count,
                                      {move_number, count - move_number},
                                      {1, 0});
            }
            else
            {
                result = ArrayRearrange(new_items, items, count,
                                      {move_number, new_index - move_number},
                                      {1, 0, 2});
            }
        }
        else if(old_index + move_number >= count)
        {
            if(new_index == 0)
            {
                result = ArrayRearrange(new_items, items, count,
                                      {old_index, move_number},
                                      {1, 0});
            }
            else
            {
                result = ArrayRearrange(new_items, items, count,
                                      {new_index, old_index - new_index},
                                      {0, 2, 1});
            }
        }
        else
        {
            if(new_index == 0)
            {
                result = ArrayRearrange(new_items, items, count,
                                      {old_index, move_number},
                                      {1, 0, 2});
            }
            else if(new_index >= count)
            {
                result = ArrayRearrange(new_items, items, count,
                                      {old_index, move_number},
                                      {0, 2, 1});
            }
            else if(new_index < old_index)
            {
                result = ArrayRearrange(new_items, items, count,
                                      {new_index, old_index - new_index, move_number},
                                      {0, 2, 1, 3});
            }
            else
            {
                result = ArrayRearrange(new_items, items, count,
                                      {old_index, move_number, new_index - (old_index + move_number)},
                                      {0, 2, 1, 3});
            }
        }

        if(result)
        {
            // 析构旧对象
            destroy_range<T>(items, count);
            hgl_free(items);
            items = new_items;
            alloc_count = new_alloc_count;
        }
        else
        {
            destroy_range<T>(new_items, count);
            hgl_free(new_items);
        }
    }
};

// ============================================================================
// DataArray - 智能类型选择器（用户接口）
// ============================================================================

namespace hgl
{
    /**
     * 数据阵列 - 根据类型特征自动选择最优实现
     * 
     * 编译时类型选择机制：
     * - Trivial 类型 → TrivialDataArrayImpl（高性能）
     * - Non-trivial 类型 → NonTrivialDataArrayImpl（完整支持）
     * 
     * 示例：
     *   DataArray<int> arr1;           // 自动使用 TrivialDataArrayImpl
     *   DataArray<float> arr2;          // 自动使用 TrivialDataArrayImpl
     *   DataArray<std::string> arr3;    // 自动使用 NonTrivialDataArrayImpl
     *   DataArray<std::vector<int>> arr4; // 自动使用 NonTrivialDataArrayImpl
     */
    template<typename T>
    using DataArray = typename std::conditional<
        std::is_trivially_copyable_v<T> && std::is_trivially_destructible_v<T>,
        TrivialDataArrayImpl<T>,
        NonTrivialDataArrayImpl<T>
    >::type;

    /**
     * 向后兼容的别名
     */
    template<typename T>
    using TrivialDataArray = TrivialDataArrayImpl<T>;

} // namespace hgl
