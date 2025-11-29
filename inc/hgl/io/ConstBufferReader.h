#ifndef HGL_IO_CONST_BUFFER_READER_INCLUDE
#define HGL_IO_CONST_BUFFER_READER_INCLUDE

#include<hgl/type/String.h>
#include<hgl/io/SeekOrigin.h>
namespace hgl
{
    namespace io
    {
        class ConstBufferReader
        {
            const void *buffer;
            const uint8 *pointer;
            int64 size;

        public:

            ConstBufferReader(const void *buf,const int64 s)
            {
                buffer=buf;
                pointer=(uint8 *)buffer;
                size=s;
            }

            ConstBufferReader(ConstBufferReader &cbr,const int64 s)
            {
                buffer=cbr.CurPointer();
                pointer=(uint8 *)buffer;
                size=s;
            }

            template<typename T> bool Read(T &v)
            {
                if(size<sizeof(T))return(false);

                v=*(T *)pointer;

                pointer+=sizeof(T);

                return(true);
            }

            template<typename T1,typename T2> bool CastRead(T2 &v)
            {
                if(size<sizeof(T1))return(false);

                v=(T2)*(T1 *)pointer;

                pointer+=sizeof(T1);

                return(true);
                
            }

            template<typename T> bool ReadArray(T *v,const int64 count)
            {
                if(size<count*sizeof(T))return(false);

                memcpy(v,pointer,count*sizeof(T));

                pointer+=count*sizeof(T);

                return(true);
            }

        private:

            template<typename T,typename S> int _ReadString(T *str)
            {
                if(!str)return(-1);

                S str_len;

                if(!Read<S>(str_len))return(-2);

                if(!ReadArray<T>(str,(const int64)str_len))return(-3);

                str[str_len]=0;

                return str_len;
            }

            template<typename T,typename S> int _ReadString(String<T> &str)
            {
                S str_len;

                if(!Read<S>(str_len))return(-2);

                str.SetString((T *)pointer,str_len);

                pointer+=str_len*sizeof(T);

                return(str_len);
            }

        public:

            template<typename T> int ReadTinyString (T *str){return _ReadString<T,uint8>(str);}
            template<typename T> int ReadShortString(T *str){return _ReadString<T,uint16>(str);}
            template<typename T> int ReadString     (T *str){return _ReadString<T,uint32>(str);}

            template<typename T> int ReadTinyString (hgl::String<T> &str){return _ReadString<T,uint8>(str);}
            template<typename T> int ReadShortString(hgl::String<T> &str){return _ReadString<T,uint16>(str);}
            template<typename T> int ReadString     (hgl::String<T> &str){return _ReadString<T,uint32>(str);}

            int64 GetSize()const{return size;}
            int64 GetRemain()const{return size-(pointer-(uint8 *)buffer);}

            int64 Tell()const{return pointer-(uint8 *)buffer;}
            int64 Seek(const int64 offset,const SeekOrigin so=SeekOrigin::Begin)
            {
                int64 new_offset;

                switch(so)
                {
                    case SeekOrigin::Begin:     new_offset=offset;break;
                    case SeekOrigin::Current:   new_offset=pointer-(uint8 *)buffer+offset;break;
                    case SeekOrigin::End:       new_offset=size+offset;break;
                    default:return(-1);
                }

                if(new_offset>size)
                    return(-1);

                pointer=(uint8 *)buffer+new_offset;

                return(new_offset);
            }

            int64 Skip(const int64 offset)
            {
                if(offset>size)
                    return(-1);

                pointer+=offset;

                return(offset);
            }

            int64 Restart()
            {
                pointer=(uint8 *)buffer;
            }

            const void *CurPointer()const{return pointer;}
        };//class ConstBufferReader
    }//namespace io
}//namespace hgl
#endif//HGL_IO_CONST_BUFFER_READER_INCLUDE
