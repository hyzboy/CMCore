#pragma once

#include<hgl/platform/Platform.h>
#include<hgl/io/SeekOrigin.h>
namespace hgl
{
    namespace io
    {
        /**
        * 定位访问功能基类
        */
        class SeekAccess
        {
        public:

            virtual ~SeekAccess()=default;

            virtual bool    CanRestart()const=0;                                                    ///<是否可以复位
            virtual bool    CanSeek()const=0;                                                       ///<是否可以定位
            virtual bool    CanSize()const=0;                                                       ///<是否可以取得尺寸

            virtual bool    Restart()=0;                                                            ///<复位访问指针
            virtual int64   Seek(int64,SeekOrigin=SeekOrigin::Begin)=0;                             ///<移动访问指针
            virtual int64   Tell()const=0;                                                          ///<返回当前访问位置
            virtual int64   GetSize()const=0;                                                       ///<取得文件长度
        };//class SeekAccess
    }//namespace io
}//namespace hgl
