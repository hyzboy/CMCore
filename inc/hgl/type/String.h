#pragma once

#include<hgl/type/StringInstance.h>
#include<hgl/Comparator.h>

namespace hgl
{
    /** 字符串类 (第1步重构: 去除引用计数/共享语义) */
    template<typename T> class String:public Comparator<String<T>>
    {
    protected:
        using SelfClass   = String<T>;
        using InstClass   = StringInstance<T>;

        InstClass *data = nullptr;                 ///< 独占指针(不共享)

        void free_data() { if(data){ delete data; data=nullptr; } }

    public:
        using CharType=T;

        String()=default;

        explicit String(InstClass *ic)
        {
            data=ic;    // 接管
        }

        String(const T *str)
        {
            fromString(str);
        }
        String(const T *str,int len)
        {
            fromString(str,len);
        }

        static String<T> newOf(T *str,const uint len)
        {
            InstClass *si=new InstClass();
            si->InitFromInstance(str,len);
            return String<T>(si);
        }

        String(const char)=delete;

        static String<T> charOf(const T &ch)
        {
            T *str=new T[2];
            str[0]=ch;str[1]=0;
            return String<T>::newOf(str,1);
        }

        String(const InstClass &si)
        {
            data=new InstClass(si);   // 深拷贝
        }
        String(const SelfClass &bs)
        {
            Set(bs);
        }

        // 数值静态构造保留原接口
        #define BASE_STRING_NUMBER_CONSTRUCT(type,func) \
        String(const type num)=delete;  \
        static String<T> numberOf(const type value)  \
        {   \
            InstClass *si=new InstClass();  \
            const int len=8*sizeof(type);   \
            T *tmp_str=new T[len];   \
            func(tmp_str,len,value);  \
            si->InitFromInstance(tmp_str,hgl::strlen(tmp_str)); \
            return String<T>(si);   \
        }

        static String<T> numberOf(const float value)=delete;
        static String<T> numberOf(const double value)=delete;

        BASE_STRING_NUMBER_CONSTRUCT(int,   itos);
        BASE_STRING_NUMBER_CONSTRUCT(uint,  utos);
        BASE_STRING_NUMBER_CONSTRUCT(int64, itos);
        BASE_STRING_NUMBER_CONSTRUCT(uint64,utos);
        #undef BASE_STRING_NUMBER_CONSTRUCT

        #define BASE_STRING_FLOAT_CONSTRUCT(type,func) \
        String(const type num)=delete;  \
        static String<T> floatOf(const type value,const uint frac_num)  \
        {   \
            InstClass *si=new InstClass();  \
            const int len=8*sizeof(type);   \
            T *tmp_str=new T[len];   \
            func(tmp_str,len,frac_num,value);  \
            si->InitFromInstance(tmp_str,hgl::strlen(tmp_str)); \
            return String<T>(si);   \
        }
        BASE_STRING_FLOAT_CONSTRUCT(float, ftos);
        BASE_STRING_FLOAT_CONSTRUCT(double,ftos);
        #undef BASE_STRING_FLOAT_CONSTRUCT

        template<typename N1,typename N2>
        static String<T> percentOf(const N1 numerator,const N2 denominator,const uint frac_num)
        {
            return String<T>::floatOf(double(numerator)/double(denominator)*100.0f,frac_num);
        }

        String(const int *value,int N)=delete;
        static String<T> numberOf(const int *value,int N)
        {
            const int size=N*sizeof(int)*8;int len;T tmp_str[size];
            itos(tmp_str,size,*value);++value;
            for(int i=1;i<N;i++)
            { strcat(tmp_str,(const T)(',')); len=strlen(tmp_str); itos(tmp_str+len,size-len,*value); ++value; }
            return String<T>(tmp_str);
        }
        String(const float *value,int N)=delete;
        static String<T> numberOf(const float *value,int N)
        {
            const int size=N*sizeof(float)*16;int len;T tmp_str[size];
            ftos(tmp_str,size,*value);++value;
            for(int i=1;i<N;i++)
            { len=strlen(tmp_str); tmp_str[len++]=','; ftos(tmp_str+len,size-len,*value); ++value; }
            return String<T>(tmp_str);
        }

        virtual ~String(){ free_data(); }

        const T GetFirstChar()const { return data?data->GetFirstChar():0; }
        const T GetLastChar()const  { return data?data->GetLastChar():0; }
        const int Length()const     { return data?data->GetLength():0; }
        const bool IsEmpty()const   { return !data||data->GetLength()<=0; }

        T *c_str()const { return data?data->c_str():nullptr; }
        T *c_str()      { return data?data->c_str():nullptr; }
        T *data_ptr()   { return data?data->c_str():nullptr; }

        T *strchr(T ch)const
        {
            if(!data)return nullptr; int result=FindChar(ch); if(result<0)return nullptr; return data->c_str()+result;
        }
        T *strrchr(T ch)const
        {
            if(!data)return nullptr; int result=FindRightChar(ch); if(result<0)return nullptr; return data->c_str()+result;
        }

        void fromString(const T *str,int len=-1)
        {
            if(!str || len==0){ Clear(); return; }
            free_data(); data=new InstClass(); data->InitFromString(str,len); if(data->GetLength()==0){ free_data(); }
        }
        void fromInstance(T *str,const uint len)
        {
            if(!str){ Clear(); return; }
            free_data(); data=new InstClass(); data->InitFromInstance(str,len); if(data->GetLength()==0){ free_data(); }
        }

        void Strcpy(const T *str,int len=-1){ fromString(str,len); }
        void StrcpyInstance(T *str,int len=-1){ fromInstance(str,len); }
        void Set(const T *str,int len=-1){ fromString(str,len); }

        void Set(const SelfClass &bs)
        {
            if(&bs==this) return; free_data(); if(!bs.data) return; data=new InstClass(*bs.data); }
        void Set(const InstClass &si){ free_data(); data=new InstClass(si); }
        void Set(InstClass *si_ptr){ free_data(); data=si_ptr; }

        bool Set(const SelfClass &bs,int start,int count)
        {
            if(count<=0||!bs.data) return false; free_data(); InstClass *cp=bs.data->CreateCopy(start,count); if(!cp) return false; data=cp; return true; }

        bool Copy(const SelfClass &bs)
        { if(bs.IsEmpty()) return false; free_data(); data=bs.data?bs.data->CreateCopy():nullptr; return data; }

        bool Unlink() { return true; }   // 不再需要复制

        T *Discard()
        {
            if(!data) return nullptr; T *ptr=data->Discard(); delete data; data=nullptr; return ptr; }

        bool GetChar(int n,T &ch)const { if(n<0||!data) return false; return data->GetChar(n,ch); }
        bool SetChar(int n,const T ch){ if(n<0||!data) return false; return data->SetChar(n,ch); }

        bool Insert(int pos,const T *str,int len=-1)
        {
            if(!str)return false; int curlen=Length(); if(pos<0||pos>curlen) return false; if(len==0) return false; if(!data){ fromString(str,len); return true; }
            if(len<0) len=hgl::strlen(str); if(len<=0) return false; return data->Insert(pos,str,len);
        }
        bool Insert(int pos,const SelfClass &str){ return Insert(pos,str.c_str(),str.Length()); }

        bool Delete(int pos,int num){ if(num<=0||pos<0||pos>=Length()||!data) return false; return data->Delete(pos,num); }
        void Clear(){ free_data(); }

        int Comp(const SelfClass &bs)const
        {
            if(!data) return bs.Length(); if(bs.Length()<=0) return Length(); return data->Comp(bs.data); }
        int Comp(const T *str)const
        {
            if(!data){ return str?*str:0; } return data->Comp(str); }
        int Comp(const uint pos,const SelfClass &bs)const
        { if(!data) return bs.Length(); if(bs.Length()<=0) return Length(); return data->Comp(pos,bs.data); }
        int Comp(const uint pos,const SelfClass &bs,const int num)const
        { if(!data) return bs.Length(); if(bs.Length()<=0) return Length(); return data->Comp(pos,bs.data,num); }
        int Comp(const uint pos,const T *str)const
        { if(!data){ return str?*str:0; } return data->Comp(pos,str); }
        int Comp(const uint pos,const T *str,const int num)const
        { if(!data){ return str?*str:0; } return data->Comp(pos,str,num); }

        int CaseComp(const SelfClass &bs)const
        { if(!data) return bs.Length(); int len=bs.Length(); if(len<=0) return 1; return data->CaseComp(bs.c_str()); }
        int CaseComp(const T *str)const
        { if(!data) return str?*str:0; return data->CaseComp(str); }
        int Comp(const SelfClass &bs,const int num)const
        { if(!data){ if(num<=0) return 0; return bs.Length(); } if(bs.Length()<=0) return 1; return data->Comp(bs.c_str(),num); }
        int Comp(const T *str,const int num)const
        { if(!data){ if(!str||num<=0) return 0; return *str; } return data->Comp(str,num); }
        int CaseComp(const SelfClass &bs,const int num)const
        { if(!data){ if(num<=0) return 0; return bs.Length(); } if(bs.Length()<=0) return 1; return data->CaseComp(bs.c_str(),num); }
        int CaseComp(const T *str,const int num)const
        { if(!data){ if(!str||num<=0) return 0; return *str; } return data->CaseComp(str,num); }
        int CaseComp(const uint pos,const T *str,const int num)const
        { if(!data){ if(!str||num<=0) return 0; return *str; } return data->CaseComp(pos,str,num); }

        bool ToBool(bool &result)const { return data?stob(data->c_str(),result):false; }
        template<typename I> bool ToInt(I &result)const { return data?etof(data->c_str(),result):false; }
        template<typename U> bool ToUint(U &result)const { return data?etof(data->c_str(),result):false; }
        template<typename F> bool ToFloat(F &result)const { return data?etof(data->c_str(),result):false; }

        SelfClass &LowerCase(){ if(data) tolower(data->c_str()); return *this; }
        SelfClass ToLowerCase()const { if(!data) return SelfClass(); return SelfClass(data->c_str()).LowerCase(); }
        SelfClass &UpperCase(){ if(data) toupper(data->c_str()); return *this; }
        SelfClass ToUpperCase()const { if(!data) return SelfClass(); return SelfClass(data->c_str()).UpperCase(); }

        bool FillChar(const T ch,int start=0,int len=-1)
        {
            if(!data) return false; if(start<0||start>data->GetLength()) return false; if(len<0) len=data->GetLength()-start; if(len<=0) return false; hgl_set(data->c_str()+start,ch,len); return true;
        }

    protected:
        typedef const T *(*ConvFunc)(const T *,int &,const bool (*trimfunc)(const T &));
        SelfClass StrConv(ConvFunc conv) const
        {
            if(!data||data->GetLength()<=0) return SelfClass(); int new_len=data->GetLength(); const T *new_str=conv(data->c_str(),new_len,isspace<T>); if(new_len<=0) return SelfClass(); return SelfClass(new_str,new_len); }

    public:
        SelfClass TrimLeft() const { return StrConv(trimleft); }
        SelfClass TrimRight() const { return StrConv(trimright); }
        SelfClass Trim() const { return StrConv(trim); }

        bool TrimLeft(int n){ return Delete(0,n); }
        bool TrimRight(int n){ return data?data->TrimRight(n):false; }
        bool ClipLeft(int n){ return data?data->ClipLeft(n):false; }
        bool ClipRight(int n){ return Delete(0,Length()-n); }
        bool Clip(int pos,int num)
        { int curlen=Length(); if(pos<0||pos>=curlen||num<=0||pos+num>curlen||!data) return false; return data->Clip(pos,num); }

        SelfClass SubString(int start,int n=-1) const
        { int len=Length(); if(start<0||start>=len) return SelfClass(); if(n<0) n=len-start; if(n==0||start+n>len) return SelfClass(); return SelfClass(data->c_str()+start,n); }
        bool SubString(SelfClass &sc,int start,int n) const
        { int len=Length(); if(start<0||n<=0||start+n>len) return false; sc.Set(data->c_str()+start,n); return true; }

        bool Resize(int n){ if(n<0||!data) return false; if(n==0){ Clear(); return true; } return data->Resize(n); }
        int StatChar(const T ch)const { return data?StatChar(data->c_str(),ch):-1; }
        int StatLine()const { return data?StatLine(data->c_str()):-1; }

        int FindChar(int pos,const T ch)const
        { if(!data||pos<0) return -1; const T *result=hgl::strchr(data->c_str()+pos,ch); if(result) return result-(data->c_str()+pos); return -1; }
        int FindChar(const T ch)const { return FindChar(0,ch); }
        int FindChars(int pos,const String<T> &ch)const
        { if(!data||pos<0) return -1; const T *result=hgl::strchr(data->c_str()+pos,ch.c_str(),ch.Length()); if(result) return result-(data->c_str()+pos); return -1; }
        int FindChars(const String<T> &ch)const { return FindChars(0,ch); }
        int FindRightChar(const T ch)const
        { if(!data) return -1; const T *result=hgl::strrchr(data->c_str(),data->GetLength(),ch); if(result) return result-(data->c_str()); return -1; }
        int FindRightChars(const String<T> &ch)const
        { if(!data) return -1; const T *result=hgl::strrchr(data->c_str(),data->GetLength(),ch.c_str(),ch.Length()); if(result) return result-(data->c_str()); return -1; }
        int FindRightChar(const int off,const T ch)const
        { if(!data) return -1; const T *result=hgl::strrchr(data->c_str(),data->GetLength(),off,ch); if(result) return result-(data->c_str()); return -1; }
        int FindRightChar(const int off,const String<T> &ch)const
        { if(!data) return -1; const T *result=hgl::strrchr(data->c_str(),data->GetLength(),off,ch.c_str(),ch.Length()); if(result) return result-(data->c_str()); return -1; }
        int FindExcludeChar(int pos,const T &ch)const
        { if(!data||pos<0) return -1; const T *result=hgl::strechr(data->c_str()+pos,ch); if(result) return result-(data->c_str()+pos); return -1; }
        int FindExcludeChar(int pos,const String<T> &ch)const
        { if(!data||pos<0) return -1; const T *result=hgl::strechr(data->c_str()+pos,ch.c_str(),ch.Length()); if(result) return result-(data->c_str()+pos); return -1; }
        int FindExcludeChar(const String &ch)const { return FindExcludeChar(0,ch); }
        int FindString(const SelfClass &str,int start=0)const
        { if(!data||str.Length()<=0) return -1; if(start<0||start>data->GetLength()-str.Length()) return -1; const T *result=strstr(data->c_str()+start,data->GetLength()-start,str.c_str(),str.Length()); if(result) return result-(data->c_str()); return -1; }

        int ClearString(const SelfClass &sub)
        { if(!data) return -1; if(sub.Length()<=0) return false; T *sp=data->c_str(); T *tp=sub.c_str(); T *p; int len=data->GetLength(); int sub_len=sub.Length(); int count=0; while((p=strstr(sp,tp))) { len-=(p-sp)+sub_len; sp=p; memcpy(p,p+sub_len,len*sizeof(T)); count++; } return count; }

        bool WriteString(int pos,const SelfClass &str)
        { if(pos<0||!data||str.Length()<=0) return false; return data->Write(pos,str); }
        int Replace(const T tch,const T sch)
        { if(!data) return -1; return replace(data->c_str(),tch,sch); }

        // 操作符
        operator const InstClass &() { static InstClass empty; return data?*data:empty; }
        const T &operator [](int index) const
        { static const T zero_char=0; if(data && index>=0 && index<data->GetLength()) return *(data->c_str()+index); return zero_char; }
        T &operator [](int index)
        { static T zero_char= *(new T(0)); if(!data||index<0||index>=data->GetLength()) return zero_char; return *(data->c_str()+index); }
        operator T *() { return (T*)c_str(); }
        operator const T *()const { return c_str(); }

        SelfClass &operator = (const T *str) { if(str!=c_str()) Set(str); return *this; }
        SelfClass &operator = (const SelfClass &str) { if(str.c_str()!=c_str()) Set(str); return *this; }

        SelfClass &operator += (const SelfClass &str){ Strcat(str); return *this; }
        SelfClass &operator << (const SelfClass &str){ return operator+=(str); }

        static SelfClass ComboString(const T *str1,int len1,const T *str2,int len2)
        {
            if(!str1||len1<=0)
            { if(!str2||len2<=0) return SelfClass(); return SelfClass(str2,len2); }
            else { if(!str2||len2<=0) return SelfClass(str1,len1); }
            const int new_len=len1+len2; T *ms=new T[new_len+1]; memcpy(ms,str1,len1*sizeof(T)); memcpy(ms+len1,str2,len2*sizeof(T)); ms[new_len]=0; return String::newOf(ms,new_len);
        }

        SelfClass operator + (const SelfClass &str) const
        { if(str.Length()<=0) return *this; if(!data) return str; return ComboString(data->c_str(),data->GetLength(),str.c_str(),str.Length()); }
        SelfClass operator + (const T ch) const
        { if(!data) return SelfClass::charOf(ch); return ComboString(data->c_str(),data->GetLength(),&ch,1); }
        SelfClass operator + (const T *str) const
        { if(!data) return SelfClass(str); return ComboString(data->c_str(),data->GetLength(),str,strlen(str)); }

        // 数值相加重载：直接生成临时字符串再拼接（避免 SharedPtr 依赖）
        SelfClass operator + (const int &num) const      { return *this + SelfClass::numberOf(num); }
        SelfClass operator + (const uint &num) const     { return *this + SelfClass::numberOf(num); }
        SelfClass operator + (const int64 &num) const    { return *this + SelfClass::numberOf(num); }
        SelfClass operator + (const uint64 &num) const   { return *this + SelfClass::numberOf(num); }
        SelfClass operator + (const float &num) const    { return *this + SelfClass::floatOf(num,6); }
        SelfClass operator + (const double &num) const   { return *this + SelfClass::floatOf(num,6); }

        const int compare(const SelfClass &str)const override { return Comp(str); }

        int UniqueCharCount()const { return data?data->UniqueCharCount():0; }

        bool Strcat(const T *str,int len=-1)
        { if(!str) return false; if(len==0) return false; if(len<0) len=hgl::strlen(str); if(len<=0) return false; return Insert(Length(),str,len); }
        bool Strcat(const SelfClass &bs){ return Insert(Length(),bs.c_str(),bs.Length()); }
    };//class String

    template<typename V,typename T2> String<T2> operator + (const V &value,const String<T2> &str)
    { if(str.Length()<=0) return String<T2>(value); return String<T2>(value)+str; }

    using AnsiString=String<char>;
    using U8String  =String<u8char>;
    using U16String =String<u16char>;
    using U32String =String<char32_t>;
    using OSString  =String<os_char>;
    using WString   =String<wchar_t>;

    using ShaderString  =U8String;

    template<typename C> bool ToNumber(const String<C> &str,int &value){return str.ToInt(value);}    
    template<typename C> bool ToNumber(const String<C> &str,uint &value){return str.ToUint(value);}  
    template<typename C> bool ToNumber(const String<C> &str,float &value){return str.ToFloat(value);} 
    template<typename C> bool ToNumber(const String<C> &str,double &value){return str.ToFloat(value);} 

    template<typename T2,int HASH_MAX> uint StringFastHash(const String<T2> &str)
    { const T2 *p=str.c_str(); int c=str.Length(); uint result=0; while(c-->0) result+=*p++; return(result%HASH_MAX); }

    template<typename T2,typename I> String<T2> ToHexString(const I &value)
    { T2 str[(sizeof(I)<<1)+1]; ToUpperHexStr(str,value); return String<T2>(str); }
    template<typename T2,typename I> String<T2> HexToString(const I &value)
    { T2 str[(sizeof(I)<<1)+1]; htos(str,(sizeof(I)<<1)+1,value); return String<T2>(str); }
    template<typename T2> String<T2> PointerToHexString(const void *ptr)
    { return HexToString<T2,HGL_POINTER_UINT>(reinterpret_cast<const HGL_POINTER_UINT>(ptr)); }
    inline String<os_char> PointerToHexOSString(const void *value){ return PointerToHexString<os_char>(value); }
    inline String<char> PointerToHexU8String(const void *value){ return PointerToHexString<char>(value); }
    inline String<u16char> PointerToHexU16String(const void *value){ return PointerToHexString<u16char>(value); }

    template<typename T2> inline void strcpy(T2 *dst,int max_count,const String<T2> &src)
    { hgl::strcpy(dst,max_count,src.c_str(),src.Length()); }
    template<typename T2> inline void strcat(T2 *dst,int max_count,const String<T2> &src)
    { hgl::strcat(dst,max_count,src.c_str(),src.Length()); }
}//namespace hgl
