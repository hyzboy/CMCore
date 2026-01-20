#pragma once

#include<type_traits>
#include<algorithm>
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
        T*      data_;
        int     count_;
        int     alloc_count_;

    public: // 属性

        int     GetAllocCount   ()const { return alloc_count_; }
        int     GetCount        ()const { return count_; }

        bool    Resize(int count)
        {
            if(count < 0) return false;
            
            if(count > alloc_count_)
            {
                data_ = reallocate_and_move(data_, count_, count);
                alloc_count_ = count;
            }

            // 如果扩大，默认构造新元素
            if(count > count_)
            {
                default_construct_range(data_ + count_, count - count_);
            }
            // 如果缩小，销毁多余元素
            else if(count < count_)
            {
                destroy_range(data_ + count, count_ - count);
            }

            count_ = count;
            return true;
        }

        bool    Reserve(int count)
        {
            if(count < 0) return false;
            if(count > alloc_count_)
            {
                data_ = reallocate_and_move(data_, count_, count);
                alloc_count_ = count;
            }
            return true;
        }

        bool    IsEmpty()const { return count_ == 0; }

        T*      GetData()       { return data_; }
        const T* GetData()const { return data_; }
        int     GetTotalBytes()const { return count_ * static_cast<int>(sizeof(T)); }

        T*      begin()         { return data_; }
        T*      end()           { return data_ + count_; }
        const T* begin()const   { return data_; }
        const T* end()const     { return data_ + count_; }
        T*      last()          { return count_ == 0 ? nullptr : data_ + count_ - 1; }

    public:

        ObjectArray() : data_(nullptr), count_(0), alloc_count_(0) {}

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
                {
                    construct_at_copy(data_ + i, *it);
                }
                count_ = n;
            }
        }

        ~ObjectArray()
        {
            destroy_range(data_, count_);
            deallocate_raw_memory(data_);
        }

        // 禁止拷贝（避免双重释放）
        ObjectArray(const ObjectArray&) = delete;
        ObjectArray& operator=(const ObjectArray&) = delete;

    public: // 方法

        void Expand(int count)
        {
            if(count <= 0) return;

            int new_count = count_ + count;
            if(new_count > alloc_count_)
            {
                int new_alloc = alloc_count_ == 0 ? 8 : alloc_count_ * 2;
                while(new_alloc < new_count)
                    new_alloc *= 2;
                data_ = reallocate_and_move(data_, count_, new_alloc);
                alloc_count_ = new_alloc;
            }

            default_construct_range(data_ + count_, count);
            count_ = new_count;
        }

        bool ReadAt(T& data, int index)const
        {
            if(index < 0 || index >= count_) return false;
            data = data_[index];
            return true;
        }

        bool WriteAt(const T& data, int index)
        {
            if(index < 0 || index >= count_) return false;
            data_[index] = data;
            return true;
        }

        bool Insert(int pos, const T* data, const int number)
        {
            if(!data || number <= 0) return false;
            if(pos < 0) pos = 0;
            if(pos > count_) pos = count_;

            int new_count = count_ + number;
            if(new_count > alloc_count_)
            {
                int new_alloc = alloc_count_ == 0 ? 8 : alloc_count_ * 2;
                while(new_alloc < new_count)
                    new_alloc *= 2;

                T* new_data = allocate_raw_memory<T>(new_alloc);

                // 移动 [0, pos) 到新内存
                move_construct_range(new_data, data_, pos);

                // 复制插入的数据
                copy_construct_range(new_data + pos, data, number);

                // 移动 [pos, count_) 到新位置
                move_construct_range(new_data + pos + number, data_ + pos, count_ - pos);

                // 销毁并释放旧内存
                destroy_range(data_, count_);
                deallocate_raw_memory(data_);
                
                data_ = new_data;
                alloc_count_ = new_alloc;
            }
            else
            {
                // 从后向前移动元素，为插入腾出空间
                for(int i = count_ - 1; i >= pos; --i)
                {
                    construct_at_move(data_ + i + number, std::move(data_[i]));
                    destroy_at(data_ + i);
                }

                // 插入新数据
                copy_construct_range(data_ + pos, data, number);
            }

            count_ = new_count;
            return true;
        }

        bool Insert(int pos, const T& data)
        {
            return Insert(pos, &data, 1);
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

            // 临时保存要移动的对象
            T* temp = allocate_raw_memory<T>(mc);
            move_construct_range(temp, data_ + old_pos, mc);
            destroy_range(data_ + old_pos, mc);

            // 移动其他元素填补空隙或腾出空间
            if(np < old_pos)
            {
                // 向前移动：[np, old_pos) 向后移动 mc 位
                for(int i = old_pos - 1; i >= np; --i)
                {
                    construct_at_move(data_ + i + mc, std::move(data_[i]));
                    destroy_at(data_ + i);
                }
            }
            else
            {
                // 向后移动：[old_pos+mc, np) 向前移动 mc 位
                for(int i = old_pos; i < np - mc; ++i)
                {
                    construct_at_move(data_ + i, std::move(data_[i + mc]));
                    destroy_at(data_ + i + mc);
                }
                np -= mc;
            }

            // 放置到新位置
            move_construct_range(data_ + np, temp, mc);
            destroy_range(temp, mc);
            deallocate_raw_memory(temp);
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

            // 销毁要删除的元素
            destroy_range(data_ + start, delete_count);

            // 移动后面的元素向前填补（不保持顺序，快速删除）
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

            // 销毁要删除的元素
            destroy_range(data_ + start, delete_count);

            // 移动后面的元素向前（保持顺序）
            for(int i = end_pos; i < count_; ++i)
            {
                construct_at_move(data_ + i - delete_count, std::move(data_[i]));
                destroy_at(data_ + i);
            }

            count_ -= delete_count;
            return true;
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

        T* At(const int index)
        {
            return (index < 0 || index >= count_) ? nullptr : &data_[index];
        }

        const T* At(const int index)const
        {
            return (index < 0 || index >= count_) ? nullptr : &data_[index];
