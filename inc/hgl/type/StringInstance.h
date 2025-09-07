#pragma once

#include<hgl/type/DataType.h>
#include<string.h>
#include<string>

namespace hgl
{
    /** 极简版字符串实例: 仅供过渡期 String<T> 内部使用 */
    template<typename T> class StringInstance
    {
    public:
        using value_type = T;
        using base_type  = std::basic_string<T>;
        using size_type  = typename base_type::size_type;

    protected:
        base_type str;                                    ///< 实际存储
        static T *WritablePtr(base_type &s){ return s.empty()?nullptr:const_cast<T*>(s.data()); }
        static const T *ReadablePtr(const base_type &s){ return s.empty()?nullptr:s.data(); }

    public:
        StringInstance()=default;
        ~StringInstance()=default;
        StringInstance(const StringInstance &)=default;
        StringInstance(StringInstance &&) noexcept=default;
        StringInstance &operator=(const StringInstance &)=default;
        StringInstance &operator=(StringInstance &&) noexcept=default;

        explicit StringInstance(const T *s){ InitFromString(s,-1); }
        StringInstance(const T *s,int len){ InitFromString(s,len); }

        // 初始化 --------------------------------------------------
        void InitFromString(const T *src,int len)
        {
            if(!src){ str.clear(); return; }
            if(len<0) str.assign(src); else str.assign(src, src + hgl::strlen(src,len));
            while(!str.empty() && str.back()==T(0)) str.pop_back();
        }
        void InitFromInstance(T *src,int len)
        {
            if(!src){ str.clear(); return; }
            int real_len=hgl::strlen(src,len);
            while(real_len>0 && src[real_len-1]==T(0)) --real_len;
            if(real_len<=0){ delete[] src; str.clear(); return; }
            str.assign(src,src+real_len);
            delete[] src;
        }

        // 基础信息 ------------------------------------------------
        int  GetLength()const { return (int)str.size(); }
        T   *c_str(){ return WritablePtr(str); }
        const T *c_str()const{ return ReadablePtr(str); }
        T GetFirstChar()const { return str.empty()?T(0):str.front(); }
        T GetLastChar ()const { return str.empty()?T(0):str.back(); }

        // 禁用所有修改接口（全部迁移到新 String<T>）
        bool Insert(int,const T *,int)=delete;
        bool Delete(int,int)=delete;
        bool Clip(int,int)=delete;
        bool Resize(int)=delete;
        bool TrimRight(int)=delete;
        bool ClipLeft(int)=delete;
        bool Write(int,const StringInstance &)=delete;
        T *Discard()=delete;                 ///< 迁移后不再支持
        StringInstance *CreateCopy() const =delete;
        StringInstance *CreateCopy(int) const =delete;
        StringInstance *CreateCopy(int,int) const =delete;
    };//class StringInstance
}//namespace hgl

// End of file
