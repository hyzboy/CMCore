#ifndef HGL_LOAD_STRING_INCLUDE
#define HGL_LOAD_STRING_INCLUDE

#include<hgl/type/String.h>
#include<hgl/CodePage.h>
#include<string>

namespace hgl
{
    int LoadStringFromText(U8String &full_text,const void *source_data,const int size,const CharSet &cs=UTF8CharSet);             ///<加载一个原始文本块到U8String
    int LoadStringFromText(U16String &full_text,const void *source_data,const int size,const CharSet &cs=UTF8CharSet);            ///<加载一个原始文本块到U16String
    int LoadStringFromTextFile(U8String &str,const OSString &filename,const CharSet &cs=UTF8CharSet);                             ///<加载一个原始文本文件到U8String
    int LoadStringFromTextFile(U16String &str,const OSString &filename,const CharSet &cs=UTF8CharSet);                            ///<加载一个原始文本文件到U16String

    int LoadStringFromTextFile(std::string &str,const OSString &filename,const CharSet &cs=UTF8CharSet);                             ///<加载一个原始文本文件到U8String
    int LoadStringFromTextFile(std::wstring &str,const OSString &filename,const CharSet &cs=UTF8CharSet);                            ///<加载一个原始文本文件到U16String
}//namespace hgl
#endif//HGL_LOAD_STRING_INCLUDE
