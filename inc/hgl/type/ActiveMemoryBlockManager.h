#pragma once

#include<hgl/type/ActiveIDManager.h>
#include<hgl/type/MemoryBlock.h>

namespace hgl
{
    /**
    * 活跃内存块管理类<br>
    * 通过ActiveIDManager管理活跃的数据ID，在要使用时通过ID来获取或写入数据。
    */
    class ActiveMemoryBlockManager
    {
        uint unit_size;                     ///<单个数据大小
        MemoryBlock *data_mb;               ///<数据内存块

        ActiveIDManager aim;                ///<活跃ID管理器

    private:

        void ReallocDataBlock();

    public:

        ActiveMemoryBlockManager(const uint us,AbstractMemoryAllocator *ma=new MemoryAllocator);
        virtual ~ActiveMemoryBlockManager();

        bool SetUnitSize(const uint us);
        void Reserve(int c);

        int GetActiveCount  ()const{return aim.GetActiveCount();}
        int GetIdleCount    ()const{return aim.GetIdleCount();}
        int GetTotalCount   ()const{return aim.GetTotalCount();}
        int GetHistoryMaxId ()const{return aim.GetHistoryMaxId();}

        const DataArray<int> &GetActiveArray()const{return aim.GetActiveArray();}
        const DataArray<int> &GetIdleArray  ()const{return aim.GetIdleArray();}

        bool IsActive(const int id)const{return aim.IsActive(id);}  ///<确认指定ID是否处于活跃状态
        bool IsIdle  (const int id)const{return aim.IsIdle(id);}    ///<确认指定ID是否处于闲置状态
        bool IsValid (const int id)const{return aim.IsValid(id);}   ///<确认指定ID是否曾被创建过

    public:

        bool WriteData      (void *d,const int id);
        int  WriteDataArray (void **da,const int *idp,const int count);
        int  WriteDataArray (void *da,const int *idp,const int count)const;

        void *GetData(const int id)const;
    
        bool GetData(void *,const int id)const;
        bool GetData(void **da,const int *idp,const int count)const;                                ///<根据ID获取数据
        bool GetData(void *da,const int *idp,const int count)const;                                 ///<根据ID获取数据，并整齐排列到一起

    public:

        int CreateActive(int *da,const int count=1);
        int CreateIdle(int *da,const int count=1);

        bool Get(int *da,const int count=1);
        bool GetOrCreate(int *da,const int count=1);

        int Release(int *id,const int count=1);    
        int ReleaseAllActive();
    };//class ActiveMemoryBlockManager
}//namespace hgl
