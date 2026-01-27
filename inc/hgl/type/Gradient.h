#pragma once

#include<hgl/type/DataType.h>
#include<hgl/type/ValueKVMap.h>

namespace hgl
{
    /**
    * 数据渐变类模板
    */
    template<typename P,typename T> class Gradient
    {
        using KVData=KeyValue<P,T>;

        ValueKVMap<P,T> stop_map;

    public:

        Gradient()=default;

        /**
        * 添加一个渐变点
        */
        void Add(const P &pos,const T &data)
        {
            stop_map.Add(pos,data);
        }

        void Clear()
        {
            stop_map.Free();
        }

        /**
        * 取得最低渐变进度
        */
        const bool GetLowestPosition(P &pos)const
        {
            if(stop_map.IsEmpty())
                return false;

            pos=stop_map.begin()->key;
            return true;
        }

        /**
        * 取得最高渐变进度
        */
        const bool GetHighestPosition(P &pos)const
        {
            if(stop_map.IsEmpty())
                return false;

            const KVData *last=stop_map.end()-1;
            pos=last->key;
            return true;
        }

        /**
        * 取得最低渐变结果
        */
        const bool GetLowestData(T &data)const
        {
            if(stop_map.IsEmpty())
                return false;

            data=stop_map.begin()->value;
            return true;
        }

        /**
        * 取得最高渐变结果
        */
        const bool GetHighestData(T &data)const
        {
            if(stop_map.IsEmpty())
                return false;

            const KVData *last=stop_map.end()-1;
            data=last->value;
            return true;
        }

        virtual void Get(T &result,const T &start,const T &end,const float &pos)
        {
            result=start+(end-start)*pos;
        }

        /**
        * 取得指定进度下的渐变结果
        * @param pos 进度
        * @param result 结果
        */
        const void Get(T &result,const P &pos)
        {
            const uint count=stop_map.GetCount();

            if(count<=0)
                return;

            const KVData *cp=stop_map.begin();

            if(count<2||pos<=cp->key)
            {
                result=cp->value;
                return;
            }

            const KVData *last=stop_map.end()-1;
            if(pos>=last->key)
            {
                result=last->value;
                return;
            }

            const KVData *pp=cp;
            ++cp;
            for(uint i=1;i<count;i++)
            {
                if(pos<=cp->key)
                {
                    Get(result,pp->value,cp->value,float(pos-pp->key)/float(cp->key-pp->key));
                    return;
                }

                ++pp;
                ++cp;
            }
        }
    };//class Gradient

    #define HGL_GRADIENT_DEFINE(name,P,T)   using name=Gradient<P,T>;  \
                                            template<> void name::Get(T &result,const T &start,const T &end,const float &pos)

    /*
    // 使用示例：
    HGL_GRADIENT_DEFINE(GradientColor3f,float,float)
    {
        result=start+(end-start)*pos;
    }

    HGL_GRADIENT_DEFINE(GradientColor3u8,uint8_t,uint32_t)
    {
        // 自定义插值逻辑
        result=start+(end-start)*pos;
    }
    */
}//namespace hgl
