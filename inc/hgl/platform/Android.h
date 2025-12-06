#pragma once

namespace hgl
{
    struct AndroidVersion
    {
        unsigned int major;
        unsigned int minor;
        unsigned int patch;

        char postfix;
    };

    /**
    * 根据版本号取得Android的APILevel,如若出错返回-1
    */
    const int GetAndroidAPILevel(const AndroidVersion &);

    constexpr const char *android_api_version_string[]=
    {
        "0",        //0
        "1.0",      //1
        "1.1",      //2
        "1.5",      //3
        "1.6",      //4
        "2.0",      //5
        "2.0.1",    //6
        "2.1",      //7
        "2.2",      //8
        "2.3",      //9
        "2.3.3",    //10
        "3.0",      //11
        "3.1",      //12
        "3.2",      //13
        "4.0",      //14
        "4.0.3",    //15
        "4.1",      //16
        "4.2",      //17
        "4.3",      //18
        "4.4",      //19
        "4.4W",     //20
        "5.0",      //21
        "5.1",      //22
        "6.0",      //23
        "7.0",      //24
        "7.1",      //25
        "8.0",      //26
        "8.1",      //27
        "9",        //28
        "10",       //29
        "11",       //30
        "12",       //31
        "12L",      //32
        "13",       //33
        "14",       //34
    };
}//namespace hgl
