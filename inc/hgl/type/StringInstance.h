#pragma once

#include<hgl/type/DataType.h>
#include<string.h>
#include<string>
#include<algorithm>

namespace hgl
{
    /** 精简版字符串实例类：仅做 std::basic_string 轻量包装，保留旧 String 所需接口 (正在被移除) */
    template<typename T> class StringInstance
    {
    public:
        using value_type = T;
        using base_type  = std::basic_string<T>;
        using size_type  = typename base_type::size_type;

    protected:
        base_type str;                        ///< 底层真实存储

        static T *WritablePtr(base_type &s){return s.empty()?nullptr:const_cast<T*>(s.data());}
        static const T *ReadablePtr(const base_type &s){return s.empty()?nullptr:s.data();}

    public:
        StringInstance()=default;
        ~StringInstance()=default;
        StringInstance(const StringInstance &)=default;
        StringInstance(StringInstance &&) noexcept=default;
        StringInstance &operator=(const StringInstance &)=default;
        StringInstance &operator=(StringInstance &&) noexcept=default;

        // 直接构造自 C 风格串
        explicit StringInstance(const T *s){ InitFromString(s,-1); }
        StringInstance(const T *s,int len){ InitFromString(s,len); }

    // 初始化 --------------------------------------------------
        void InitFromString(const T *src,int len)
        {
            if(!src){ str.clear(); return; }
            if(len<0)
                str.assign(src);
            else
                str.assign(src, src + hgl::strlen(src,len));
            // 去除末尾多余 0
            while(!str.empty() && str.back()==T(0)) str.pop_back();
        }

        void InitFromInstance(T *src,int len)
        {
            if(!src){ str.clear(); return; }
            int real_len=hgl::strlen(src,len);
            while(real_len>0 && src[real_len-1]==T(0)) --real_len;
            if(real_len<=0){ delete[] src; str.clear(); return; }
            str.assign(src,src+real_len);
            delete[] src;        // 与旧实现一致：接管后释放
        }

    // 基础信息 ------------------------------------------------
        const bool isEmpty()const { return str.empty(); }
        const int  GetLength()const { return (int)str.size(); }

        T *c_str(){ return WritablePtr(str); }
        const T *c_str()const{ return ReadablePtr(str); }

        T GetFirstChar()const { return str.empty()?T(0):str.front(); }
        T GetLastChar ()const { return str.empty()?T(0):str.back(); }

        // 复制出动态数组并清空自身（与原语义保持）
        T *Discard()
        {
            if(str.empty()) return nullptr;
            T *buf=new T[str.size()+1];
            memcpy(buf,str.data(),str.size()*sizeof(T));
            buf[str.size()]=0;
            str.clear();
            return buf;
        }

    // 拷贝辅助  -----------------------------------------------
        StringInstance *CreateCopy() const { return new StringInstance(*this); }
        StringInstance *CreateCopy(int start) const
        {
            if(start<0||start>=GetLength()) return nullptr;
            auto *sc=new StringInstance();
            sc->str.assign(str.begin()+start,str.end());
            return sc;
        }
        StringInstance *CreateCopy(int start,int count) const
        {
            if(start<0||count<=0||start>=GetLength()) return nullptr;
            int real = std::min<int>(count,GetLength()-start);
            auto *sc=new StringInstance();
            sc->str.assign(str.begin()+start,str.begin()+start+real);
            return sc;
        }

    // 修改操作 -------------------------------------------------
        bool Insert(int pos,const T *istr,int len)
        {
            if(!istr||pos<0||pos>GetLength()) return false;
            if(len==-1) len=hgl::strlen(istr); else len=hgl::strlen(istr,len);
            if(len<=0) return false;
            str.insert((size_type)pos,istr,(size_type)len);
            return true;
        }
        bool Delete(int pos,int num)
        {
            if(pos<0||num<0||pos>=GetLength()) return false; if(num==0) return true; if(pos+num>GetLength()) num=GetLength()-pos;
            str.erase((size_type)pos,(size_type)num); return true;
        }
        bool Clip(int pos,int num)                             // 保留 [pos,pos+num)
        {
            if(pos<0||num<0||pos+num>GetLength()) return false;
            str.assign(str.begin()+pos,str.begin()+pos+num); return true;
        }
        bool Resize(int num)
        {
            if(num<0) return false; if(num==0){ str.clear(); return true; }
            str.resize((size_type)num,T(' ')); return true;
        }
        bool TrimRight(int n)
        {
            if(n<0) return false; if(n==0) return true; if(n>=GetLength()){ str.clear(); return true; }
            str.erase(str.size()-n); return true;
        }
        bool ClipLeft(int n)                                   // 保留前 n 个
        {
            if(n<0) return false; if(n>=GetLength()) return true; str.resize((size_type)n); return true;
        }
        bool Write(int pos,const StringInstance &other)
        {
            if(pos<0||pos>GetLength()) return false; int end_pos=pos+other.GetLength(); if(end_pos>GetLength()) str.resize((size_type)end_pos);
            std::copy(other.str.begin(),other.str.end(),str.begin()+pos); return true;
        }
    };//class StringInstance
}//namespace hgl

// End of file
