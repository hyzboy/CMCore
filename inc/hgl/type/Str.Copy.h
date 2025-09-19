#pragma once
#include <hgl/type/Str.Length.h>
#include <hgl/type/Str.Case.h>
namespace hgl
{
    /**
     * @brief CN: 复制字符串，并指定最大长度。
     * @brief EN: Copy a string with a maximum destination size.
     *
     * @tparam T 字符类型. EN: character type.
     * @param[out] dst CN: 目标字符串缓冲区. EN: destination buffer.
     * @param[in] count CN: 目标字符串最大可用字符数（含终止符位置）. EN: destination buffer capacity (including space for terminator).
     * @param[in] src CN: 源字符串. EN: source string.
     * @return int CN: 返回写入到目标的字符数（不含终止符），若出错返回负数. EN: number of characters written (excluding terminator), negative on error.
     */
    template<typename T>
    inline const int strcpy(T *dst,int count,const T *src)
    {
        if(!dst)return(-1);

        if(!src||!(*src)||count<=0)
        {
            if(count>0)
                *dst=0;
            return(0);
        }

        if(dst==src)
            return(-1);

        T *start=dst;

        // 保留一个位置给终止符
        while(*src && count>1)
        {
            *dst=*src;
            ++dst;
            ++src;
            --count;
        }

        if(count>0)
            *dst=0;

        return int(dst-start);
    }

    /**
     * @brief CN: 复制字符串到目标，并限定源和目标的最大长度。
     * @brief EN: Copy a string with both destination capacity and source max characters limit.
     *
     * @tparam T 字符类型. EN: character type.
     * @param[out] dst CN: 目标字符串缓冲区. EN: destination buffer.
     * @param[in] dst_count CN: 目标缓冲区容量（含终止符位置）. EN: destination buffer capacity (including terminator).
     * @param[in] src CN: 源字符串. EN: source string.
     * @param[in] count CN: 最多从 src 复制的字符数. EN: maximum number of characters to copy from src.
     * @return int CN: 写入到目标的字符数（不含终止符），若出错返回负数. EN: number of characters written (excluding terminator), negative on error.
     */
    template<typename T>
    inline const int strcpy(T *dst,int dst_count,const T *src,int count)
    {
        if(!dst)return(-1);

        if(!src||!(*src)||dst_count<=0||count<=0)
        {
            //没什么好复制的
            *dst=0;
            return(0);
        }

        if(dst==src)
            return(-1);

        T *start=dst;

        // 至少保留1个结尾0
        while(*src&&dst_count>1&&count)
        {
            *dst=*src;
            ++dst;
            ++src;
            --dst_count;
            --count;
        }

        if(dst_count>0)
            *dst=0;

        return(dst-start);
    }

    /**
     * @brief CN: 在字符串尾部追加单个字符（带最大长度限制）。
     * @brief EN: Append a single character to a string with a maximum capacity.
     *
     * @tparam T 字符类型. EN: character type.
     * @param[in,out] dst CN: 目标字符串缓冲区（将被修改）. EN: destination buffer to append to.
     * @param[in] max_count CN: 目标缓冲区的总容量（含终止符位置）. EN: total buffer capacity (including terminator).
     * @param[in] ch CN: 要追加的字符. EN: character to append.
     */
    template<typename T>
    inline void strcat(T *dst,int max_count,const T ch)
    {
        if(!dst||!ch)return;

        // 移动到结尾，max_count 为总容量，逐步减少
        while(*dst && max_count>0)
        {
            ++dst;
            --max_count;
        }

        // 需要至少两个位置：一个字符+终止符
        if(max_count>1)
        {
            *dst++=ch;
            *dst=0;
        }
    }

    /**
     * @brief CN: 在字符串尾部追加另一个字符串（带最大长度与源长度限制）。
     * @brief EN: Append a string to destination with destination capacity and source max length.
     *
     * @tparam T 字符类型. EN: character type.
     * @param[in,out] dst CN: 目标字符串缓冲区（将被修改）. EN: destination buffer to append to.
     * @param[in] max_count CN: 目标缓冲区总容量（含终止符位置）. EN: total buffer capacity (including terminator).
     * @param[in] src CN: 要追加的源字符串. EN: source string to append.
     * @param[in] count CN: 最多从 src 复制的字符数. EN: maximum number of characters to copy from src.
     * @return int CN: 返回追加操作前的起始偏移（即原始字符串长度），若出错返回负值. EN: return value indicates the number of characters traversed (original length), negative on error.
     */
    template<typename T>
    inline const int strcat(T *dst,int max_count,const T *src,int count)
    {
        if(!dst||!src||!(*src)||count<=0)return(-1);

        T *start=dst;

        // 前移到末尾
        while(*dst && max_count>0)
        {
            ++dst;
            --max_count;
        }

        // 复制，保留终止符
        while(*src && max_count>1 && count>0)
        {
            *dst=*src;
            ++dst;
            ++src;
            --count;
            --max_count;
        }

        if(max_count>0)
            *dst=0;

        return int(dst-start);
    }

    /**
     * @brief CN: 在堆上创建并返回一个字符串的副本。
     * @brief EN: Create and return a heap-allocated copy of a string.
     *
     * @tparam T 字符类型. EN: character type.
     * @param[in] str CN: 源字符串指针. EN: source string.
     * @param[in] size CN: 指定要复制的长度，-1 表示自动计算（以 null 结尾为准）. EN: length to copy, -1 to detect automatically.
     * @return T* CN: 返回新分配的字符串指针，失败返回 nullptr. EN: newly allocated copy or nullptr on failure.
     */
    template<typename T>
    inline T *create_copy(const T *str,int size=-1)
    {
        if(!str)return(nullptr);

        T *copy;

        if(size==-1)size=hgl::strlen(str);

        if(size==0)return(nullptr);

        ++size;

        copy=new T[size];
        hgl::strcpy(copy,size,str);

        return(copy);
    }
}//namespace hgl
