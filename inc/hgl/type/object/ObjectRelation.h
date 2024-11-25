
#pragma once

#include<hgl/type/object/ObjectBaseInfo.h>
#include<hgl/type/List.h>
//#include<tsl/robin_map.h>

namespace hgl
{
    struct ObjectReferringRecord
    {
        ObjectSimpleInfo osi;

        size_t refer_serial;            ///<引用序号

        SourceCodeLocation scl;
    };

    /**
    * 对象引用关系
    */
    struct ObjectRelation
    {
        List<ObjectReferringRecord> referring_me; ///<引用自己的对象
        List<ObjectReferringRecord> me_referring; ///<自己引用的对象

        //tsl::robin_map< size_t, /*refer_serial*/
        //                ObjectReferringRecord *> referring_me_map; ///<引用自己的对象
    };//struct ObjectRelation
}//namespace hgl
