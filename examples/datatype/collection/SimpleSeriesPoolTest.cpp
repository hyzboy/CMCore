#include<iostream>
#include<time.h>
#include<hgl/type/DataType.h>
#include<hgl/type/SeriesPool.h>
#include<hgl/type/ValueArray.h>
#include<random>

using namespace std;
using namespace hgl;

constexpr int POOL_MAX_COUNT=10;

template<typename T> void out_series_pool(const SeriesPool<T> &sp)
{
    const int free_count=sp.GetFreeCount();

    cout<<"Series Pool("<<free_count<<"): ";

    const T *p=sp.GetRawData();

    for(int i=0;i<free_count;i++)
    {
        if(i>0)cout<<",";
        cout<<uint(*p++);
    }

    cout<<endl;
}

template<typename T> void out_list(const ValueArray<T> &l)
{
    cout<<"List("<<uint(l.GetCount())<<"): ";

    const T *v=l.GetData();

    for(int i=0;i<l.GetCount();i++)
    {
        if(i>0)cout<<",";
        cout<<uint(*v++);
    }

    cout<<endl;
}

int os_main(int,os_char **)
{
    SeriesU8 series_pool(POOL_MAX_COUNT);       //序号池
    ValueArray<uint8> tmp_list;                       //临时序号列表

    tmp_list.Reserve(POOL_MAX_COUNT);          //预分配空间(注：没有数据，只是预分配空间，减少未来内存分配次数而己)

    cout<<"Series pool MaxCount="<<uint(series_pool.GetMaxCount())<<endl;

    random_device rd;
    mt19937 rand_engine(rd());
    uniform_int_distribution<uint>  uid_op_count(POOL_MAX_COUNT,POOL_MAX_COUNT*2);

    bernoulli_distribution          uid_op(0.5);            //使用伯努利分布随机数产生操作类型，50%的概率是Acquire，50%的概率是Release
    uniform_int_distribution<>      uid_count(1,4);         //使用均匀分布随机数产生操作数量，1~4个

    const uint op_count=uid_op_count(rand_engine);          //整体执行次数

    for (uint i = 0; i < op_count; i++)
    {
        bool  op     = uid_op(rand_engine);                 //这一次的操作类型
        uint8 count  = uid_count(rand_engine);              //这一次的操作数量

        if (op)
        {
            count=hgl_min(count,series_pool.GetFreeCount());

            if(count<=0)continue;

            cout << "Acquire ";

            for (uint j = 0; j < count; j++)
            {
                uint8 index;

                if (series_pool.Acquire(&index))                     //取得一个序号
                {
                    cout<< (int)index <<" ";

                    tmp_list.Add(index);
                }
                else
                {
                    cout << "Acquire failed!" << endl;
                    return -1;
                }
            }
        }
        else
        {
            count=hgl_min(count,uint8(tmp_list.GetCount()));

            if(count<=0)continue;

            cout << "Release ";

            for (uint j = 0; j < count; j++)
            {
                uint8 pos = uid_op_count(rand_engine)%tmp_list.GetCount();     //取得一个随机位置
                uint8 index;

                tmp_list.Get(pos,index);            //取得这个位置的数据
                tmp_list.Delete(pos);               //删除这个位置的数据

                if (series_pool.Release(index))              //释放一个序号
                {
                    cout << (int)index << " ";
                }
                else
                {
                    cout << "Release failed!" << endl;
                    return -2;
                }
            }
        }

        cout<<endl;

        out_series_pool(series_pool);
        out_list(tmp_list);

        cout<<endl;
    }

    return 0;
}
