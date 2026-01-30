#pragma once

#include<stdlib.h>
#include<initializer_list>
#include<type_traits>
#include<vector>
#include<algorithm>
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
            data_array.push_back(T());
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
            data_array.push_back(data);
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
            data_array.insert(data_array.end(), n, data);
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
            data_array.insert(data_array.end(), data, data+n);
            return(ec);
        }

                int  Add(const ValueArray<T> &l){return Add(l.GetData(),l.GetCount());}                            ///<增加一批数据

        virtual void Free(){data_array.clear(); data_array.shrink_to_fit();}                                                     ///<清除所有数据，并释放内存
        virtual void Clear(){data_array.clear();}                                                   ///<清除所有数据，但不清空缓冲区

        virtual int  Find(const T &data)const
        {
            auto it = std::find(data_array.begin(), data_array.end(), data);
            return (it == data_array.end()) ? -1 : (int)std::distance(data_array.begin(), it);
        }                        ///<查找指定数据的索引

        virtual bool Contains(const T &flag)const{return Find(flag)>=0;}                            ///<确认数据项是否存在

        virtual bool Delete(int start,int num=1)
        {
            if(start<0 || start+num>(int)data_array.size()) return false;
            data_array.erase(data_array.begin()+start, data_array.begin()+start+num);
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
                std::swap(data_array[a], data_array[b]);
        }                               ///<根据索引交换两个数据

        virtual bool Insert(int pos,const T &data)
        {
            if(pos<0 || pos>(int)data_array.size()) return false;
            data_array.insert(data_array.begin()+pos, data);
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
            data_array.insert(data_array.begin()+pos, data, data+number);
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

            std::vector<T> temp(data_array.begin()+old_pos, data_array.begin()+old_pos+move_count);
            data_array.erase(data_array.begin()+old_pos, data_array.begin()+old_pos+move_count);

            int actual_new_pos = new_pos;
            if(new_pos > old_pos)
                actual_new_pos -= move_count;

            data_array.insert(data_array.begin()+actual_new_pos, temp.begin(), temp.end());
        }

        virtual ValueArray<T>& operator = (const ValueArray<T> &da)
        {
            data_array = da.data_array;
            return *this;
        }

        virtual ValueArray<T>& operator = (const std::initializer_list<T> &l)
        {
            data_array.assign(l.begin(), l.end());
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
            data = data_array[index];
            return true;
        }    ///<取得指定索引处的数据

        virtual bool Set(int index,const T &data)
        {
            if(index<0 || index>=(int)data_array.size()) return false;
            data_array[index] = data;
            return true;
        }    ///<设置指定索引处的数据

        virtual bool GetFirst   (T &data)const{return Get(0, data);}                   ///<取第一个数据
        virtual bool GetLast    (T &data)const{return Get((int)data_array.size()-1, data);}        ///<取最后一个数据
    };//template <typename T> class ValueArray

    template<typename T> T *GetObjectFromMap(const ValueArray<T *> &list,const int index)
    {
        T *obj;

        if(list.Get(index,obj))
            return(obj);

        return(nullptr);
    }

    using CharPointerList=hgl::ValueArray<const char *>;
}//namespace hgl
