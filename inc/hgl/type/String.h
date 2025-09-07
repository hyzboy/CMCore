#pragma once

#include <hgl/type/DataType.h>
#include <hgl/type/StrChar.h>
#include <hgl/Comparator.h>
#include <string>
#include <initializer_list>
#include <unordered_set>

namespace hgl
{
    template<typename T> class String:public Comparator<String<T>>
    {
    protected:
        using SelfClass = String<T>;

        std::basic_string<T> buffer;                  // 唯一权威存储

    public:
        using CharType=T;

        // 构造/析构 ---------------------------------------------------------
        String()=default;
        String(const SelfClass &rhs){ Set(rhs); }
        String(SelfClass &&rhs) noexcept{ buffer=std::move(rhs.buffer); }
        String(const T *str){ fromString(str); }
        String(const T *str,int len){ fromString(str,len); }
        explicit String(const std::basic_string<T> &s){ buffer=s; }
        String(size_t count,T ch){ if(count>0) buffer.assign(count,ch); }
        String(std::initializer_list<T> il){ if(il.size()) buffer.assign(il.begin(),il.end()); }
        String(T)=delete;
        String(int)=delete; String(unsigned int)=delete; String(int64)=delete; String(uint64)=delete; String(float)=delete; String(double)=delete;
        virtual ~String()=default;

        // 赋值 --------------------------------------------------------------
        SelfClass &operator=(SelfClass &&rhs) noexcept
        { if(this!=&rhs){ buffer=std::move(rhs.buffer);} return *this; }
        SelfClass &operator=(const SelfClass &rhs){ if(this!=&rhs) Set(rhs); return *this; }
        SelfClass &operator=(const std::basic_string<T> &s){ buffer=s; return *this; }
        SelfClass &operator=(std::initializer_list<T> il){ if(!il.size()){ Clear(); return *this; } buffer.assign(il.begin(),il.end()); return *this; }
        SelfClass &operator=(const T *str){ if(str!=c_str()) fromString(str); return *this; }

        // 工厂 ---------------------------------------------------------------
        static SelfClass newOf(T *str,const uint len){ if(!str||len==0) return {}; int real=hgl::strlen(str,len); while(real>0 && str[real-1]==T(0)) --real; SelfClass s(str,real); delete[] str; return s; }
        static SelfClass charOf(const T &ch){ T tmp[2]; tmp[0]=ch; tmp[1]=0; return SelfClass(tmp); }

        // 显式数字转换 ------------------------------------------------------
        static SelfClass numberOf(int value){ T tmp[8*sizeof(int)]; itos(tmp,sizeof(tmp)/sizeof(T),value); return SelfClass(tmp); }
        static SelfClass numberOf(uint value){ T tmp[8*sizeof(uint)]; utos(tmp,sizeof(tmp)/sizeof(T),value); return SelfClass(tmp); }
        static SelfClass numberOf(int64 value){ T tmp[8*sizeof(int64)]; itos(tmp,sizeof(tmp)/sizeof(T),value); return SelfClass(tmp); }
        static SelfClass numberOf(uint64 value){ T tmp[8*sizeof(uint64)]; utos(tmp,sizeof(tmp)/sizeof(T),value); return SelfClass(tmp); }
        static SelfClass floatOf(float value,uint frac){ T tmp[8*sizeof(float)]; ftos(tmp,sizeof(tmp)/sizeof(T),frac,value); return SelfClass(tmp); }
        static SelfClass floatOf(double value,uint frac){ T tmp[8*sizeof(double)]; ftos(tmp,sizeof(tmp)/sizeof(T),frac,value); return SelfClass(tmp); }
        template<typename N1,typename N2>
        static SelfClass percentOf(const N1 num,const N2 den,uint frac){ return floatOf(double(num)/double(den)*100.0,frac); }

        // 基础信息 -----------------------------------------------------------
        const int Length()const { return int(buffer.size()); }
        const bool IsEmpty()const { return buffer.empty(); }
        const T GetFirstChar()const { return buffer.empty()?T(0):buffer.front(); }
        const T GetLastChar()const { return buffer.empty()?T(0):buffer.back(); }

        T *c_str() const { return buffer.empty()?nullptr:const_cast<T*>(buffer.data()); }
        T *c_str(){ return buffer.empty()?nullptr:const_cast<T*>(buffer.data()); }
        T *data_ptr(){ return c_str(); }

        // 载入/设置 ----------------------------------------------------------
        void fromString(const T *str,int len=-1)
        {
            if(!str || len==0){ Clear(); return; }
            if(len<0) buffer.assign(str); else buffer.assign(str,str+hgl::strlen(str,len));
            while(!buffer.empty() && buffer.back()==T(0)) buffer.pop_back();
        }
        void fromInstance(T *str,const uint len)   // 兼容旧API(保留指针版本)
        {
            if(!str){ Clear(); return; }
            int real_len=hgl::strlen(str,len); while(real_len>0 && str[real_len-1]==T(0)) --real_len; buffer.assign(str,str+real_len); delete[] str; }
        void Strcpy(const T *str,int len=-1){ fromString(str,len); }
        void StrcpyInstance(T *str,int len=-1){ fromInstance(str,len); }
        void Set(const T *str,int len=-1){ fromString(str,len); }

        void Set(const SelfClass &rhs)
        { if(this==&rhs) return; buffer=rhs.buffer; }

        bool Set(const SelfClass &rhs,int start,int count)
        { if(count<=0||start<0||start>=rhs.Length()) return false; int real=count; if(start+real>rhs.Length()) real=rhs.Length()-start; if(real<=0) return false; buffer=rhs.buffer.substr(size_t(start),size_t(real)); return true; }
        bool Copy(const SelfClass &rhs){ if(rhs.IsEmpty()) return false; return Set(rhs,0,rhs.Length()); }
        bool Unlink(){ return true; }
        T *Discard(){ if(Length()==0) return nullptr; T *out=new T[Length()+1]; memcpy(out,c_str(),Length()*sizeof(T)); out[Length()]=0; Clear(); return out; }

        // 修改 ---------------------------------------------------------------
        bool Insert(int pos,const T *str,int len=-1)
        { if(!str||pos<0||pos>Length()) return false; if(len==0) return false; if(len<0) len=hgl::strlen(str); if(len<=0) return false; buffer.insert(buffer.begin()+pos,str,str+len); return true; }
        bool Insert(int pos,const SelfClass &s){ return Insert(pos,s.c_str(),s.Length()); }
        bool Delete(int pos,int num){ if(num<=0||pos<0||pos>=Length()) return false; if(pos+num>Length()) num=Length()-pos; buffer.erase(buffer.begin()+pos,buffer.begin()+pos+num); return true; }
        
        // 预分配内存：预先保留至少 n 个字符的容量，避免多次扩展
        bool PreAlloc(int n){ if(n<=0) return false; buffer.reserve(size_t(n)); return true; }
        
        // 分配指定长度并返回可写指针；会直接把当前长度设为 n
        T *Resize(int n,bool fill_zero=false){ if(n<=0){ Clear(); return nullptr; } buffer.resize(size_t(n));

        if(fill_zero)
            hgl_zero(buffer.data(),size_t(n)*sizeof(T));

        return buffer.data(); }

        void Clear(){ buffer.clear(); }
        bool FillChar(const T ch,int start=0,int len=-1){ if(start<0||start>Length()) return false; if(len<0) len=Length()-start; if(len<=0) return false; for(int i=0;i<len;i++) buffer[size_t(start+i)]=ch; return true; }

        // 单字符 -------------------------------------------------------------
        bool GetChar(int n,T &ch)const { if(n<0||n>=Length()) return false; ch=c_str()[n]; return true; }
        bool SetChar(int n,const T ch){ if(n<0||n>=Length()) return false; buffer[size_t(n)]=ch; return true; }

        // 比较 ---------------------------------------------------------------
        int Comp(const SelfClass &rhs)const{ if(Length()==0) return rhs.Length(); if(rhs.Length()==0) return Length(); return hgl::strcmp(c_str(),Length(),rhs.c_str(),rhs.Length()); }
        int Comp(const T *str)const{ if(!c_str()){ if(!str) return 0; else return *str; } return hgl::strcmp(c_str(),Length(),str,hgl::strlen(str)); }
        int Comp(const uint pos,const SelfClass &rhs)const{ if(pos>(uint)Length()) return 0; return hgl::strcmp(c_str()+pos,Length()-pos,rhs.c_str(),rhs.Length()); }
        int Comp(const uint pos,const SelfClass &rhs,const int num)const{ if(pos>(uint)Length()||num<=0) return 0; return hgl::strcmp(c_str()+pos,rhs.c_str(),num); }
        int Comp(const uint pos,const T *str)const{ if(pos>(uint)Length()) return 0; return hgl::strcmp(c_str()+pos,Length()-pos,str,hgl::strlen(str)); }
        int Comp(const uint pos,const T *str,const int num)const{ if(pos>(uint)Length()||num<=0) return 0; return hgl::strcmp(c_str()+pos,str,num); }
        int CaseComp(const SelfClass &rhs)const{ if(Length()==0) return rhs.Length(); return hgl::stricmp(c_str(),Length(),rhs.c_str(),rhs.Length()); }
        int CaseComp(const T *str)const{ if(!c_str()){ if(!str) return 0; return *str; } return hgl::stricmp(c_str(),Length(),str,hgl::strlen(str)); }
        int Comp(const SelfClass &rhs,const int num)const{ if(num<=0) return 0; return hgl::strcmp(c_str(),rhs.c_str(),num); }
        int Comp(const T *str,const int num)const{ if(num<=0) return 0; return hgl::strcmp(c_str(),str,num); }
        int CaseComp(const SelfClass &rhs,const int num)const{ if(num<=0) return 0; return hgl::stricmp(c_str(),Length(),rhs.c_str(),num); }
        int CaseComp(const T *str,const int num)const{ if(num<=0) return 0; return hgl::stricmp(c_str(),Length(),str,num); }
        int CaseComp(const uint pos,const T *str,const int num)const{ if(num<=0||pos>(uint)Length()) return 0; return hgl::stricmp(c_str()+pos,Length()-pos,str,num); }

        // 数字转换 -----------------------------------------------------------
        bool ToBool(bool &result)const { return stob(c_str(),result); }
        template<typename I> bool ToInt(I &result)const { return etof(c_str(),result); }
        template<typename U> bool ToUint(U &result)const { return etof(c_str(),result); }
        template<typename F> bool ToFloat(F &result)const { return etof(c_str(),result); }

        // 大小写 -------------------------------------------------------------
        SelfClass &LowerCase(){ if(Length()>0){ tolower(buffer.data()); } return *this; }
        SelfClass ToLowerCase()const { if(Length()==0) return SelfClass(); SelfClass tmp(*this); return tmp.LowerCase(); }
        SelfClass &UpperCase(){ if(Length()>0){ toupper(buffer.data()); } return *this; }
        SelfClass ToUpperCase()const { if(Length()==0) return SelfClass(); SelfClass tmp(*this); return tmp.UpperCase(); }

    protected:
        typedef const T *(*ConvFunc)(const T *,int &,const bool (*trimfunc)(const T &));
        SelfClass StrConv(ConvFunc conv) const
        { if(Length()==0) return SelfClass(); int new_len=Length(); const T *new_str=conv(c_str(),new_len,isspace<T>); if(new_len<=0) return SelfClass(); return SelfClass(new_str,new_len); }

    public:
        // 修剪/截取 ----------------------------------------------------------
        SelfClass TrimLeft() const { return StrConv(trimleft); }
        SelfClass TrimRight() const { return StrConv(trimright); }
        SelfClass Trim() const { return StrConv(trim); }
        bool TrimLeft(int n){ return Delete(0,n); }
        bool TrimRight(int n){ if(n<=0||n>Length()) return false; buffer.erase(buffer.end()-n,buffer.end()); return true; }
        bool ClipLeft(int n){ if(n<0) return false; if(n>=Length()){ Clear(); return true; } buffer.resize(size_t(n)); return true; }
        bool ClipRight(int n){ if(n<0||n>Length()) return false; return Delete(0,Length()-n); }
        bool Clip(int pos,int num){ return Delete(pos,num); }
        SelfClass SubString(int start,int n=-1) const{ if(start<0||start>=Length()) return SelfClass(); if(n<0) n=Length()-start; if(n<=0||start+n>Length()) return SelfClass(); return SelfClass(c_str()+start,n); }
        bool SubString(SelfClass &sc,int start,int n) const{ if(start<0||n<=0||start+n>Length()) return false; sc.Set(c_str()+start,n); return true; }

        int StatChar(const T ch)const { return ::StatChar(c_str(),ch); }
        int StatLine()const { return ::StatLine(c_str()); }

        // 查找 --------------------------------------------------------------
        int FindChar(int pos,const T ch)const{ if(pos<0||pos>=Length()) return -1; const T *r=hgl::strchr(c_str()+pos,ch); return r? int(r-(c_str()+pos)):-1; }
        int FindChar(const T ch)const { return FindChar(0,ch); }
        int FindChars(int pos,const SelfClass &ch)const{ if(pos<0||pos>=Length()) return -1; const T *r=hgl::strchr(c_str()+pos,ch.c_str(),ch.Length()); return r? int(r-(c_str()+pos)):-1; }
        int FindChars(const SelfClass &ch)const { return FindChars(0,ch); }
        int FindRightChar(const T ch)const{ if(Length()==0) return -1; const T *r=hgl::strrchr(c_str(),Length(),ch); return r? int(r-c_str()):-1; }
        int FindRightChars(const SelfClass &ch)const{ if(Length()==0) return -1; const T *r=hgl::strrchr(c_str(),Length(),ch.c_str(),ch.Length()); return r? int(r-c_str()):-1; }
        int FindRightChar(const int off,const T ch)const{ if(Length()==0) return -1; const T *r=hgl::strrchr(c_str(),Length(),off,ch); return r? int(r-c_str()):-1; }
        int FindRightChar(const int off,const SelfClass &ch)const{ if(Length()==0) return -1; const T *r=hgl::strrchr(c_str(),Length(),off,ch.c_str(),ch.Length()); return r? int(r-c_str()):-1; }
        int FindExcludeChar(int pos,const T &ch)const{ if(pos<0||pos>=Length()) return -1; const T *r=hgl::strechr(c_str()+pos,ch); return r? int(r-(c_str()+pos)):-1; }
        int FindExcludeChar(int pos,const SelfClass &ch)const{ if(pos<0||pos>=Length()) return -1; const T *r=hgl::strechr(c_str()+pos,ch.c_str(),ch.Length()); return r? int(r-(c_str()+pos)):-1; }
        int FindExcludeChar(const SelfClass &ch)const { return FindExcludeChar(0,ch); }
        int FindString(const SelfClass &str,int start=0)const{ if(str.Length()<=0||start<0||start>Length()-str.Length()) return -1; const T *r=strstr(c_str()+start,Length()-start,str.c_str(),str.Length()); return r? int(r-c_str()):-1; }

        int ClearString(const SelfClass &sub){ if(sub.IsEmpty()||IsEmpty()) return 0; int count=0; size_t pos=0; while((pos=buffer.find(sub.c_str(),pos,sub.Length()))!=std::basic_string<T>::npos){ buffer.erase(pos,sub.Length()); ++count; } return count; }
        bool WriteString(int pos,const SelfClass &str){ if(pos<0||pos>Length()) return false; if(pos+str.Length()>Length()) buffer.resize(pos+str.Length()); memcpy(buffer.data()+pos,str.c_str(),str.Length()*sizeof(T)); return true; }
        int Replace(const T tch,const T sch){ if(IsEmpty()) return 0; int cnt=0; for(auto &c:buffer) if(c==tch){ c=sch; ++cnt; } return cnt; }

        // 运算符 -------------------------------------------------------------
        const T &operator[](int index) const { static const T zero_char=0; if(index>=0 && index<Length()) return c_str()[index]; return zero_char; }
        T &operator[](int index){ static T zero_char=* (new T(0)); if(index<0||index>=Length()) return zero_char; return buffer[index]; }
        operator T *(){ return c_str(); }
        operator const T *()const{ return c_str(); }
        SelfClass &operator+=(const SelfClass &str){ Insert(Length(),str.c_str(),str.Length()); return *this; }
        SelfClass &operator<<(const SelfClass &str){ return operator+=(str); }
        static SelfClass ComboString(const T *s1,int l1,const T *s2,int l2){ if(!s1||l1<=0){ if(!s2||l2<=0) return SelfClass(); return SelfClass(s2,l2);} if(!s2||l2<=0) return SelfClass(s1,l1); SelfClass r; r.buffer.reserve(l1+l2); r.buffer.append(s1,l1); r.buffer.append(s2,l2); return r; }
        SelfClass operator+(const SelfClass &str) const{ if(str.IsEmpty()) return *this; if(IsEmpty()) return str; return ComboString(c_str(),Length(),str.c_str(),str.Length()); }
        SelfClass operator+(const T ch) const{ if(IsEmpty()) return SelfClass::charOf(ch); return ComboString(c_str(),Length(),&ch,1); }
        SelfClass operator+(const T *str) const{ if(IsEmpty()) return SelfClass(str); return ComboString(c_str(),Length(),str,hgl::strlen(str)); }
        SelfClass operator+(const int &) const = delete;
        SelfClass operator+(const uint &) const = delete;
        SelfClass operator+(const int64 &) const = delete;
        SelfClass operator+(const uint64 &) const = delete;
        SelfClass operator+(const float &) const = delete;
        SelfClass operator+(const double &) const = delete;

        const int compare(const SelfClass &str)const override{ return Comp(str); }

        int UniqueCharCount()const
        {
            if(IsEmpty()) return 0;
            std::unordered_set<T> uniq; uniq.reserve(buffer.size());
            for(const auto &ch:buffer) uniq.insert(ch);
            return int(uniq.size());
        }
        bool Strcat(const T *str,int len=-1){ if(!str) return false; if(len==0) return false; if(len<0) len=hgl::strlen(str); if(len<=0) return false; return Insert(Length(),str,len); }
        bool Strcat(const SelfClass &bs){ return Insert(Length(),bs.c_str(),bs.Length()); }
    };//class String

    using AnsiString=String<char>;
    using U8String  =String<u8char>;
    using U16String =String<u16char>;
    using U32String =String<char32_t>;
    using OSString  =String<os_char>;
    using WideString=String<wchar_t>;
    using ShaderString=U8String;

    // 全局 const T* + String<T>
    template<typename T>
    inline String<T> operator+(const T *lhs,const String<T> &rhs)
    {
        if(!lhs || *lhs==0) return rhs;
        if(rhs.IsEmpty()) return String<T>(lhs);
        return String<T>::ComboString(lhs,hgl::strlen(lhs),rhs.c_str(),rhs.Length());
    }

    template<typename C> bool ToNumber(const String<C> &str,int &value){ return str.ToInt(value);}    
    template<typename C> bool ToNumber(const String<C> &str,uint &value){ return str.ToUint(value);}  
    template<typename C> bool ToNumber(const String<C> &str,float &value){ return str.ToFloat(value);} 
    template<typename C> bool ToNumber(const String<C> &str,double &value){ return str.ToFloat(value);} 

    template<typename T2,int HASH_MAX> uint StringFastHash(const String<T2> &str){ const T2 *p=str.c_str(); int c=str.Length(); uint result=0; while(c-->0) result+=*p++; return(result%HASH_MAX); }

    template<typename T2,typename I> String<T2> ToHexString(const I &value){ T2 str[(sizeof(I)<<1)+1]; ToUpperHexStr(str,value); return String<T2>(str); }
    template<typename T2,typename I> String<T2> HexToString(const I &value){ T2 str[(sizeof(I)<<1)+1]; htos(str,(sizeof(I)<<1)+1,value); return String<T2>(str); }
    template<typename T2> String<T2> PointerToHexString(const void *ptr){ return HexToString<T2,HGL_POINTER_UINT>(reinterpret_cast<const HGL_POINTER_UINT>(ptr)); }
    inline String<os_char> PointerToHexOSString(const void *value){ return PointerToHexString<os_char>(value); }
    inline String<char> PointerToHexU8String(const void *value){ return PointerToHexString<char>(value); }
    inline String<u16char> PointerToHexU16String(const void *value){ return PointerToHexString<u16char>(value); }

    template<typename T2> inline void strcpy(T2 *dst,int max_count,const String<T2> &src){ hgl::strcpy(dst,max_count,src.c_str(),src.Length()); }
    template<typename T2> inline void strcat(T2 *dst,int max_count,const String<T2> &src){ hgl::strcat(dst,max_count,src.c_str(),src.Length()); }
}//namespace hgl
