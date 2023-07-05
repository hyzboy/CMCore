#pragma once

#include<hgl/type/DataType.h>
#include<hgl/type/List.h>
#include<hgl/util/sort/Sort.h>

namespace hgl
{
    template<typename P,typename T> struct GradientStop
    {
        P pos;
        T data;
    };

    /**
    * Ω•±‰¿‡
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

        const bool GetLowest(P &pos)const
        {
            GS gs;

            if(stop_list.First(gs))
            {
                pos=gs.pos;
                return(true);
            }

            return false;
        }

        const bool GetHighest(P &pos)const
        {
            GS gs;

            if(stop_list.Last(gs))
            {
                pos=gs.pos;
                return(true);
            }

            return false;
        }

        virtual void Get(T &reuslt,const T &start,const T &end,const float &pos)
        {
            result=start+(end-start)*pos;
        }

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
}//namespace hgl