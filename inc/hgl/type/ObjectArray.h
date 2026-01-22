#pragma once

#include<type_traits>
#include<utility>
#include<algorithm>
#include<initializer_list>
#include<hgl/type/ObjectUtil.h>

namespace hgl
{
    /**
     * 非平凡类型专用的阵列容器。
     * 与 DataArray 接口尽量保持一致，手动管理内存并显式调用构造/析构函数。
     */
    template<typename T> class ObjectArray
    {
        static_assert(!std::is_trivially_copyable_v<T>, "Use DataArray for trivially copyable types");

    private:
        T*  data_;
        int count_;
        int alloc_count_;

    private:
        // 确保容量充足，按 2 的指数扩张
        void ensure_capacity(int desired)
        {
            if(desired <= alloc_count_)
                return;

            int new_alloc = alloc_count_ == 0 ? 8 : alloc_count_;
            while(new_alloc < desired)
                new_alloc *= 2;

            data_ = reallocate_and_move(data_, count_, new_alloc);
            alloc_count_ = new_alloc;
        }

    public: // 属性

        int     GetAllocCount   ()const { return alloc_count_; }
        int     GetCount        ()const { return count_; }
        int     GetTotalBytes   ()const { return count_ * static_cast<int>(sizeof(T)); }
        bool    IsEmpty         ()const { return count_ == 0; }

        int     GetAllocBytes   ()const { return alloc_count_ * static_cast<int>(sizeof(T)); }

        T*      GetData         (){ return data_; }
        const T*GetData         ()const{ return data_; }
        T*      data            (){ return data_; }
        const T*data            ()const{ return data_; }

        T*      begin           (){ return data_; }
        T*      end             (){ return data_ + count_; }
        const T*begin           ()const{ return data_; }
        const T*end             ()const{ return data_ + count_; }
        T*      last            (){ return count_ == 0 ? nullptr : data_ + count_ - 1; }
        const T*last            ()const{ return count_ == 0 ? nullptr : data_ + count_ - 1; }

    public:

        ObjectArray() : data_(nullptr), count_(0), alloc_count_(0) {}

        explicit ObjectArray(int size) : data_(nullptr), count_(0), alloc_count_(0)
        {
            if(size > 0)
            {
                Resize(size);
            }
        }

        ObjectArray(const T* lt, const int n) : data_(nullptr), count_(0), alloc_count_(0)
        {
            if(lt && n > 0)
            {
                Reserve(n);
                copy_construct_range(data_, lt, n);
                count_ = n;
            }
        }

        ObjectArray(const std::initializer_list<T>& lt) : data_(nullptr), count_(0), alloc_count_(0)
        {
            const int n = static_cast<int>(lt.size());
            if(n > 0)
            {
                Reserve(n);
                auto it = lt.begin();
                for(int i = 0; i < n; ++i, ++it)
                    construct_at_copy(data_ + i, *it);
                count_ = n;
            }
        }

        ObjectArray(ObjectArray&& other) noexcept
            : data_(other.data_), count_(other.count_), alloc_count_(other.alloc_count_)
        {
            other.data_ = nullptr;
            other.count_ = 0;
            other.alloc_count_ = 0;
        }

        ObjectArray& operator=(ObjectArray&& other) noexcept
        {
            if(this == &other)
                return *this;

            Free();
            data_ = other.data_;
            count_ = other.count_;
            alloc_count_ = other.alloc_count_;

            other.data_ = nullptr;
            other.count_ = 0;
            other.alloc_count_ = 0;
            return *this;
        }

        ~ObjectArray()
        {
            destroy_range(data_, count_);
            deallocate_raw_memory(data_);
        }

        // 禁止拷贝（避免双重释放）
        ObjectArray(const ObjectArray&) = delete;
        ObjectArray& operator=(const ObjectArray&) = delete;

    public: // 访问

        operator T*() { return data_; }
        operator const T*() const { return data_; }

        T& operator[](int index) { return data_[index]; }
        const T& operator[](int index)const { return data_[index]; }

        T* At(int index)
        {
            return (index < 0 || index >= count_) ? nullptr : data_ + index;
        }

        const T* At(int index)const
        {
            return (index < 0 || index >= count_) ? nullptr : data_ + index;
        }

        int Find(const T& data)const
        {
            for(int i = 0; i < count_; ++i)
            {
                if(data_[i] == data)
                    return i;
            }
            return -1;
        }

        bool Contains(const T& data)const
        {
            return Find(data) >= 0;
        }

    public: // 容量管理

        bool Reserve(int count)
        {
            if(count < 0) return false;
            ensure_capacity(count);
            return true;
        }

        bool Resize(int count)
        {
            if(count < 0) return false;

            if(count > alloc_count_)
                ensure_capacity(count);

            if(count > count_)
                default_construct_range(data_ + count_, count - count_);
            else if(count < count_)
                destroy_range(data_ + count, count_ - count);

            count_ = count;
            return true;
        }

        int Expand(int count)
        {
            if(count <= 0) return count_;
            Resize(count_ + count);
            return count_;
        }

        void SetData(T* data, int data_count)
        {
            Free();
            data_ = data;
            count_ = data_count;
            alloc_count_ = data_count;
        }

        void Unlink()
        {
            data_ = nullptr;
            count_ = 0;
            alloc_count_ = 0;
        }

    public: // 读写

        bool ReadAt(T& data, int index)const
        {
            if(index < 0 || index >= count_) return false;
            data = data_[index];
            return true;
        }

        bool ReadAt(T* data, int start, int num)const
        {
            if(!data || start < 0 || num < 0 || start + num > count_) return false;
            for(int i = 0; i < num; ++i)
                data[i] = data_[start + i];
            return true;
        }

        bool WriteAt(const T& data, int index)
        {
            if(index < 0 || index >= count_) return false;
            data_[index] = data;
            return true;
        }

        bool WriteAt(const T* data, int start, int num)
        {
            if(!data || start < 0 || num < 0 || start + num > count_) return false;
            for(int i = 0; i < num; ++i)
                data_[start + i] = data[i];
            return true;
        }

        void Zero()
        {
            for(int i = 0; i < count_; ++i)
                data_[i] = T();
        }

    public: // 修改

        void Append(const T& obj)
        {
            ensure_capacity(count_ + 1);
            construct_at_copy(data_ + count_, obj);
            ++count_;
        }

        /**
         * 向列表中添加一个默认构造的对象
         * @return 这个对象的指针
         */
        T* Add()
        {
            ensure_capacity(count_ + 1);
            construct_at_default(data_ + count_);
            ++count_;
            return data_ + (count_ - 1);
        }

        /**
         * 追加一个对象，返回其索引
         */
        int Add(const T& obj)
        {
            const int index = count_;
            Append(obj);
            return index;
        }

        /**
         * 追加一批对象，返回起始索引
         */
        int Add(const T* obj, int number)
        {
            if(!obj || number <= 0)
                return -1;

            const int index = count_;
            Append(obj, number);
            return index;
        }

        /**
         * 从另一个 ObjectArray 追加全部数据
         */
        int Add(const ObjectArray<T>& other)
        {
            return Add(other.data_, other.count_);
        }

        void Exchange(int a, int b)
        {
            if(a < 0 || b < 0 || a >= count_ || b >= count_) return;
            std::swap(data_[a], data_[b]);
        }

        bool Delete(int start, int num = 1)
        {
            if(start < 0 || num <= 0 || start >= count_) return false;

            int end_pos = start + num;
            if(end_pos > count_) end_pos = count_;

            int delete_count = end_pos - start;

            destroy_range(data_ + start, delete_count);

            int move_count = count_ - end_pos;
            for(int i = 0; i < move_count; ++i)
            {
                construct_at_move(data_ + start + i, std::move(data_[end_pos + i]));
                destroy_at(data_ + end_pos + i);
            }

            count_ -= delete_count;
            return true;
        }

        bool DeleteShift(int start, int num = 1)
        {
            if(start < 0 || num <= 0 || start >= count_) return false;

            int end_pos = start + num;
            if(end_pos > count_) end_pos = count_;

            int delete_count = end_pos - start;

            destroy_range(data_ + start, delete_count);

            for(int i = end_pos; i < count_; ++i)
            {
                construct_at_move(data_ + i - delete_count, std::move(data_[i]));
                destroy_at(data_ + i);
            }

            count_ -= delete_count;
            return true;
        }

        void Move(const int new_pos, const int old_pos, const int move_count)
        {
            if(move_count <= 0) return;
            if(old_pos < 0 || old_pos >= count_) return;

            int mc = move_count;
            if(old_pos + mc > count_) mc = count_ - old_pos;
            if(mc <= 0) return;

            int np = new_pos;
            if(np < 0) np = 0;
            if(np > count_) np = count_;

            if(np >= old_pos && np <= old_pos + mc) return;

            T* temp = allocate_raw_memory<T>(mc);
            move_construct_range(temp, data_ + old_pos, mc);
            destroy_range(data_ + old_pos, mc);

            if(np < old_pos)
            {
                for(int i = old_pos - 1; i >= np; --i)
                {
                    construct_at_move(data_ + i + mc, std::move(data_[i]));
                    destroy_at(data_ + i);
                }
            }
            else
            {
                for(int i = old_pos; i < np - mc; ++i)
                {
                    construct_at_move(data_ + i, std::move(data_[i + mc]));
                    destroy_at(data_ + i + mc);
                }
                np -= mc;
            }

            move_construct_range(data_ + np, temp, mc);
            destroy_range(temp, mc);
            deallocate_raw_memory(temp);
        }

        bool Insert(int pos, const T* data, const int number)
        {
            if(!data || number <= 0) return false;
            if(pos < 0) pos = 0;
            if(pos > count_) pos = count_;

            int new_count = count_ + number;
            ensure_capacity(new_count);

            if(pos < count_)
            {
                for(int i = count_ - 1; i >= pos; --i)
                {
                    construct_at_move(data_ + i + number, std::move(data_[i]));
                    destroy_at(data_ + i);
                }
            }

            copy_construct_range(data_ + pos, data, number);
            count_ = new_count;
            return true;
        }

        bool Insert(int pos, const T& data)
        {
            return Insert(pos, &data, 1);
        }

        void Append(const T* data, int number)
        {
            if(!data || number <= 0) return;
            ensure_capacity(count_ + number);
            copy_construct_range(data_ + count_, data, number);
            count_ += number;
        }

    public: // 状态维护

        void Clear()
        {
            destroy_range(data_, count_);
            count_ = 0;
        }

        void Free()
        {
            destroy_range(data_, count_);
            deallocate_raw_memory(data_);
            data_ = nullptr;
            count_ = 0;
            alloc_count_ = 0;
        }

    public: // 赋值辅助

        ObjectArray& operator=(const std::initializer_list<T>& l)
        {
            Clear();
            const int n = static_cast<int>(l.size());
            if(n <= 0)
                return *this;

            ensure_capacity(n);
            auto it = l.begin();
            for(int i = 0; i < n; ++i, ++it)
                construct_at_copy(data_ + i, *it);
            count_ = n;
            return *this;
        }

        void WithoutList(ObjectArray<T>& result_list, const ObjectArray<T>& without_list)
        {
            result_list.Clear();
            if(count_ <= 0) return;

            result_list.Reserve(count_);
            for(int i = 0; i < count_; ++i)
            {
                if(!without_list.Contains(data_[i]))
                    result_list.Append(data_[i]);
            }
        }
    };
}
