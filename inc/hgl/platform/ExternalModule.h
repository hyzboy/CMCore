﻿#ifndef HGL_EXTERNAL_MODULE_INCLUDE
#define HGL_EXTERNAL_MODULE_INCLUDE

#include<hgl/type/DataType.h>
#include<hgl/log/ObjectLogger.h>
#include<hgl/platform/FuncLoad.h>
#include OS_EXTERNAL_H
namespace hgl
{
    /**
    * 这个类用于对外部动态库的调用,支持Windows/Linux/BSD/MacOSX<br>
    * Windows 下动态库扩展名为.DLL<br>
    * FreeBSD/MacOSX 下动态库扩展名为.dylib<br>
    * Linux 下动态库扩展名为.so
    */
    class ExternalModule                                                                            ///外部模块调用类
    {
        OBJECT_LOGGER

    private:

        ExternalModulePointer fp;

    public: //方法

        ExternalModule();                                                                           ///<本类构造函数
        ExternalModule(ExternalModulePointer);                                                      ///<本类构造函数
        virtual ~ExternalModule();                                                                  ///<本类析构函数

        bool Load(const os_char *);                                                                 ///<加载一个外部模块
        void Clear();                                                                               ///<清除当前模块

        void *FindFunc(const char *);                                                               ///<查找函数
        void *GetFunc(const char *);                                                                ///<取得函数

        int Get(FuncLoad *);                                                                        ///<加载函数列表
    };//class ExternalModule

    ExternalModule *LoadExternalModule(const os_char *);                                            ///<加载一个外部模块
}//namespace hgl
#endif//HGL_EXTERNAL_MODULE_INCLUDE
