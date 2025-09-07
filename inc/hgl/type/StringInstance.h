#pragma once

#include<hgl/type/DataType.h>
#include<hgl/type/SortedSet.h>
#include<string.h>
#include<utility>
#include <memory>
#include <string>
#include <unordered_set>
#include <algorithm>

namespace hgl
{
    /**
     * 字符串实例类 (基于 std::basic_string 封装)
     * 目标：保持原 String<T> 对此类的接口/语义 (除去无法零拷贝接管外部 new[] 内存) 基本兼容。
     */
    template<typename T> class StringInstance
    {
    public:
        using SelfClass = StringInstance<T>;
        using value_type = T;
    protected:
        std::basic_string<T> str;   // 底层存储

        static int Normalize(int v){return v<0?-1:(v>0?1:0);}        

        // 标准库字符串无法真正“接管”外部已分配缓冲区，只能复制。
        // 为空时返回 nullptr 以兼容旧代码 (旧实现 buffer 为 nullptr 时返回 nullptr)。
        static T *WritablePtr(std::basic_string<T> &s)
        {
            return s.empty()?nullptr:const_cast<T*>(s.data());
        }
        static const T *ReadablePtr(const std::basic_string<T> &s)
        {
            return s.empty()?nullptr:s.data();
        }

    public:
        StringInstance()=default;
        ~StringInstance()=default;
        StringInstance(const SelfClass &)=default;
        StringInstance(SelfClass &&) noexcept =default;
        SelfClass &operator=(const SelfClass &)=default;
        SelfClass &operator=(SelfClass &&) noexcept =default;

        // -------- 初始化 --------
        void InitFromString(const T *src,int len)
        {
            if(!src){ str.clear(); return; }
            if(len<0)
                str.assign(src);
            else
                str.assign(src, src + hgl::strlen(src,len));
            // 去掉尾部多余的 0 (与旧实现行为一致)
            while(!str.empty() && str.back()==T(0)) str.pop_back();
        }

        void InitFromInstance(T *src,int len)
        {
            if(!src){ str.clear(); return; }
            int real_len=hgl::strlen(src,len);
            while(real_len>0 && src[real_len-1]==T(0)) --real_len;
            if(real_len<=0){ delete[] src; str.clear(); return; }
            str.assign(src,src+real_len);   // 复制并释放
            delete[] src;
        }

        // -------- 基础信息 --------
        const bool isEmpty()const { return str.empty(); }
        const int  GetLength()const { return (int)str.size(); }

        T *c_str(){ return WritablePtr(str); }                  // 兼容旧接口 (可写)
        const T *c_str()const{ return ReadablePtr(str); }       // 兼容旧接口 (只读)

        T *Discard()                                            // 复制出动态数组并清空自身
        {
            if(str.empty()) return nullptr;
            T *buf=new T[str.size()+1];
            memcpy(buf,str.data(),str.size()*sizeof(T));
            buf[str.size()]=0;
            str.clear();
            return buf;
        }

        // -------- 拷贝辅助 --------
        SelfClass *CreateCopy() const
        {
            return new SelfClass(*this);
        }
        SelfClass *CreateCopy(int start) const
        {
            if(start<0||start>=GetLength()) return nullptr;
            SelfClass *sc=new SelfClass();
            sc->str.assign(str.begin()+start,str.end());
            return sc;
        }
        SelfClass *CreateCopy(int start,int count) const
        {
            if(start<0||count<=0||start>=GetLength()) return nullptr;
            int real = std::min<int>(count,GetLength()-start);
            SelfClass *sc=new SelfClass();
            sc->str.assign(str.begin()+start,str.begin()+start+real);
            return sc;
        }

        // -------- 读写单字符 --------
        bool GetChar(int n,T &ch)const
        {
            if(n<0||n>=GetLength()) return false; ch=str[(size_t)n]; return true;
        }
        bool SetChar(int n,const T &ch)
        {
            if(n<0||n>=GetLength()) return false; str[(size_t)n]=ch; return true;
        }
        T GetFirstChar()const { return str.empty()?T(0):str.front(); }
        T GetLastChar ()const { return str.empty()?T(0):str.back(); }

        // -------- 比较 (保持旧签名) --------
        int Comp(const SelfClass *sc)const
        {
            const bool a_empty=isEmpty();
            const bool b_empty=(!sc||sc->isEmpty());
            if(a_empty&&b_empty) return 0;
            if(a_empty) return -1;
            if(b_empty) return 1;
            return Normalize(hgl::strcmp(c_str(),GetLength(),sc->c_str(),sc->GetLength()));
        }
        int Comp(int pos,const SelfClass *sc)const
        {
            if(pos<0||pos>GetLength()) return 0;
            if(!sc) return (GetLength()-pos)>0?1:0;
            return Normalize(hgl::strcmp(c_str()+pos,GetLength()-pos,sc->c_str(),sc->GetLength()));
        }
        int Comp(int pos,const SelfClass *sc,int num)const
        {
            if(pos<0||pos>GetLength()||num<=0) return 0;
            if(!sc||sc->isEmpty()) return (GetLength()-pos)>0?1:0;
            return Normalize(hgl::strcmp(c_str()+pos, sc->c_str(), num));
        }
        int Comp(const T *s)const
        {
            const bool a_empty=isEmpty();
            const bool b_empty=(!s||!*s);
            if(a_empty&&b_empty) return 0;
            if(a_empty) return -1;
            if(b_empty) return 1;
            return Normalize(hgl::strcmp(c_str(),GetLength(),s,hgl::strlen(s)));
        }
        int Comp(int pos,const T *s)const
        {
            if(pos<0||pos>GetLength()) return 0;
            const int remain=GetLength()-pos;
            const bool a_empty=(remain<=0);
            const bool b_empty=(!s||!*s);
            if(a_empty&&b_empty) return 0; if(a_empty) return -1; if(b_empty) return 1;
            return Normalize(hgl::strcmp(c_str()+pos,remain,s,hgl::strlen(s)));
        }
        int Comp(const T *s,int num)const
        {
            if(num<=0) return 0;
            const bool a_empty=isEmpty(); const bool b_empty=(!s||num==0);
            if(a_empty&&b_empty) return 0; if(a_empty) return -1; if(b_empty) return 1;
            return Normalize(hgl::strcmp(c_str(),s,num));
        }
        int Comp(int pos,const T *s,int num)const
        {
            if(pos<0||pos>GetLength()||num<=0) return 0;
            const int remain=GetLength()-pos; const bool a_empty=(remain<=0); const bool b_empty=(!s);
            if(a_empty&&b_empty) return 0; if(a_empty) return -1; if(b_empty) return 1;
            return Normalize(hgl::strcmp(c_str()+pos,s,num));
        }

        int CaseComp(const T *s)const
        {
            const bool a_empty=isEmpty(); const bool b_empty=(!s||!*s);
            if(a_empty&&b_empty) return 0; if(a_empty) return -1; if(b_empty) return 1;
            return Normalize(hgl::stricmp(c_str(),GetLength(),s,hgl::strlen(s)));
        }
        int CaseComp(const SelfClass &sc,int num)const
        {
            if(num<=0) return 0; const bool a_empty=isEmpty(); const bool b_empty=sc.isEmpty();
            if(a_empty&&b_empty) return 0; if(a_empty) return -1; if(b_empty) return 1;
            return Normalize(hgl::stricmp(c_str(),GetLength(),sc.c_str(),num));
        }
        int CaseComp(const T *s,int num)const
        {
            if(num<=0) return 0; const bool a_empty=isEmpty(); const bool b_empty=(!s);
            if(a_empty&&b_empty) return 0; if(a_empty) return -1; if(b_empty) return 1;
            return Normalize(hgl::stricmp(c_str(),GetLength(),s,num));
        }
        int CaseComp(int pos,const T *s,int num)const
        {
            if(num<=0||pos<0||pos>GetLength()) return 0;
            const int remain=GetLength()-pos; const bool a_empty=(remain<=0); const bool b_empty=(!s);
            if(a_empty&&b_empty) return 0; if(a_empty) return -1; if(b_empty) return 1;
            return Normalize(hgl::stricmp(c_str()+pos,remain,s,num));
        }

        // -------- 修改操作 --------
        bool Insert(int pos,const T *istr,int len)
        {
            if(!istr||pos<0||pos>GetLength()) return false;
            if(len==-1) len=hgl::strlen(istr); else len=hgl::strlen(istr,len);
            if(len<=0) return false;
            str.insert((size_t)pos, istr, (size_t)len);
            return true;
        }
        bool Insert(int pos,const T &ch){ return Insert(pos,&ch,1); }
        bool Insert(int pos,const T *s){ return Insert(pos,s,hgl::strlen(s)); }
        bool Insert(int pos,const SelfClass &s){ return Insert(pos,s.c_str(),s.GetLength()); }

        bool Append(const T &ch){ return Insert(GetLength(),&ch,1); }
        bool Append(const T *s,int len){ return Insert(GetLength(),s,len); }
        bool Append(const T *s){ return Insert(GetLength(),s,hgl::strlen(s)); }
        bool Append(const SelfClass &s){ return Insert(GetLength(),s.c_str(),s.GetLength()); }

        bool Delete(int pos,int num)
        {
            if(pos<0||num<0||pos>=GetLength()) return false;
            if(num==0) return true;
            if(pos+num>GetLength()) num=GetLength()-pos;
            str.erase((size_t)pos,(size_t)num);
            return true;
        }

        // Clip: 保留 [pos, pos+num) 段
        bool Clip(int pos,int num)
        {
            if(pos<0||num<0||pos+num>GetLength()) return false;
            str.assign(str.begin()+pos,str.begin()+pos+num);
            return true;
        }

        // SubString: 把本串裁成从 start 开始长度 n(或到末尾)
        bool SubString(int start,int n=-1)
        {
            if(start<0||start>=GetLength()||n==0) return false;
            int real_len=(n<0)?(GetLength()-start):n;
            if(real_len<0||start+real_len>GetLength()) return false;
            str.assign(str.begin()+start,str.begin()+start+real_len);
            return true;
        }

        bool Resize(int num)
        {
            if(num<0) return false;
            if(num==0){ str.clear(); return true; }
            str.resize((size_t)num, T(' '));
            return true;
        }

        // TrimRight: 删除末尾 n 个字符
        bool TrimRight(int n)
        {
            if(n<0) return false; if(n==0) return true;
            if(n>=GetLength()){ str.clear(); return true; }
            str.erase(str.size()-n);
            return true;
        }
        // ClipLeft: 保留前 n 个字符 (等同长度裁剪)
        bool ClipLeft(int n)
        {
            if(n<0) return false; if(n>=GetLength()) return true; str.resize((size_t)n); return true;
        }

        bool Write(int pos,const SelfClass &other)
        {
            if(pos<0||pos>GetLength()) return false;
            const int end_pos=pos+other.GetLength();
            if(end_pos>GetLength()) str.resize((size_t)end_pos);
            std::copy(other.str.begin(),other.str.end(),str.begin()+pos);
            return true;
        }

        int UniqueCharCount()const
        {
            if(str.empty()) return 0;
            std::unordered_set<T> s(str.begin(),str.end());
            return (int)s.size();
        }
    };//class StringInstance
}//namespace hgl

// End of file
