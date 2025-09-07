#pragma once

#include<hgl/type/DataType.h>
#include<hgl/type/SortedSet.h>
#include<string.h>
#include<utility>
#include <memory>

namespace hgl
{
    /**
     * 字符串实例类
     */
    template<typename T> class StringInstance                                                       ///字符串实例类
    {
    protected:
        typedef StringInstance<T> SelfClass;
        int length;
        int malloc_length;
        std::unique_ptr<T[]> buffer;

    protected:

        virtual void InitPrivate()
        {
            malloc_length=0;
            length=0;
            buffer.reset();
        }
        virtual void InitPrivate(const T *str,const int len)
        {
            length=len;
            malloc_length=power_to_2(length+1);
            buffer=std::make_unique<T[]>(malloc_length);
            memcpy(buffer.get(),str,length*sizeof(T));
            buffer[length]=0;
        }
        void EnsureCapacity(int need_length)
        {
            if(need_length<=malloc_length)
                return;
            malloc_length=power_to_2(need_length);
            std::unique_ptr<T[]> new_buf=std::make_unique<T[]>(malloc_length);
            if(buffer&&length>0)
                memcpy(new_buf.get(),buffer.get(),length*sizeof(T));
            buffer=std::move(new_buf);
            buffer[length]=0;
        }

        static int Normalize(int v){return v<0?-1:(v>0?1:0);}         

    public:

        StringInstance()
        {
            InitPrivate();
        }

        /**
         * 初始化字符串实例(基于一个字符串)
         */
        void InitFromString(const T *str,const int len)
        {
            if(!str)
            {
                InitPrivate();
                return;
            }

            int real_len=len;

            if(len<0)
            {
                real_len=hgl::strlen(str);
            }
            else
            {
                real_len=hgl::strlen(str,len);

                while(real_len&&!str[real_len-1])               //清除后面的0
                    real_len--;
            }

            if(real_len<=0)
            {
                InitPrivate();
                return;
            }

            InitPrivate(str,real_len);
        }

        /**
         * 初始化字符串实例(基于一块已经分配好的内存)
         */
        void InitFromInstance(T *str,const int len)
        {
            if(!str)
            {
                InitPrivate();
                return;
            }

            int real_len=hgl::strlen(str,len);

            while(real_len&&!str[real_len-1])               //清除后面的0
                real_len--;

            if(real_len<=0)
            {
                InitPrivate();
                delete[] str;
                return;
            }

            // 如果没有空间放终止符或末尾不是0，则需要重新分配
            const bool need_copy = (real_len>=len) || (str[real_len]!=0);
            if(need_copy)
            {
                InitPrivate(str,real_len);
                delete[] str;
            }
            else                            // 已经包含结束符，直接接管
            {
                length=real_len;
                malloc_length=len;          // len 是原始可用长度
                buffer.reset(str); // 只接管由 new[] 分配的指针
            }
        }

        StringInstance(const SelfClass &bs)
        {
            if(bs.length<=0)
            {
                InitPrivate();
                return;
            }
            length=bs.length;
            malloc_length=bs.malloc_length;
            buffer=std::make_unique<T[]>(malloc_length);
            memcpy(buffer.get(),bs.buffer.get(),length*sizeof(T));
            buffer[length]=0;
        }
        StringInstance(SelfClass &&other) noexcept
        {
            length=other.length;
            malloc_length=other.malloc_length;
            buffer=std::move(other.buffer);
            other.length=0;
            other.malloc_length=0;
        }
        SelfClass &operator=(const SelfClass &bs)
        {
            if(this==&bs)
                return *this;
            buffer.reset();
            if(bs.length<=0)
            {
                InitPrivate();
                return *this;
            }
            length=bs.length;
            malloc_length=bs.malloc_length;
            buffer=std::make_unique<T[]>(malloc_length);
            memcpy(buffer.get(),bs.buffer.get(),length*sizeof(T));
            buffer[length]=0;
            return *this;
        }
        SelfClass &operator=(SelfClass &&other) noexcept
        {
            if(this==&other)
                return *this;
            buffer=std::move(other.buffer);
            length=other.length;
            malloc_length=other.malloc_length;
            other.length=0;
            other.malloc_length=0;
            return *this;
        }
        virtual ~StringInstance() = default;

        const bool isEmpty()const                                                                   ///<是否为空
        {
            return !buffer||length<=0;
        }

        SelfClass *CreateCopy()                                                                     ///<创建一份自身的拷贝
        {
            if(!buffer)return(0);

            SelfClass *sc=new SelfClass();

            sc->InitFromString(buffer.get(),length);

            return sc;
        }

        SelfClass *CreateCopy(const int start)
        {
            if(!buffer)return(nullptr);
            if(start<0||start>=length)return(nullptr);
            
            SelfClass *sc=new SelfClass();

            sc->InitFromString(buffer.get()+start,length-start);

            return sc;
        }

        SelfClass *CreateCopy(const int start,const int count)
        {
            if(!buffer)return(nullptr);
            if(start<0||count<=0)return(nullptr);

            SelfClass *sc=new SelfClass();

            sc->InitFromString(buffer.get()+start,start+count>=length?length-start:count);

            return sc;
        }

        T *Discard()
        {
            T *result=buffer.release();
            length=0;
            malloc_length=0;
            return result;
        }
        T * c_str()                                                                                ///<取得字符串C指针(可写)
        {
            return buffer.get();
        }
        const T * c_str() const                                                                    ///<取得字符串C指针(只读)
        {
            return buffer.get();
        }

        const int GetLength()const                                                                 ///<取得字符串长度
        {
            return length;
        }

        bool GetChar(const int n,T &ch)const
        {
            if(n<0||n>=length)
                return(false);

            ch=buffer[n];

            return(true);
        }

        bool SetChar(const int n,const T &ch)
        {
            if(n<0||n>=length)
                return(false);

            buffer[n]=ch;

            return(true);
        }

        const T GetFirstChar()const
        {
            return buffer ? *(buffer.get()) : 0;
        }

        const T GetLastChar()const
        {
            return buffer && length > 0 ? *(buffer.get() + length - 1) : 0;
        }

        /**
         * 和一个字符串进行比较
         * @param sc 比较字符串
         * @return <0 我方小
         * @return 0 等同
         * @return >0 我方大
         */
        int Comp(const SelfClass *sc)const
        {
            const bool a_empty=isEmpty();
            const bool b_empty=(!sc||sc->isEmpty());
            if(a_empty&&b_empty) return 0; 
            if(a_empty) return -1; 
            if(b_empty) return 1; 
            return Normalize(hgl::strcmp(buffer.get(),length,sc->buffer.get(),sc->length));
        }

        /**
         * 和一个字符串进行比较
         * @param pos 起始位置
         * @param sc 比较字符串
         * @return <0 我方小
         * @return 0 等同
         * @return >0 我方大
         */
        int Comp(const int pos, const SelfClass *sc) const
        {
            if(pos < 0 || pos > length) return 0;
            if(!sc) return (length-pos)>0 ? 1 : 0;
            return Comp(sc->CreateCopy() ? sc : sc);
        }

        /**
         * 和一个字符串进行比较
         * @param str 比较字符串
         * @return <0 我方小
         * @return 0 等同
         * @return >0 我方大
         */
        int Comp(const T *str)const
        {
            const bool a_empty=isEmpty(); 
            const bool b_empty=(!str||!*str); 
            if(a_empty&&b_empty) return 0; 
            if(a_empty) return -1; 
            if(b_empty) return 1; 
            return Normalize(hgl::strcmp(buffer.get(),length,str,hgl::strlen(str))); 
        }

        /**
         * 和一个字符串进行比较
         * @param pos 起始位置
         * @param str 比较字符串
         * @return <0 我方小
         * @return 0 等同
         * @return >0 我方大
         */
        int Comp(const int pos, const T *str) const
        {
            if(pos < 0 || pos > length) return 0;
            const int remain = length - pos;
            const bool a_empty = (remain <= 0);
            const bool b_empty = (!str || !*str);
            if(a_empty && b_empty) return 0;
            if(a_empty) return -1;
            if(b_empty) return 1;
            return Normalize(hgl::strcmp(buffer.get() + pos, remain, str, hgl::strlen(str)));
        }

        /**
         * 和一个字符串比较指字长度的字符
         * @param str 比较字符串
         * @param num 比较字数
         * @return <0 我方小
         * @return 0 等同
         * @return >0 我方大
         */
        int Comp(const T *str,int num)const
        {
            const bool a_empty=isEmpty(); 
            const bool b_empty=(!str||num==0); 
            if(num<=0) return 0; 
            if(a_empty&&b_empty) return 0; 
            if(a_empty) return -1; 
            if(b_empty) return 1; 
            return Normalize(hgl::strcmp(buffer.get(),str,num)); 
        }

        /**
         * 和一个字符串比较指字长度的字符
         * @param pos 起始偏移位置
         * @param str 比较字符串
         * @param num 比较字数
         * @return <0 我方小
         * @return 0 等同
         * @return >0 我方大
         */
        int Comp(const int pos,const T *str,int num)const
        {
            if(pos < 0 || pos > length || num <= 0) return 0;
            const int remain=length-pos; 
            const bool a_empty=(remain<=0); 
            const bool b_empty=(!str); 
            if(a_empty&&b_empty) return 0; 
            if(a_empty) return -1; 
            if(b_empty) return 1; 
            return Normalize(hgl::strcmp(buffer.get()+pos,str,num)); 
        }

        /**
         * 和一个字符串进行比较,英文不区分大小写
         * @param str 比较字符串
         * @return <0 我方小
         * @return 0 等同
         * @return >0 我方大
         */
        int CaseComp(const T *str)const
        {
            const bool a_empty=isEmpty(); 
            const bool b_empty=(!str||!*str); 
            if(a_empty&&b_empty) return 0; 
            if(a_empty) return -1; 
            if(b_empty) return 1; 
            return Normalize(hgl::stricmp(buffer.get(),length,str,hgl::strlen(str))); 
        }

        /**
         * 和一个字符串进行比较指字长度的字符,英文不区分大小写
         * @param sc 比较字符串
         * @return <0 我方小
         * @return 0 等同
         * @return >0 我方大
         */
        int CaseComp(const SelfClass &sc,int num)const
        { 
            if(num<=0) return 0; 
            const bool a_empty=isEmpty(); 
            const bool b_empty=sc.isEmpty(); 
            if(a_empty&&b_empty) return 0; 
            if(a_empty) return -1; 
            if(b_empty) return 1; 
            return Normalize(hgl::stricmp(buffer.get(),length,sc.buffer.get(),num)); 
        }

        /**
         * 和那一个字符串进行比较指字长度的字符,英文不区分大小写
         * @param str 比较字符串
         * @return <0 我方小
         * @return 0 等同
         * @return >0 我方大
         */
        int CaseComp(const T *str,int num)const
        { 
            if(num<=0) return 0; 
            const bool a_empty=isEmpty(); 
            const bool b_empty=(!str); 
            if(a_empty&&b_empty) return 0; 
            if(a_empty) return -1; 
            if(b_empty) return 1; 
            return Normalize(hgl::stricmp(buffer.get(),length,str,num)); 
        }

        /**
         * 和那一个字符串进行比较指字长度的字符,英文不区分大小写
         * @param pos 起始偏移位置
         * @param str 比较字符串
         * @return <0 我方小
         * @return 0 等同
         * @return >0 我方大
         */
        int CaseComp(const int pos,const T *str,int num)const
        { 
            if(num<=0) return 0; 
            if(pos<0||pos>length) return 0; 
            const int remain=length-pos; 
            const bool a_empty=(remain<=0); 
            const bool b_empty=(!str); 
            if(a_empty&&b_empty) return 0; 
            if(a_empty) return -1; 
            if(b_empty) return 1; 
            return Normalize(hgl::stricmp(buffer.get()+pos,remain,str,num)); 
        }

        bool Insert(int pos,const T *istr,int len)                                                  ///<插入一个字符串
        {
            if(!istr||!(*istr))
                return(false);

            if(len==-1)
                len=hgl::strlen(istr);
            else
                len=hgl::strlen(istr,len);

            if(pos<0||pos>length||len<=0)return(false);

            const int need_length=length+len+1;   // 包含结束符
            EnsureCapacity(need_length);

            if(pos<length)
                hgl_move(buffer.get()+pos+len,buffer.get()+pos,length-pos+1);   // +1 包含原来的结束符

            hgl_cpy(buffer.get()+pos,istr,len);
            length+=len;
            buffer[length]=0;
            return(true);
        }

        bool Insert(int pos,const T &ch         ){return Insert(pos,    &ch,        1               );}
        bool Insert(int pos,const T *str        ){return Insert(pos,    str,        hgl::strlen(str));}
        bool Insert(int pos,const SelfClass &str){return Insert(pos,    str.c_str(),str.GetLength() );}

        bool Append(const T &ch                 ){return Insert(length, &ch,        1               );}
        bool Append(const T *str,const int len  ){return Insert(length, str,        len             );}
        bool Append(const T *str                ){return Insert(length, str,        hgl::strlen(str));}
        bool Append(const SelfClass &str        ){return Insert(length, str.c_str(),str.GetLength() );}

        bool Delete(int pos, int num)
        {
            if(pos < 0 || pos >= length || num < 0) return false;

            if(num == 0) return true;

            if(pos + num > length)  //超出长度
            {
                buffer[pos]=0;
                length=pos;
            }
            else
            {
                hgl_typemove(buffer.get()+pos,buffer.get()+pos+num,length-pos-num);
                length-=num;
                buffer[length]=0;
            }

            return(true);
        }

        bool Clip(int pos, int num)
        {
            if(pos < 0 || pos > length || num < 0 || pos + num > length) return false;
            hgl_typemove(buffer.get(),buffer.get()+pos,num);
            buffer[num]=0;
            length=num;
            return(true);
        }

        bool SubString(const int start,int n=-1)
        {
            if(start<0||start>=length||n==0)return(false);
            if(n>0&&start+n>length)return(false);

            if(n<0)
                n=length-start;

            hgl_typemove(buffer.get(),buffer.get()+start,n);
            buffer[n]=0;
            length=n;
            return(true);
        }

        bool Resize(int num)
        {
            if(num<0)
                return(false);

            if(num==length)
                return true;

            if(num==0)
            {
                buffer.reset();
                length=0;
                malloc_length=0;
                return true;
            }

            if(num<length)
            {
                length=num;
                buffer[length]=0;
                return(true);
            }

            EnsureCapacity(num+1);   // +1 为结尾0

            hgl_set(buffer.get()+length,' ',num-length);   // 使用空格填充
            length=num;
            buffer[length]=0;
            return(true);
        }

        bool Write(int pos, const SelfClass &str)
        {
            if(pos < 0 || pos > length) return false;

            int end_pos=pos+str.length;

            EnsureCapacity(end_pos+1);

            hgl_typecpy(buffer.get()+pos,str.buffer.get(),str.length);
            buffer[end_pos]=0;
            length=end_pos>length?end_pos:length;
            return(true);
        }

    public:

        int UniqueCharCount()const
        {
            if(!buffer||length<=0)
                return 0;

            SortedSet<T> unique_chars;

            for(int i=0;i<length;i++)
                unique_chars.Add(buffer[i]);

            return unique_chars.GetCount();
        }
    };//class StringInstance
}//namespace hgl
