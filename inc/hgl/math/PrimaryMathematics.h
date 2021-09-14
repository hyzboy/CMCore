#ifndef HGL_Primary_Mathematics_INCLUDE
#define HGL_Primary_Mathematics_INCLUDE
namespace hgl
{
    /**
     * 求一批数的合
     */
    template<typename T>
    const T sum(const T *data,const int count)
    {
        T result=0;

        for(int i=0;i<count;i++)
        {
            result+=*data;
            ++data;
        }

        return result;
    }

    /**
     * 求一批数的合
     */
    template<typename R,typename T>
    const R sum(const T *data,const int count)
    {
        R result=0;

        for(int i=0;i<count;i++)
        {
            result+=*data;
            ++data;
        }

        return result;
    }
}//namespace hgl
#endif//HGL_Primary_Mathematics_INCLUDE
