#pragma once

#include<hgl/type/DataType.h>
#include<hgl/type/SortedSet.h>
#include<string.h>
#include<utility>

namespace hgl
{
    /**
     * 字符串实例类
     */
    template<typename T> class StringInstance                                                       ///字符串实例类
    {
    protected:

        typedef StringInstance<T> SelfClass;

        int length;                                                                                 ///<字符串长度
        int malloc_length;                                                                          ///<空间实际分配长度
        
        T *buffer;

    protected:

        virtual void InitPrivate()
        {
            malloc_length=0;
            length=0;
            buffer=nullptr;
        }

        virtual void InitPrivate(const T *str,const int len)
        {
            length=len;

            malloc_length=power_to_2(length+1);

            buffer=new T[malloc_length];
            memcpy(buffer,str,length*sizeof(T));
            buffer[length]=0;
        }

        void EnsureCapacity(int need_length)   ///< 确保容量(包含结尾0)
        {
            if(need_length<=malloc_length)
                return;

            malloc_length=power_to_2(need_length);
            T *new_buf=new T[malloc_length];
            if(buffer&&length>0)
                memcpy(new_buf,buffer,length*sizeof(T));
            delete[] buffer;
            buffer=new_buf;
            buffer[length]=0;        // 保持结束符
        }

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

            // 注释: “如果最后不是0，则需要重新分配内存创建带0结尾的字串”
            // 原代码逻辑与注释相反，这里改正
            if(str[real_len]!=0)            // 最后不是0，需要重新分配
            {            
                InitPrivate(str,real_len);
                delete[] str;
            }
            else                            // 已经包含结束符，直接接管
            {
                length=real_len;
                malloc_length=len;          // len 是原始可用长度
                buffer=str;
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
            malloc_length=bs.malloc_length;    // 复用相同的容量策略
            buffer=new T[malloc_length];
            memcpy(buffer,bs.buffer,length*sizeof(T));
            buffer[length]=0;                  // 保证以0结尾
        }

        StringInstance(SelfClass &&other) noexcept
        {
            length=other.length;
            malloc_length=other.malloc_length;
            buffer=other.buffer;
            other.buffer=nullptr;
            other.length=0;
            other.malloc_length=0;
        }

        SelfClass &operator=(const SelfClass &bs)
        {
            if(this==&bs)
                return *this;

            delete[] buffer;
            if(bs.length<=0)
            {
                InitPrivate();
                return *this;
            }

            length=bs.length;
            malloc_length=bs.malloc_length;
            buffer=new T[malloc_length];
            memcpy(buffer,bs.buffer,length*sizeof(T));
            buffer[length]=0;
            return *this;
        }

        SelfClass &operator=(SelfClass &&other) noexcept
        {
            if(this==&other)
                return *this;

            delete[] buffer;
            length=other.length;
            malloc_length=other.malloc_length;
            buffer=other.buffer;
            other.buffer=nullptr;
            other.length=0;
            other.malloc_length=0;
            return *this;
        }

        virtual ~StringInstance()
        {
            delete[] buffer;        //delete[] NULL; don't is error
        }

        const bool isEmpty()const                                                                   ///<是否为空
        {
            return !buffer||length<=0;
        }

        SelfClass *CreateCopy()                                                                     ///<创建一份自身的拷贝
        {
            if(!buffer)return(0);

            SelfClass *sc=new SelfClass();

            sc->InitFromString(buffer,length);

            return sc;
        }

        SelfClass *CreateCopy(const int start)
        {
            if(!buffer)return(nullptr);
            if(start<0||start>=length)return(nullptr);
            
            SelfClass *sc=new SelfClass();

            sc->InitFromString(buffer+start,length-start);

            return sc;
        }

        SelfClass *CreateCopy(const int start,const int count)
        {
            if(!buffer)return(nullptr);
            if(start<0||count<=0)return(nullptr);

            SelfClass *sc=new SelfClass();

            sc->InitFromString(buffer+start,start+count>=length?length-start:count);

            return sc;
        }

        T *Discard()
        {
            T *result=buffer;

            buffer=nullptr;
            length=0;
            malloc_length=0;

            return result;
        }

        T * c_str()                                                                                ///<取得字符串C指针(可写)
        {
            return buffer;
        }

        const T * c_str() const                                                                    ///<取得字符串C指针(只读)
        {
            return buffer;
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
            return buffer?*buffer:0;
        }

        const T GetLastChar()const
        {
            return buffer&&length>0?*(buffer+length-1):0;
        }

        /**
         * 和一个字符串进行比较
         * @param sc 比较字符串
         * @return <0 我方小
         * @return 0 等同
         * @return >0 我方大
         */
        const int Comp(const SelfClass *sc)const
        {
            if(!sc)
                return length;

            return hgl::strcmp(buffer,length,sc->buffer,sc->length);
        }

        /**
         * 和一个字符串进行比较
         * @param pos 起始位置
         * @param sc 比较字符串
         * @return <0 我方小
         * @return 0 等同
         * @return >0 我方大
         */
        const int Comp(const uint pos,const SelfClass *sc)const
        {
            if(!sc)
                return length;

            if(length< (int)pos)
                return -1;

            return hgl::strcmp(buffer+pos,length-pos,sc->buffer,sc->length);
        }

        /**
         * 和一个字符串进行比较
         * @param str 比较字符串
         * @return <0 我方小
         * @return 0 等同
         * @return >0 我方大
         */
        const int Comp(const T *str)const
        {
            return hgl::strcmp(buffer,length,str,hgl::strlen(str));
        }

        /**
         * 和一个字符串进行比较
         * @param pos 起始位置
         * @param str 比较字符串
         * @return <0 我方小
         * @return 0 等同
         * @return >0 我方大
         */
        const int Comp(const uint pos,const T *str)const
        {
            if(!str)
                return length;

            if(length< (int)pos)
                return -1;

            return hgl::strcmp(buffer+pos,length-pos,str,hgl::strlen(str));
        }

        /**
         * 和一个字符串比较指字长度的字符
         * @param str 比较字符串
         * @param num 比较字数
         * @return <0 我方小
         * @return 0 等同
         * @return >0 我方大
         */
        const int Comp(const T *str,const uint num)const
        {
            if(!str)
                return length-num;

            return hgl::strcmp(buffer,str,num);
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
        const int Comp(const int pos,const T *str,const uint num)const
        {
            if(!str)
                return(length-pos);

            if(length<pos)
                return(-1);

            return hgl::strcmp(buffer+pos,str,num);
        }

        /**
         * 和那一个字符串进行比较,英文不区分大小写
         * @param str 比较字符串
         * @return <0 我方小
         * @return 0 等同
         * @return >0 我方大
         */
        const int CaseComp(const T *str)const
        {
            return hgl::stricmp(buffer,length,str,hgl::strlen(str));
        }

        /**
         * 和一个字符串进行比较指字长度的字符,英文不区分大小写
         * @param sc 比较字符串
         * @return <0 我方小
         * @return 0 等同
         * @return >0 我方大
         */
        const int CaseComp(const SelfClass &sc,const uint num)const
        {
            return hgl::stricmp(buffer,length,sc.buffer,num);
        }

        /**
         * 和那一个字符串进行比较指字长度的字符,英文不区分大小写
         * @param str 比较字符串
         * @return <0 我方小
         * @return 0 等同
         * @return >0 我方大
         */
        const int CaseComp(const T *str,const uint num)const
        {
            return hgl::stricmp(buffer,length,str,num);
        }

        /**
         * 和那一个字符串进行比较指字长度的字符,英文不区分大小写
         * @param pos 起始偏移位置
         * @param str 比较字符串
         * @return <0 我方小
         * @return 0 等同
         * @return >0 我方大
         */
        const int CaseComp(const int pos,const T *str,const uint num)const
        {
            if(pos<0)
                return -1;
            
            return hgl::stricmp(buffer+pos,length-pos,str,num);
        }

        bool Insert(const int pos,const T *istr,int len)                                                  ///<插入一个字符串
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
                hgl_move(buffer+pos+len,buffer+pos,length-pos+1);   // +1 包含原来的结束符

            hgl_cpy(buffer+pos,istr,len);
            length+=len;
            buffer[length]=0;
            return(true);
        }

        bool Insert(uint pos,const T &ch         ){return Insert(pos,    &ch,        1               );}
        bool Insert(uint pos,const T *str        ){return Insert(pos,    str,        hgl::strlen(str));}
        bool Insert(uint pos,const SelfClass &str){return Insert(pos,    str.c_str(),str.GetLength() );}

        bool Append(const T &ch                 ){return Insert(length, &ch,        1               );}
        bool Append(const T *str,const int len  ){return Insert(length, str,        len             );}
        bool Append(const T *str                ){return Insert(length, str,        hgl::strlen(str));}
        bool Append(const SelfClass &str        ){return Insert(length, str.c_str(),str.GetLength() );}

        bool Delete(uint pos,int num)                                                                ///<删除指定字符
        {
            if(pos>= (uint)length||num<0)return(false);

            if(num==0)return(true);

            if(pos+num> (uint)length)  //超出长度
            {
                buffer[pos]=0;
                length=pos;
            }
            else
            {
                hgl_typemove(buffer+pos,buffer+pos+num,length-pos-num);
                length-=num;
                buffer[length]=0;
            }

            return(true);
        }

        bool ClipLeft(int num)
        {
            if(num<0||num>length)
                return(false);

            buffer[num]=0;
            length=num;
            return(true);
        }

        bool TrimRight(int num)
        {
            return ClipLeft(length-num);
        }

        bool Clip(uint pos,int num)
        {
            if(pos> (uint)length || num<0 || pos+num> (uint)length)
                return(false);

            hgl_typemove(buffer,buffer+pos,num);
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

            hgl_typemove(buffer,buffer+start,n);
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
                delete[] buffer;
                buffer=nullptr;
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

            hgl_set(buffer+length,' ',num-length);   // 使用空格填充
            length=num;
            buffer[length]=0;
            return(true);
        }

        bool Write(uint pos,const SelfClass &str)
        {
            if(pos> (uint)length)
                return(false);

            int end_pos=pos+str.length;

            EnsureCapacity(end_pos+1);

            hgl_typecpy(buffer+pos,str.buffer,str.length);
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
