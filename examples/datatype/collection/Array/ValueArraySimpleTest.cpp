#include<hgl/type/ValueArray.h>
#include<hgl/type/IndexedValueArray.h>
#include<hgl/time/Time.h>
#include<random>
#include<iostream>
#include<iomanip>

using namespace hgl;

constexpr const uint MAX_LIST_COUNT=100000;
constexpr const uint DELETE_COUNT=MAX_LIST_COUNT/10;

template<typename T>
void out_list(const T &list)
{
    std::cout<<"List("<<list.GetCount()<<"):[";

    for ( int i=0;i<list.GetCount();i++ )
    {
        if(i>0)
            std::cout<<",";

        std::cout<<list.At(i);
    }

    std::cout<<"]"<<std::endl;
}

inline void out_time(const char *front,const double value)
{
    std::cout<<front<< std::fixed << std::setprecision(8) << std::setw(12) << std::setfill('0')<<value;
}

inline void out_faster(const double ar,const double ir)
{
    std::cout<<"\tFaster: ";

    if(ar<ir)
        std::cout<<"ValueArray\t"<<((ir-ar)/ir)*100.0<<"%"<<std::endl;
    else
        std::cout<<"IndexedValueArray\t"<<((ar-ir)/ar)*100.0<<"%"<<std::endl;
}

template<typename T>
void TestListType(bool pre_alloc)
{
    double start,end;

    std::random_device device;
    std::mt19937 generator(device());

    ValueArray<T> al;
    IndexedValueArray<T> il;

    double ar,ir;

    if(pre_alloc)
    {
        al.Reserve(MAX_LIST_COUNT);
        il.Reserve(MAX_LIST_COUNT);
    }

    AutoDeleteArray<T> origin_data(MAX_LIST_COUNT);
    AutoDeleteArray<uint> delete_index(DELETE_COUNT);

    std::cout<<"Type: "<<typeid(T).name()<<"\t";
    std::cout<<"Size: "<<sizeof(T)<<" bytes"<<std::endl;

    {
        uint8 *p=(uint8 *)origin_data.data();
        for(size_t i=0;i<MAX_LIST_COUNT;i++)
        {
            *p=generator()&0xFF;
            ++p;
        }
    }

    for(uint i=0;i<DELETE_COUNT;i++)
        delete_index[i]=generator()%(MAX_LIST_COUNT-1);

    std::cout<<"\tAdd "<<std::setw(8)<<MAX_LIST_COUNT<<" data ";
    {
        start=GetTimeSec();
        for(uint i=0;i<MAX_LIST_COUNT;i++)
        {
            al.Add(origin_data[i]);
        }
        end=GetTimeSec();

        ar=end-start;
        out_time("\tAL: ",ar);
    }

    {
        start=GetTimeSec();
        for(uint i=0;i<MAX_LIST_COUNT;i++)
            il.Add(origin_data[i]);
        end=GetTimeSec();

        ir=end-start;
        out_time("\tIL: ",ir);
    }

    out_faster(ar,ir);
    std::cout<<"\tDel "<<std::setw(8)<<(DELETE_COUNT)<<" data";

    {
        start=GetTimeSec();
        for(uint i=0;i<DELETE_COUNT;i++)
            al.Delete(delete_index[i]);
        end=GetTimeSec();

        ar=end-start;
        out_time("\tAL: ",ar);
    }

    {
        start=GetTimeSec();
        for(uint i=0;i<DELETE_COUNT;i++)
            il.Delete(delete_index[i]);
        end=GetTimeSec();

        ir=end-start;
        out_time("\tIL: ",ir);
    }

    out_faster(ar,ir);
}

namespace
{
    using uint128=uint64[2];
    using uint256=uint64[4];
    using uint512=uint64[8];
    using uint1k=uint64[16];
    using uint2k=uint64[32];
    using uint4k=uint64[64];
    using uint8k=uint64[128];
    using uint16k=uint64[256];
}

void Test(bool pre_alloc)
{
    TestListType<uint32>(pre_alloc);
    TestListType<uint64>(pre_alloc);
    TestListType<uint128>(pre_alloc);
    TestListType<uint256>(pre_alloc);
    TestListType<uint512>(pre_alloc);
    TestListType<uint1k>(pre_alloc);
    TestListType<uint2k>(pre_alloc);
    TestListType<uint4k>(pre_alloc);
    TestListType<uint8k>(pre_alloc);
    TestListType<uint16k>(pre_alloc);
}

int main(int,char **argv)
{
    std::cout<<"No pre-alloc"<<std::endl;
    Test(false);
    std::cout<<"pre-alloc"<<std::endl;
    Test(true);
    return 0;
}
