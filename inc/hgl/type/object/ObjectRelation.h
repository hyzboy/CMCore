
#pragma once

#include<hgl/type/object/ObjectBaseInfo.h>
#include<hgl/type/SortedSet.h>
#include<tsl/robin_set.h>

namespace hgl
{
    /**
    * 对象引用关系
    */
    struct ObjectRelation
    {
        tsl::robin_set<ObjectSimpleInfo> follow_me;            ///<引用自己的对象
        tsl::robin_set<ObjectSimpleInfo> me_follow;            ///<自己引用的对象
    };//struct ObjectRelation
}//namespace hgl
