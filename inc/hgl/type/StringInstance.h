#ifndef HGL_TYPE_STRING_INSTANCE_INCLUDE
#define HGL_TYPE_STRING_INSTANCE_INCLUDE

#include<hgl/type/DataType.h>
#include<string.h>
namespace hgl
{
    /**
     * 字符串实例类
     */
    template<typename T> class StringInstance                                                       ///字符串实例类
    {
    protected:

        typedef StringInstance<T> SelfClass;

        uint length;                                                                                ///<字符串长度
        uint malloc_length;                                                                         ///<空间实际分配长度

        T *buffer;

    protected:

        virtual void InitPrivate()
        {
            malloc_length=0;
            length=0;
            buffer=0;
        }

        virtual void InitPrivate(const T *str,const uint len)
        {
            length=len;

            malloc_length=power_to_2(length+1);

            buffer=new T[malloc_length];
            memcpy(buffer,str,length*sizeof(T));
            buffer[length]=0;
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
            if(!str||!len)
            {
                InitPrivate();
                return;
            }

            if(len<0)
            {
                length=hgl::strlen(str);
            }
            else
            {
                length=hgl::strlen(str,len);

                while(length&&!str[length-1])               //清除后面的0
                    length--;
            }

            if(length<=0)
            {
                InitPrivate();
                return;
            }

            InitPrivate(str,length);
        }

        /**
         * 初始化字符串实例(基于一块已经分配好的内存)
         */
        void InitFromInstance(T *str,const uint len)
        {
            if(!str)
            {
                InitPrivate();
                return;
            }

            {
                length=hgl::strlen(str,len);

                while(length&&!str[length-1])               //清除后面的0
                    length--;
            }

            if(length<=0)
            {
                InitPrivate();
                delete[] str;

                return;
            }

            if(str[length]==0)                            //如果最后不是0，则需要重新分配内存创建带0结尾的字串
            {
                malloc_length=len;
                buffer=str;
                return;
            }
            else
            {            
                InitPrivate(str,length);

                delete[] str;
            }
        }

        StringInstance(const SelfClass &bs)
        {
            if(bs.Length<=0)
            {
                InitPrivate();
                return;
            }

            length=bs.length;

            if(length<=0)
                return;

            malloc_length=bs.malloc_length;

            buffer=new T[malloc_length];
            memcpy(buffer,bs.buffer,length*sizeof(T));
        }

        virtual ~StringInstance()
        {
            delete[] buffer;        //delete[] NULL; don't is error
        }

        const bool isEmpty()const                                                                   ///<是否为空
        {
            return !buffer;
        }

        SelfClass *CreateCopy()                                                                     ///<创建一份自身的拷贝
        {
            if(!buffer)return(0);

            SelfClass *sc=new SelfClass();

            sc->InitFromString(buffer,length);

            return sc;
        }

        SelfClass *CreateCopy(const uint start)
        {
            if(!buffer)return(nullptr);
            if(start>=length)return(nullptr);
            
            SelfClass *sc=new SelfClass();

            sc->InitFromString(buffer+start,length-start);

            return sc;
        }

        SelfClass *CreateCopy(const uint start,const uint count)
        {
            if(!buffer)return(nullptr);
            if(count==0)return(nullptr);

            SelfClass *sc=new SelfClass();

            sc->InitFromString(buffer+start,start+count>=length?length-start:count);

            return sc;
        }

        T *Discard()
        {
            T *result=buffer;

            buffer=nullptr;

            return result;
        }

        T * c_str()                                                                                 ///<取得字符串C指针
        {
            return buffer;
        }

        const uint GetLength()const                                                                 ///<取得字符串长度
        {
            return length;
        }

        bool GetChar(const uint n,T &ch)const
        {
            if(n>=length)
                return(false);

            ch=buffer[n];

            return(true);
        }

        bool SetChar(const uint n,const T &ch)
        {
            if(n>=length)
                return(false);

            buffer[n]=ch;

            return(true);
        }

        const T GetBeginChar()const
        {
            return buffer?*buffer:0;
        }

        const T GetEndChar()const
        {
            return buffer?*(buffer+length-1):0;
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

            if(length<pos)
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

            if(length<pos)
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
        const int Comp(const uint pos,const T *str,const uint num)const
        {
            if(!str)
                return(length-pos);

            if(length<pos)
                return(-1);

            return hgl::strcmp(buffer+pos,str,num);
        }

        /**
         * 和一个字符串进行比较,英文不区分大小写
         * @param sc 比较字符串
         * @return <0 我方小
         * @return 0 等同
         * @return >0 我方大
         */
        const int CaseComp(const SelfClass &sc)const
        {
            return hgl::stricmp(buffer,length,sc.buffer,sc.length);
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
        const int CaseComp(const uint pos,const T *str,const uint num)const
        {
            return hgl::stricmp(buffer+pos,length-pos,str,num);
        }

        bool Insert(const uint pos,const T *istr,int len)                                                  ///<插入一个字符串
        {
            if(!istr||!*istr)
                return(false);

            if(len==-1)
                len=hgl::strlen(istr);
            else
                len=hgl::strlen(istr,len);

            if(pos<0||pos>length||len<=0)return(false);

            const uint need_length=length+len+1;

            if(need_length>malloc_length)
            {
                malloc_length=power_to_2(need_length);

                T *new_str=new T[malloc_length];

                if(pos)
                    hgl_cpy(new_str,buffer,pos);

                hgl_cpy(new_str+pos,istr,len);

                if(pos<length)
                    hgl_cpy(new_str+pos+len,buffer+pos,length-pos);

                new_str[need_length-1]=0;

                length+=len;

                delete[] buffer;

                buffer=new_str;
            }
            else
            {
                hgl_move(buffer+pos+len,buffer+pos,length-pos+1);
                hgl_cpy(buffer+pos,istr,len);
                length+=len;
            }

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
            if(pos<0||pos>=length||num<0)return(false);

            if(num==0)return(true);

            if(pos+num>length)  //超出长度
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
            if(pos<0||pos>length
             ||num<0||pos+num>length)
                return(false);

            hgl_typemove(buffer,buffer+pos,num);
            buffer[num]=0;
            length=num;
            return(true);
        }

        bool SubString(const int start,int n=-1)
        {
            if(start<0||n==0)return(false);
            if(n>0&&start+n>length)return(false);

            if(n<0)
                n=length-start;

            hgl_typemove(buffer,buffer+start,n);
            buffer[n]=0;

            return(true);
        }

        bool Resize(int num)
        {
            if(num<=0)
                return(false);

            if(num<length)
            {
                buffer[num]=0;
                length=num;
            }
            else
            {
                if(length>=malloc_length)
                {
                    malloc_length=power_to_2(length+1);
                    T *new_str=new T[malloc_length];
                    hgl_typecpy(new_str,buffer,length);
                    delete[] buffer;
                    buffer=new_str;
                }

                hgl_set(buffer+length,' ',num-length);
                length=num;
                buffer[length]=0;
            }

            return(true);
        }

        bool Write(uint pos,const SelfClass &str)
        {
            if(pos<0||pos>length)
                return(false);

            int end_pos=pos+str.length;

            if(end_pos>malloc_length)
            {
                malloc_length=power_to_2(end_pos+1);

                T *new_str=new T[malloc_length];

                hgl_typecpy(new_str,buffer,length);
                delete[] buffer;
                buffer=new_str;
            }

            hgl_typecpy(buffer+pos,str.buffer,str.length);
            buffer[end_pos]=0;
            length=end_pos;
            return(true);
        }
    };//class StringInstance
}//namespace hgl
#endif//HGL_TYPE_STRING_INSTANCE_INCLUDE
