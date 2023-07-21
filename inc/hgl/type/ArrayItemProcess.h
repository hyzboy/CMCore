#pragma once

namespace hgl
{
    /**
     * 阵列数据项处理回调
     */
    template<typename T> struct ArrayItemProcessCallback
    {
        virtual void Equal(T &dst,const T &src)=0;                  ///<将src赋值给dst

        virtual void Equal(T *dst,const T *src,const int count)=0;  ///<批量将src赋值给dst

        virtual int  Comp(const T &a,const T &b)=0;                 ///<数据比较，数值的话，如果纯整数比较，建议返回a-b
    };

    template<typename T> struct RawTypeArrayItemProcessCallback:public ArrayItemProcessCallback<T>
    {
        void Equal(T &dst,const T &src) override {dst=src;}

        void Equal(T *dst,const T *src,const int count) override {hgl_cpy(dst,src,count);}

        int Comp(const T &a,const T &b) override {return a-b;}
    };

    template<typename T> struct MemoryArrayItemProcessCallback:public ArrayItemProcessCallback<T>
    {
        void Equal(T &dst,const T &src) override {hgl_cpy<T>(dst,src);}

        void Equal(T *dst,const T *src,const int count) override {hgl_cpy(dst,src,count);}

        int Comp(const T &a,const T &b) override {return memcmp(a,b,sizeof(T));}
    };

    /**
     * 查找数据在无序阵列中的位置
     */
    template<typename T> static int FindDataPositionInArray(const T *data_list,const int count,const T &data)
    {
        if(!data_list)return(-1);
        if(count<=0)return(-2);

        const T *p=data_list;

        for(int i=0;i<count;i++)
        {
            //if(*p==data)
            if(!memcmp(p,&data,sizeof(T)))
                return i;

            ++p;
        }

        return -3;
    }

    template<typename T,typename O> static int FindDataPositionInArray(const T &data_list,const O &data)
    {
        return FindDataPositionInArray(data_list.GetData(),data_list.GetCount(),data);
    }

    /**
     * 查找数据在有序阵列中的位置
     */
    template<typename T> static int FindDataPositionInSortedArray(const T *data_array,const int count,const T &flag)
    {
        int left=0,right=count-1;                //使用left,right而不使用min,max是为了让代码能够更好的阅读。
        int mid;

        while(left<=right)
        {
            if(data_array[left ]==flag)return(left);
            if(data_array[right]==flag)return(right);

            mid=(right+left)>>1;

            if(data_array[mid]==flag)return(mid);

            if(data_array[mid]>flag)
            {
                left++;
                right=mid-1;
            }
            else
            {
                right--;
                left=mid+1;
            }
        }

        return(-1);
    }

    template<typename T,typename O> static int FindDataPositionInSortedArray(const T &data_array,const O &flag)
    {
        return FindDataPositionInSortedArray(data_array.GetData(),data_array.GetCount(),flag);
    }

    /**
    * 在已排序的阵列中查找数据的插入位置
    * @param pos 返回的插入位置
    * @param data_array 数据阵列
    * @param count 数据数量
    * @param flag 要查找的数据
    * @return 这个数据是否已经存在
    */
    template<typename T> static bool FindInsertPositionInSortedArray(int *pos,const T *data_array,const int count,const T &flag)
    {   
        int left=0,right=count-1;
        int mid;

        while(left<=right)
        {
            if(data_array[left]>flag)
            {
                *pos=left;
                return(false);
            }
            else
            if(data_array[left]==flag)
            {
                *pos=left;
                return(true);
            }

            if(data_array[right]<flag)
            {
                *pos=right+1;
                return(false);
            }
            else
            if(data_array[right]==flag)
            {
                *pos=right;
                return(true);
            }

            mid=(right+left)>>1;

            if(data_array[mid]==flag)
            {
                *pos=mid;
                return(true);
            }

            if(data_array[mid]>flag)
            {
                if(data_array[mid-1]<flag)
                {
                    *pos=mid;
                    return(false);
                }
                else
                if(data_array[mid-1]==flag)
                {
                    *pos=mid-1;
                    return(true);
                }

                ++left;
                right=mid-1;
            }
            else
            {
                if(data_array[mid+1]>flag)
                {
                    *pos=mid+1;
                    return(false);
                }
                else
                if(data_array[mid+1]==flag)
                {
                    *pos=mid+1;
                    return(true);
                }

                --right;
                left=mid+1;
            }
        }

        return(false);
    }

    template<typename T,typename O> static bool FindInsertPositionInSortedArray(int *pos,const T &data_array,const O &flag)
    {
        return FindInsertPositionInSortedArray(pos,data_array.GetData(),data_array.GetCount(),flag);
    }
}//namespace hgl
