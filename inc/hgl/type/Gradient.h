#pragma once

#include<hgl/type/DataType.h>
#include<hgl/type/ValueArray.h>
#include<hgl/util/sort/Sort.h>

namespace hgl
{
    template<typename P,typename T> struct GradientStop
    {
        P pos;                          ///<进度数据
        T data;                         ///<数据

        // C++20 三路比较
        std::strong_ordering operator<=>(const GradientStop &other) const
        {
            return pos <=> other.pos;  // 只比较 pos 字段
        }

        bool operator==(const GradientStop &other) const
        {
            return pos == other.pos;
        }
    };

    /**
    * 数据渐变类模板
    */
    template<typename P,typename T> class Gradient
    {
        using GS=GradientStop<P,T>;

        ValueArray<GS> stop_list;

        bool dirty;

    protected:

        void Sort()
        {
            if(!dirty || stop_list.GetCount() < 2)
                return;

            ::Sort(stop_list.GetArray());  // 直接使用 operator<=>
            dirty = false;
        }

    public:

        Gradient()
        {
            dirty=true;
        }

        /**
        * 添加一个渐变点
        */
        void Add(const P &pos,const T &data)
        {
            GS gs;

            gs.pos=pos;
            gs.data=data;

            stop_list.Add(gs);
            dirty=true;
        }

        void Clear()
        {
            stop_list.Free();
            dirty=true;
        }

        /**
        * 取得最低渐变进度
        */
        const bool GetLowestPosition(P &pos)const
        {
            GS gs;

            if(stop_list.GetFirst(gs))
            {
                pos=gs.pos;
                return(true);
            }

            return false;
        }

        /**
        * 取得最高渐变进度
        */
        const bool GetHighestPosition(P &pos)const
        {
            GS gs;

            if(stop_list.GetLast(gs))
            {
                pos=gs.pos;
                return(true);
            }

            return false;
        }

        /**
        * 取得最低渐变结果  
        */
        const bool GetLowestData(T &data)const
        {
            GS gs;

            if(stop_list.GetFirst(gs))
            {
                data=gs.data;
                return(true);
            }

            return false;
        }

        /**
        * 取得最高渐变结果
        */
        const bool GetHighestData(T &data)const
        {
            GS gs;

            if(stop_list.GetLast(gs))
            {
                data=gs.data;
                return(true);
            }

            return false;
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
            const uint count=stop_list.GetCount();

            if(count<=0)
                return;

            Sort();

            GS *cp=stop_list.GetData();
            GS *pp;

            if(count<2||pos<=cp->pos)
            {
                result=cp->data;
                return;
            }

            if(pos>=cp[count-1].pos)
            {
                result=cp[count-1].data;
                return;
            }

            pp=cp;
            ++cp;
            for(uint i=1;i<count;i++)
            {
                if(pos<=cp->pos)
                {
                    Get(result,pp->data,cp->data,float(pos-pp->pos)/float(cp->pos-pp->pos));
                    return;
                }

                ++pp;
                ++cp;
            }
        }
    };//class Gradient

    #define HGL_GRADIENT_DEFINE(name,P,T)   using name=Gradient<P,T>;  \
                                            using name##Stop=GradientStop<P,T>; \
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
