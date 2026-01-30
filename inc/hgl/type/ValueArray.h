#pragma once

#include<stdlib.h>
#include<initializer_list>
#include<type_traits>
#include<vector>
#include<algorithm>
#include<cstring>
#include<hgl/type/MemoryUtil.h>

namespace hgl
{
    /**
    * ValueArray类用于保存数据列表。可以在列表中添加、删除、查找、访问和排序数据。<br>
    * ValueArray使用真实的数组保存数据，没有独立的索引，所以仅适用于不会变动的小数据的保存与访问。<br>
    * 如果使用大块的数据，仅需要频繁的增删排序，建议使用IndexedValueArray.
    */
    template<typename T> class ValueArray                                                            ///阵列列表处理类
    {
    protected:

        std::vector<T> data_array;

    public: //属性

        static_assert(std::is_trivially_copyable_v<T>,
                      "ValueArray<T> requires trivially copyable types (int, float, POD structs, etc). "
                      "For non-trivial types (std::string, custom classes with dynamic memory), use ManagedArray<T> instead.");

                const   int     GetAllocCount   ()const{return (int)data_array.capacity();}         ///<取得已分配容量
                const   int     GetCount        ()const{return (int)data_array.size();}              ///<取得列表内数据数量
        virtual         bool    Resize          (int count){data_array.resize(count); return true;}       ///<设置列表内数据数量
        virtual         bool    Reserve         (int count){data_array.reserve(count); return true;}      ///<预分配指定数量的数据空间

                const   bool    IsEmpty         ()const{return data_array.empty();}               ///<确认列表是否为空

                        T *     GetData         ()const{return data_array.empty()?nullptr:const_cast<T*>(data_array.data());}               ///<提供原始数据项
                        int     GetTotalBytes   ()const{return (int)(data_array.size()*sizeof(T));}         ///<取得原始数据总字节数

                        T *     begin           ()const{return data_array.empty()?nullptr:const_cast<T*>(data_array.data());}
                        T *     end             ()const{return data_array.empty()?nullptr:const_cast<T*>(data_array.data())+data_array.size();}
                        T *     last            ()const{return data_array.empty()?nullptr:const_cast<T*>(data_array.data())+data_array.size()-1;}

    public:

            std::vector<T> &      GetArray()          {return data_array;}
            const std::vector<T> &GetArray()const     {return data_array;}

            operator        std::vector<T> & ()       {return data_array;}
            operator const  std::vector<T> & ()const  {return data_array;}
                    T &     operator[](int index)             {return data_array[index];}
            const   T &     operator[](int index)const        {return data_array[index];}

    public: //方法

        ValueArray()=default;                                                                             ///<本类构造函数
        ValueArray(const T *lt,const int n){Add(lt,n);}                                                   ///<本类构造函数
        ValueArray(const ValueArray<T> &lt){operator=(lt);}                                                     ///<本类构造函数
        ValueArray(const std::initializer_list<T> &lt){operator=(lt);}

        virtual ~ValueArray(){Free();}                                                                    ///<本类析构函数

        /**
         * 向列表中添加一个空数据
         * @return 这个数据的指针
         */
        virtual T *  Add()
        {
            const int index=(int)data_array.size();
            data_array.resize(index+1);
            return &data_array.back();
        }

        /**
        * 向列表中添加一个数据对象
        * @param data 要添加的数据对象
        * @return 这个数据的索引号
        */
        virtual int  Add(const T &data)
        {
            const int index=(int)data_array.size();
            if constexpr (std::is_array_v<T>)
            {
                data_array.resize(index+1);
                std::memcpy(&data_array[index], &data, sizeof(T));
            }
            else
            {
                data_array.push_back(data);
            }
            return index;
        }

        /**
        * 重复向列表中添加一个数据对象
        * @param data 要添加的数据对象
        * @param n 要添加的数据个数
        * @return 这个数据的索引号
        * @return >0 出错
        */
        virtual int  RepeatAdd(const T &data,int n)
        {
            if(n<=0)return(-1);

            const int ec=(int)data_array.size();
            if constexpr (std::is_array_v<T>)
            {
                data_array.resize(ec + n);
                for(int i=0;i<n;i++)
                    std::memcpy(&data_array[ec+i], &data, sizeof(T));
            }
            else
            {
                data_array.insert(data_array.end(), n, data);
            }
            return(ec);
        }

        /**
        * 向列表中添加一批数据对象
        * @param data 要添加的数据对象
        * @param n 要添加的数据数量
        * @return 起始数据的索引号
        */
        virtual int  Add(const T *data,int n)
        {
            if(!data||n<=0)
                return(-1);

            const int ec=(int)data_array.size();
            if constexpr (std::is_array_v<T>)
            {
                data_array.resize(ec + n);
                for(int i=0;i<n;i++)
                    std::memcpy(&data_array[ec+i], &data[i], sizeof(T));
            }
            else
            {
                data_array.insert(data_array.end(), data, data+n);
            }
            return(ec);
        }

                int  Add(const ValueArray<T> &l){return Add(l.GetData(),l.GetCount());}                            ///<增加一批数据

        virtual void Free()
        {
            data_array.clear();
            if constexpr (!std::is_array_v<T>)
            {
                data_array.shrink_to_fit();
            }
        }                                                     ///<清除所有数据，并释放内存
        virtual void Clear(){data_array.clear();}                                                   ///<清除所有数据，但不清空缓冲区

        virtual int  Find(const T &data)const
        {
            if constexpr (std::is_array_v<T>)
            {
                // 对数组类型使用 memcmp 比较
                for(size_t i=0; i<data_array.size(); ++i)
                {
                    if(std::memcmp(&data_array[i], &data, sizeof(T)) == 0)
                        return (int)i;
                }
                return -1;
            }
            else
            {
                auto it = std::find(data_array.begin(), data_array.end(), data);
                return (it == data_array.end()) ? -1 : (int)std::distance(data_array.begin(), it);
            }
        }                        ///<查找指定数据的索引

        virtual bool Contains(const T &flag)const{return Find(flag)>=0;}                            ///<确认数据项是否存在

        virtual bool Delete(int start,int num=1)
        {
            if(start<0 || start+num>(int)data_array.size()) return false;
            
            if constexpr (std::is_array_v<T>)
            {
                // 对数组类型手动删除并移动元素
                int tail_count = (int)data_array.size() - start - num;
                if(tail_count > 0)
                {
                    for(int i=0; i<tail_count; ++i)
                        std::memcpy(&data_array[start+i], &data_array[start+num+i], sizeof(T));
                }
                data_array.resize(data_array.size() - num);
            }
            else
            {
                data_array.erase(data_array.begin()+start, data_array.begin()+start+num);
            }
            return true;
        }              ///<删除指定索引的数据

        virtual bool DeleteShift(int start,int num=1)
        {
            return Delete(start, num);
        }      ///<删除指定索引的数据,将后面紧邻的数据前移

        /**
        * 删除列表中的指定项
        * @param data 要删除的数据项
        * @return 是否成功
        */
        virtual bool DeleteByValue(T &data)
        {
            const int pos=Find(data);

            return(pos>=0?Delete(pos,1):false);
        }

        /**
        * 删除列表中的指定项
        * @param data 要删除的数据项
        * @param n 要删除的数据个数
        * @return 成功删除的数据个数
        */
        virtual int  DeleteByValue(T *data,int n)
        {
            int result=0;

            while(n--)
            {
                int index=Find(*data);

                ++data;

                if(index>=0)
                    if(Delete(index))
                        ++result;
            }

            return result;
        }

        virtual void Exchange(int a,int b)
        {
            if(a>=0 && a<(int)data_array.size() && b>=0 && b<(int)data_array.size())
            {
                if constexpr (std::is_array_v<T>)
                {
                    T temp;
                    std::memcpy(&temp, &data_array[a], sizeof(T));
                    std::memcpy(&data_array[a], &data_array[b], sizeof(T));
                    std::memcpy(&data_array[b], &temp, sizeof(T));
                }
                else
                {
                    std::swap(data_array[a], data_array[b]);
                }
            }
        }                               ///<根据索引交换两个数据

        virtual bool Insert(int pos,const T &data)
        {
            if(pos<0 || pos>(int)data_array.size()) return false;
            
            if constexpr (std::is_array_v<T>)
            {
                data_array.resize(data_array.size() + 1);
                // 向后移动元素
                for(int i=(int)data_array.size()-1; i>pos; --i)
                    std::memcpy(&data_array[i], &data_array[i-1], sizeof(T));
                // 插入新元素
                std::memcpy(&data_array[pos], &data, sizeof(T));
            }
            else
            {
                data_array.insert(data_array.begin()+pos, data);
            }
            return true;
        }          ///<在指定索引处插入一个数据

        /**
        * 在指定索引处插入一批数据
        * @param pos 插入的位置
        * @param data 要插入的数据
        * @param number 要插入的数据个数
        */
        virtual bool Insert(int pos,const T *data,const int number)
        {
            if(!data || number<=0 || pos<0 || pos>(int)data_array.size()) return false;
            
            if constexpr (std::is_array_v<T>)
            {
                int old_size = (int)data_array.size();
                data_array.resize(old_size + number);
                // 向后移动元素
                for(int i=old_size-1; i>=pos; --i)
                    std::memcpy(&data_array[i+number], &data_array[i], sizeof(T));
                // 插入新元素
                for(int i=0; i<number; ++i)
                    std::memcpy(&data_array[pos+i], &data[i], sizeof(T));
            }
            else
            {
                data_array.insert(data_array.begin()+pos, data, data+number);
            }
            return true;
        }

        /**
        * 移动一批数据到新的位置
        * @param new_pos 新的位置
        * @param old_pos 原来的位置
        * @param move_count 要移动的数据个数
        */
        virtual void Move(const int new_pos,const int old_pos,const int move_count)
        {
            if(old_pos<0 || old_pos+move_count>(int)data_array.size()) return;
            if(new_pos<0 || new_pos>(int)data_array.size()) return;
            if(move_count<=0) return;

            // 如果目标位置在源范围内，不处理
            if(new_pos>=old_pos && new_pos<=old_pos+move_count) return;

            if constexpr (std::is_array_v<T>)
            {
                // 对数组类型使用 memcpy 进行移动
                std::vector<T> temp(move_count);
                for(int i=0; i<move_count; ++i)
                    std::memcpy(&temp[i], &data_array[old_pos+i], sizeof(T));
                
                // 手动移除元素（避免调用erase）
                int actual_new_pos = new_pos;
                if(new_pos > old_pos)
                {
                    actual_new_pos -= move_count;
                    // 向前移动元素
                    for(int i=old_pos; i<actual_new_pos; ++i)
                        std::memcpy(&data_array[i], &data_array[i+move_count], sizeof(T));
                }
                else
                {
                    // 向后移动元素
                    for(int i=old_pos-1; i>=actual_new_pos; --i)
                        std::memcpy(&data_array[i+move_count], &data_array[i], sizeof(T));
                }
                
                // 复制临时元素到目标位置
                for(int i=0; i<move_count; ++i)
                    std::memcpy(&data_array[actual_new_pos+i], &temp[i], sizeof(T));
            }
            else
            {
                std::vector<T> temp(data_array.begin()+old_pos, data_array.begin()+old_pos+move_count);
                data_array.erase(data_array.begin()+old_pos, data_array.begin()+old_pos+move_count);

                int actual_new_pos = new_pos;
                if(new_pos > old_pos)
                    actual_new_pos -= move_count;

                data_array.insert(data_array.begin()+actual_new_pos, temp.begin(), temp.end());
            }
        }

        virtual ValueArray<T>& operator = (const ValueArray<T> &da)
        {
            if constexpr (std::is_array_v<T>)
            {
                data_array.resize(da.data_array.size());
                for(size_t i=0;i<da.data_array.size();++i)
                    std::memcpy(&data_array[i], &da.data_array[i], sizeof(T));
            }
            else
            {
                data_array = da.data_array;
            }
            return *this;
        }

        virtual ValueArray<T>& operator = (const std::initializer_list<T> &l)
        {
            if constexpr (std::is_array_v<T>)
            {
                data_array.resize(l.size());
                size_t i=0;
                for(const auto &item : l)
                {
                    std::memcpy(&data_array[i], &item, sizeof(T));
                    ++i;
                }
            }
            else
            {
                data_array.assign(l.begin(), l.end());
            }
            return *this;
        }                  ///<操作符重载复制一个列表

        virtual void operator += (T &obj){Add(obj);}                                                ///<操作符重载添加一个数据
        virtual void operator << (T &obj){Add(obj);}                                                ///<操作符重载添加一个数据
        virtual void operator -= (T &obj){DeleteByValue(obj);}                                      ///<操作符重载删除一个数据

                T *  At(const int index)
        {
            if(index<0 || index>=(int)data_array.size()) return nullptr;
            return &data_array[index];
        }                         ///<取得指定序列号数据的索引

        const   T *  At(const int index)const
        {
            if(index<0 || index>=(int)data_array.size()) return nullptr;
            return &data_array[index];
        }                         ///<取得指定序列号数据的索引

                bool Get(int index,      T &data)const
        {
            if(index<0 || index>=(int)data_array.size()) return false;
            if constexpr (std::is_array_v<T>)
            {
                std::memcpy(&data, &data_array[index], sizeof(T));
            }
            else
            {
                data = data_array[index];
            }
            return true;
        }    ///<取得指定索引处的数据

        virtual bool Set(int index,const T &data)
        {
            if(index<0 || index>=(int)data_array.size()) return false;
            if constexpr (std::is_array_v<T>)
            {
                std::memcpy(&data_array[index], &data, sizeof(T));
            }
            else
            {
                data_array[index] = data;
            }
            return true;
        }    ///<设置指定索引处的数据

        virtual bool GetFirst   (T &data)const{return Get(0, data);}                   ///<取第一个数据
        virtual bool GetLast    (T &data)const{return Get((int)data_array.size()-1, data);}        ///<取最后一个数据
    };//template <typename T> class ValueArray

    /**
     * @brief Specialization for array types (e.g., uint64[256])
     * Uses byte-level storage to avoid std::vector's array type construction issues
     */
    template<typename ElementType, size_t ArraySize>
    class ValueArray<ElementType[ArraySize]>
    {
    protected:
        using ArrayType = ElementType[ArraySize];
        std::vector<uint8_t> byte_storage;
        
        int GetElementCount() const { return (int)(byte_storage.size() / sizeof(ArrayType)); }
        ArrayType* GetElementPtr(int idx) { return reinterpret_cast<ArrayType*>(byte_storage.data() + idx * sizeof(ArrayType)); }
        const ArrayType* GetElementPtr(int idx) const { return reinterpret_cast<const ArrayType*>(byte_storage.data() + idx * sizeof(ArrayType)); }

    public:
        int GetAllocCount() const { return (int)(byte_storage.capacity() / sizeof(ArrayType)); }
        int GetCount() const { return GetElementCount(); }
        bool IsEmpty() const { return byte_storage.empty(); }
        
        bool Resize(int count) 
        { 
            if (count < 0) return false;
            byte_storage.resize((size_t)count * sizeof(ArrayType));
            return true;
        }
        
        bool Reserve(int count)
        {
            if (count < 0) return false;
            byte_storage.reserve((size_t)count * sizeof(ArrayType));
            return true;
        }

        int GetTotalBytes() const { return (int)byte_storage.size(); }
        
        ArrayType* GetData() const 
        { 
            return byte_storage.empty() ? nullptr : const_cast<ArrayType*>(reinterpret_cast<const ArrayType*>(byte_storage.data()));
        }
        
        ArrayType* begin() const { return GetData(); }
        ArrayType* end() const { return GetData() + GetElementCount(); }
        ArrayType* last() const { return GetElementCount() == 0 ? nullptr : GetData() + GetElementCount() - 1; }

        bool Clear() { byte_storage.clear(); return true; }
        bool Free() { std::vector<uint8_t>().swap(byte_storage); return true; }

        bool Add(const ArrayType& data)
        {
            size_t offset = byte_storage.size();
            byte_storage.resize(offset + sizeof(ArrayType));
            std::memcpy(byte_storage.data() + offset, &data, sizeof(ArrayType));
            return true;
        }

        bool Insert(int index, const ArrayType& data)
        {
            int count = GetElementCount();
            if (index < 0 || index > count) return false;

            byte_storage.insert(
                byte_storage.begin() + (ptrdiff_t)index * sizeof(ArrayType),
                reinterpret_cast<const uint8_t*>(&data),
                reinterpret_cast<const uint8_t*>(&data) + sizeof(ArrayType)
            );
            return true;
        }

        bool Get(int index, ArrayType& data) const
        {
            int count = GetElementCount();
            if (index < 0 || index >= count) return false;
            std::memcpy(&data, byte_storage.data() + (size_t)index * sizeof(ArrayType), sizeof(ArrayType));
            return true;
        }

        bool Set(int index, const ArrayType& data)
        {
            int count = GetElementCount();
            if (index < 0 || index >= count) return false;
            std::memcpy(byte_storage.data() + (size_t)index * sizeof(ArrayType), &data, sizeof(ArrayType));
            return true;
        }

        int Find(const ArrayType& data) const
        {
            int count = GetElementCount();
            const uint8_t* search_bytes = reinterpret_cast<const uint8_t*>(&data);
            
            for (int i = 0; i < count; ++i)
            {
                if (std::memcmp(byte_storage.data() + (size_t)i * sizeof(ArrayType), search_bytes, sizeof(ArrayType)) == 0)
                    return i;
            }
            return -1;
        }

        bool Delete(int index)
        {
            int count = GetElementCount();
            if (index < 0 || index >= count) return false;
            byte_storage.erase(
                byte_storage.begin() + (ptrdiff_t)index * sizeof(ArrayType),
                byte_storage.begin() + (ptrdiff_t)(index + 1) * sizeof(ArrayType)
            );
            return true;
        }

        bool Delete(const ArrayType& data)
        {
            int index = Find(data);
            return index >= 0 ? Delete(index) : false;
        }

        bool operator==(const ValueArray<ArrayType>& other) const
        {
            return byte_storage == other.byte_storage;
        }

        bool operator!=(const ValueArray<ArrayType>& other) const
        {
            return !(*this == other);
        }

        ValueArray() = default;
        ~ValueArray() = default;
        ValueArray(const ValueArray& rhs) : byte_storage(rhs.byte_storage) {}
        ValueArray& operator=(const ValueArray& rhs)
        {
            if (this != &rhs)
                byte_storage = rhs.byte_storage;
            return *this;
        }
    };

    template<typename T> T *GetObjectFromMap(const ValueArray<T *> &list,const int index)
    {
        T *obj;

        if(list.Get(index,obj))
            return(obj);

        return(nullptr);
    }

    using CharPointerList=hgl::ValueArray<const char *>;
}//namespace hgl
