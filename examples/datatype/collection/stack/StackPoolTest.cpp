#include<hgl/type/FixedValuePool.h>
#include"UserInfo.h"        //测试用结构体
#include<iostream>
#include<random>

using namespace hgl;
using namespace std;

template<typename T> void out_series_pool(const SeriesPool<T> *sp)
{
    const int free_count=sp->GetFreeCount();

    cout<<"Series Pool("<<free_count<<"): ";

    const T *p=sp->GetRawData();

    for(int i=0;i<free_count;i++)
    {
        if(i>0)cout<<",";
        cout<<uint(*p++);
    }

    cout<<endl;
}

#define USE_RANDOM_SERIES_POOL  true        //使用随机序号池

int os_main(int,os_char **)
{
    FixedValuePool<UserInfo> dsp_userinfo;

    dsp_userinfo.Init(user_info_array_count);    //用户信息数据池

    #ifdef USE_RANDOM_SERIES_POOL
    {
        auto *series=dsp_userinfo.GetSeries();

        series->InitRandomSeries();

        out_series_pool(series);
    }
    #endif//USE_RANDOM_SERIES_POOL

    {
        UserInfo *tp;

        for(const UserInfo &sp:user_info_array)
        {
            tp=dsp_userinfo.Acquire();

            if(!tp)
            {
                cout<<"Acquire UserInfo Error!"<<endl;
                break;
            }

            *tp=sp;
        }

        out_userinfo(dsp_userinfo.GetRawData(),user_info_array_count);
    }

    return 0;
}
