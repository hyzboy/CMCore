#include<hgl/type/ActiveMemoryBlockManager.h>

namespace hgl
{
    void ActiveMemoryBlockManager::ReallocDataBlock()
    {
        data_mb->Alloc(aim.GetHistoryMaxId()*unit_size);
    }

    ActiveMemoryBlockManager::ActiveMemoryBlockManager(const uint us,AbstractMemoryAllocator *ma)
    {
        unit_size=us;

        data_mb=new MemoryBlock(ma);
    }

    ActiveMemoryBlockManager::~ActiveMemoryBlockManager()
    {
        delete data_mb;
    }

    bool ActiveMemoryBlockManager::SetUnitSize(const uint us)
    {
        if(unit_size>0)             //unit_size已经存在
            return(unit_size==us);

        unit_size=us;
        return(true);
    }

    void ActiveMemoryBlockManager::Alloc(int c)
    {
        aim.Alloc(c);
        data_mb->Alloc(c*unit_size);
    }

    bool ActiveMemoryBlockManager::WriteData(void *d,const int id)
    {
        if(!d||id<0||id>=aim.GetHistoryMaxId())
            return(false);

        return data_mb->Write(unit_size*id,d,unit_size);
    }

    int ActiveMemoryBlockManager::WriteDataArray(void **da,const int *idp,const int count)
    {
        if(!da||!idp||count<=0)return(0);

        uint8 *sp=(uint8 *)(data_mb->Get());
        int result=0;

        for(int i=0;i<count;i++)
        {
            if(*idp>=0&&*idp<aim.GetHistoryMaxId())
            {
                memcpy(sp+(*idp)*unit_size,*da,unit_size);
                ++result;
            }

            ++da;
            ++idp;
        }

        return(result);
    }

    int ActiveMemoryBlockManager::WriteDataArray(void *da,const int *idp,const int count)const
    {
        if(!da||!idp||count<=0)return(0);

        uint8 *sp=(uint8 *)da;
        uint8 *tp=(uint8 *)(data_mb->Get());
        int result=0;

        for(int i=0;i<count;i++)
        {
            if(*idp>=0&&*idp<aim.GetHistoryMaxId())
            {
                memcpy(tp+(*idp)*unit_size,sp,unit_size);
                ++result;
            }

            sp+=unit_size;
            ++idp;
        }

        return(result);
    }
    
    bool ActiveMemoryBlockManager::GetData(void *da,const int id)const
    {
        if(!id||id<0||id>=aim.GetHistoryMaxId())
            return(false);

        memcpy(da,(uint8 *)(data_mb->Get())+id*unit_size,unit_size);

        return(true);
    }

    /**
        * 根据ID获取数据
        */
    bool ActiveMemoryBlockManager::GetData(void **da,const int *idp,const int count)const
    {
        if(!da||!idp||count<=0)return(false);

        uint8 *sp=(uint8 *)(data_mb->Get());

        for(int i=0;i<count;i++)
        {
            if(*idp<0||*idp>=aim.GetHistoryMaxId())
                return(false);

            *da=sp+(*idp)*unit_size;

            ++da;
            ++idp;
        }

        return(true);
    }

    /**
    * 根据ID获取数据，并整齐排列到一起
    */
    bool ActiveMemoryBlockManager::GetData(void *da,const int *idp,const int count)const
    {
        if(!da||!idp||count<=0)return(false);

        uint8 *sp=(uint8 *)(data_mb->Get());
        uint8 *tp=(uint8 *)da;

        for(int i=0;i<count;i++)
        {
            if(*idp<0||*idp>=aim.GetHistoryMaxId())
                return(false);

            memcpy(tp,sp+(*idp)*unit_size,unit_size);

            tp+=unit_size;
            ++idp;
        }

        return(true);
    }

    int ActiveMemoryBlockManager::CreateActive(int *da,const int count)
    {
        if(!da||count<=0)return(-1);

        int result=aim.CreateActive(da,count);

        if(result!=count)
            return(false);

        ReallocDataBlock();
        return(result);
    }

    int ActiveMemoryBlockManager::CreateIdle(int *da,const int count)
    {
        if(count<=0)return 0;

        int result=aim.CreateIdle(da,count);

        if(result!=count)
            return(false);

        ReallocDataBlock();
        return(result);
    }

    bool ActiveMemoryBlockManager::Get(int *da,const int count)
    {
        if(!da||count<=0)return(false);

        if(aim.Get(da,count)!=count)
            return(false);

        ReallocDataBlock();
        return(true);
    }

    int ActiveMemoryBlockManager::Release(int *id,const int count)
    {
        return aim.Release(id,count);
    }
    
    int ActiveMemoryBlockManager::ReleaseAllActive()
    {
        return aim.ReleaseAllActive();
    }
}//namespace hgl
