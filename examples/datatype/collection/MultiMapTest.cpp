//多重map测试

//多重map指的是一套数据，含有多个关键字，每个关键字都可以用来查找数据，这样的数据结构称为多重map。

#include"UserInfo.h"
#include<hgl/type/DataArray.h>

using namespace hgl;

template<typename T> class IndexAccess
{
    DataArray<T> *data_pool;

    DataArray<int> data_index;

public:

    const int Comp(const T &a,const T &b)const;


};

template<typename T> class MultiIndexMap
{
    DataArray<T> data_pool;

public:


};