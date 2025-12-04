#pragma once

#include<hgl/type/StrChar.h>
#include<hgl/io/SeekAccess.h>
namespace hgl
{
    namespace io
    {
        /**
        * 数据输入流基类
        */
        class InputStream:public SeekAccess                                                         ///数据输入流基类
        {
        public:

            virtual ~InputStream()=default;

            virtual void    Close()=0;                                                              ///<关闭输入流

            virtual int64   Read(void *,int64)=0;                                                   ///<读取数据
            virtual int64   Peek(void *,int64)=0;                                                   ///<预览数据
            virtual int64   ReadFully(void *buf,int64 buf_size){return Read(buf,buf_size);}         ///<充分读取,保证读取到指定长度的数据(不计算超时)

            virtual bool    CanRestart()const=0;                                                    ///<是否可以复位
            virtual bool    CanSeek()const=0;                                                       ///<是否可以定位
            virtual bool    CanSize()const=0;                                                       ///<是否可以取得尺寸
            virtual bool    CanPeek()const=0;                                                       ///<是否可以预览数据

            virtual bool    Restart()=0;                                                            ///<复位访问指针
            virtual int64   Skip(int64)=0;                                                          ///<跳过指定字节不访问
            virtual int64   Seek(int64,SeekOrigin=SeekOrigin::Begin)=0;                             ///<移动访问指针
            virtual int64   Tell()const=0;                                                          ///<返回当前访问位置
            virtual int64   GetSize()const=0;                                                       ///<取得流长度
            virtual int64   Available()const=0;                                                     ///<剩下的可以不受阻塞访问的字节数
        };//class InputStream

        int64 LoadFromInputStream(void *buf,int64 max_size,InputStream *is);                        ///<从输入流中加载指定最大长度的数据
        void *LoadFromInputStream(int64 *size,InputStream *is);                                     ///<从输入流中加载所有的数据到一块内存
    }//namespace io
}//namespace hgl
