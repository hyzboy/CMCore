#pragma once

#include<hgl/type/DataType.h>
#include<hgl/type/List.h>
#include<hgl/util/sort/Sort.h>

namespace hgl
{
    template<typename P,typename T> struct GradientStop
    {
        P pos;                          ///<进度数据
        T data;                         ///<数据
    };

    /**
    * 数据渐变类模板
    */
    template<typename P,typename T> class Gradient
    {
        using GS=GradientStop<P,T>;

        List<GS> stop_list;

        bool dirty;

    protected:

        void Sort()
        {
            if(!dirty||stop_list.GetCount()<2)
                return;

            Comparator<GS> comp;

            hgl::Sort(stop_list,&comp);

            dirty=false;
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
            stop_list.Clear();
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

        virtual void Get(T &reuslt,const T &start,const T &end,const float &pos)
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
                                            \
                                            int Comparator<name##Stop>::compare(const name##Stop &a,const name##Stop &b)const   \
                                            {   \
                                                return a.pos-b.pos; \
                                            }   \
                                            \
                                            template<> void name::Get(T &result,const T &start,const T &end,const float &pos)

    /*
    HGL_GRADIENT_DEFINE(GradientColor3f,float,Lum)
    {
        result=start+(end-start)*pos;
    }

    HGL_GRADIENT_DEFINE(GradientColor3u8,uint,Color3b)
    {
        result.r=start.r+float(end.r-start.r)*pos;
        result.g=start.g+float(end.g-start.g)*pos;
        result.b=start.b+float(end.b-start.b)*pos;
    }
*/
}//namespace hgl
