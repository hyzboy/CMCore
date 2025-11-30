#pragma once

#include<hgl/TypeFunc.h>
namespace hgl
{
    /**
     * bool型矢量
     */
    template<size_t N> class bool_vec
    {
    public:

        bool data[N];

    public:

        bool_vec()                      {hgl_zero(data);}
        bool_vec(const bool_vec<N> &b)  {hgl_cpy(*this,b);}
        bool_vec(const bool *ba)        {hgl_cpy<bool>(data,ba,N);}
        virtual ~bool_vec()=default;

        bool &operator[](const int index)
        {
            return data[index];
        }

        const bool_vec<N> &operator = (const bool_vec<N> &b)
        {
            hgl_cpy(this->data,b.data,N);
            return *this;
        }

        bool all()const
        {
            bool result = true;

            for (int i = 0; i < N; ++i)
                result &= data[i];

            return result;

        }

        bool any()const
        {
            bool result = false;

            for (int i = 0; i < N; ++i)
                result |= data[i];

            return result;
        }

        const bool_vec<N> not()const
        {
            bool_vec<N> result;

            for (int i = 0; i < N; ++i)
                result.data[i]=!data[i];

            return result;
        }
    };//template<size_t N> class bool_vec

    #define BVEC_BEGIN(N)   class bvec##N:public bool_vec<N> \
                            {   \
                            public: \
                                bvec##N(){} \
                                bvec##N(const bvec##N &b){hgl_cpy(this->data,b.data,N);}  \
                                bvec##N(const bool *b){hgl_cpy(this->data,b,N);}    \
                                ~bvec##N()=default;

    #define BVEC_END        };

    BVEC_BEGIN(2)
        bvec2(const bool &x,const bool &y){data[0]=x;data[1]=y;}
    BVEC_END

    BVEC_BEGIN(3)
        bvec3(const bool &x,const bool &y,const bool &z){data[0]=x;data[1]=y;data[2]=z;}
    BVEC_END

    BVEC_BEGIN(4)
        bvec4(const bool &x,const bool &y,const bool &z,const bool &w){data[0]=x;data[1]=y;data[2]=z;data[3]=w;}
    BVEC_END

    #undef BVEC_BEGIN
    #undef BVEC_END
}//namespace hgl
