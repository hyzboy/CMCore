#pragma once

#include<hgl/type/ActiveMemoryBlockManager.h>

namespace hgl
{
    /**
    * 活动数据管理模板类<br>
    * 通过ActiveIDManager管理活跃的数据ID，在要使用时通过ID来获取或写入数据。
    */
    template<typename T> class ActiveDataManager
    {
        ActiveIDManager aim;

        DataArray<T> data_array;

    public:

        ActiveDataManager(){}
        virtual ~ActiveDataManager()=default;

        void Alloc(int c)
        {
            aim.Alloc(c);
            data_array.Alloc(c);
        }

        int GetActiveCount  ()const{return aim.GetActiveCount();}
        int GetIdleCount    ()const{return aim.GetIdleCount();}
        int GetTotalCount   ()const{return aim.GetTotalCount();}
        int GetHistoryMaxId ()const{return aim.GetHistoryMaxId();}

        const DataArray<int> &GetActiveArray()const{return aim.GetActiveArray();}
        const DataArray<int> &GetIdleArray()const{return aim.GetIdleArray();}

    public:

        bool WriteData(const T &d,const int id)
        {
            return data_array.WriteAt(d,id);
        }

        int WriteDataArray(const T **da,const int *idp,const int count)
        {
            int result=0;

            for(int i=0;i<count;i++)
            {
                if(data_array.WriteAt(**da,*idp))
                    ++result;

                ++da;
                ++idp;
            }

            return result;
        }

        int WriteDataArray(T *da,const int *idp,const int count)const
        {
            int result=0;

            for(int i=0;i<count;i++)
            {
                if(data_array.WriteAt(*da,*idp))
                    ++result;

                ++da;
                ++idp;
            }

            return result;
        }

        bool GetData(T &d,const int id)const
        {
            return data_array.ReadAt(d,id);
        }

        bool GetData(T **da,const int *idp,const int count)const
        {
            int result=0;

            for(int i=0;i<count;i++)
            {
                *da=data_array.At(*idp);

                if(*da)++result;

                ++da;
                ++idp;
            }

            return result;
        }

        bool GetData(T *da,const int *idp,const int count)const
        {
            int result=0;

            for(int i=0;i<count;i++)
            {
                if(data_array.ReadAt(*da,*idp))
                    ++result;

                ++da;
                ++idp;
            }

            return result;
        }

    public:

        void CreateActive(int *id,const int count=1)
        {
            aim.CreateActive(id,count);

            data_array.AddCount(count);
        }

        void CreateIdle(int *idp=nullptr,const int count=1)
        {
            aim.CreateIdle(idp,count);

            data_array.AddCount(count);
        }

        void    CreateIdle      (const int count=1)         {CreateIdle(nullptr,count);}
        bool    Get             (int *id,const int count=1) {return aim.Get(id,count);}
        int     Release         (int *id,const int count=1) {return aim.Release(id,count);}
        int     ReleaseAllActive()                          {return aim.ReleaseAllActive();}
    };//template<typename T> class ActiveDataManager
}//namespace hgl
