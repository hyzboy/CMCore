#ifndef HGL_LOAD_STRING_INCLUDE
#define HGL_LOAD_STRING_INCLUDE

#include<hgl/type/String.h>
#include<hgl/CodePage.h>

namespace hgl
{
    int LoadStringFromText(UTF8String &full_text,const void *source_data,const int size,const CharSet &cs);             ///<加载一个原始文本块到UTF8String
    int LoadStringFromText(UTF16String &full_text,const void *source_data,const int size,const CharSet &cs);            ///<加载一个原始文本块到UTF16String
    int LoadStringFromTextFile(UTF8String &str,const OSString &filename,const CharSet &cs);                             ///<加载一个原始文本文件到UTF8String
    int LoadStringFromTextFile(UTF16String &str,const OSString &filename,const CharSet &cs);                            ///<加载一个原始文本文件到UTF16String
}//namespace hgl
#endif//HGL_LOAD_STRING_INCLUDE
