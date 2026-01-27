#include<hgl/io/LoadString.h>
#include<hgl/filesystem/FileSystem.h>
#include<string>
namespace hgl
{
    namespace
    {
        template<typename CH> struct MakeString
        {
            virtual void set(CH *str,const int count)=0;
        };

        template<typename CH,typename STR> struct MakeStringImpl:public MakeString<CH>
        {
            STR *hgl_string;

            MakeStringImpl(STR *s)
            {
                hgl_string=s;
            }

            void set(CH *str,const int count) override
            {
                hgl_string->fromString(str,count);
            }
        };

        template<typename CH,typename STR> struct MakeStringSTD:public MakeString<CH>
        {
            STR *std_string;

            MakeStringSTD(STR *s)
            {
                std_string=s;
            }

            void set(CH *str,const int count) override
            {
                std_string->assign(str,count);
            }
        };

        using MakeAString   =MakeStringImpl<char,   AnsiString>;
        using MakeU8String  =MakeStringImpl<u8char, U8String>;
        using MakeU16String =MakeStringImpl<u16char,U16String>;
        using MakeWString   =MakeStringImpl<wchar_t,WideString>;

        using MakeStdString     =MakeStringSTD<char,    std::string>;
        using MakeStdU8String   =MakeStringSTD<u8char,  std::u8string>;
        using MakeStdU16String  =MakeStringSTD<u16char, std::u16string>;
        using MakeStdWString    =MakeStringSTD<wchar_t, std::wstring>;
    }

    /**
     * 加载一个原始文本块到U8String
     */
    int LoadStringFromText(MakeString<u8char> *make_string,const void *source_data,const int size,const CharSet &cs)
    {
        if(!make_string||!source_data||size<=0)
            return 0;

        u8char *str=nullptr;
        int char_count=0;

        const uint8 *data=(const uint8 *)source_data;

        if(size>=3&&data[0]==0xEF&&data[1]==0xBB&&data[2]==0xBF)            //utf8
        {
            make_string->set((u8char *)(data+3),size-3);
            char_count=size-3;
        }
        else
        {
            if(size>=2)
            {
                int u16_count;

                if(data[0]==0xFF&&data[1]==0xFE)                                //LE
                {
                    if(size>=4&&data[2]==0&&data[3]==0)                         //utf32-le
                    {
                        u16_count=(size-4)>>2;
                        u16char *u16str=new u16char[u16_count];

                        LittleToCurrentEndian(u16str,(uint32 *)(data+4),u16_count);

                        str=u16_to_u8(u16str,u16_count,char_count);
                        delete[] u16str;
                    }
                    else                                                        //utf16-le
                    {
                        u16_count=(size-2)>>1;
                        LittleToCurrentEndian((u16char *)(data+2),u16_count);
                        str=u16_to_u8((u16char *)(data+2),u16_count,char_count);
                    }
                }
                else
                if(data[0]==0&&data[1]==0&&data[2]==0xFE&&data[3]==0xFF)        //utf32-be
                {
                    u16_count=(size-4)>>2;
                    u16char *u16str=new u16char[u16_count];

                    BigToCurrentEndian(u16str,(uint32 *)(data+4),u16_count);

                    str=u16_to_u8(u16str,u16_count,char_count);
                    delete[] u16str;
                }
                else
                if(data[0]==0xFE&&data[1]==0xFF)                                //utf16-be
                {
                    u16_count=(size-2)>>1;
                    BigToCurrentEndian((u16char *)(data+2),u16_count);
                    str=u16_to_u8((u16char *)(data+2),u16_count,char_count);
                }
            }

            if(!str)
#ifdef __ANDROID__
            {
                delete[] data;
                return 0;
            }
#else
                char_count=to_utf8(cs,&str,(char *)data,size);
#endif//

            make_string->set(str,char_count);
            delete[] str;
        }

        return char_count;
    }

    /**
     * 加载一个原始文本块到U16String
     */
    int LoadStringFromText(MakeString<u16char> *make_string,const void *source_data,const int size,const CharSet &cs)
    {
        if(!make_string||!source_data||size<=0)
            return(0);

        const uint8 *data=(const uint8 *)source_data;
        u16char *str=nullptr;

        int char_count=0;

        if(size>=2)
        {
            if(data[0]==0xFF&&data[1]==0xFE)                                //LE
            {
                if(size>=4&&data[2]==0&&data[3]==0)                         //utf32-le
                {
                    str=(u16char *)data;        //32->16使用原缓冲区覆盖
                    char_count=(size-4)>>2;
                    LittleToCurrentEndian(str,(uint32 *)(data+4),char_count);
                }
                else                                                        //utf16-le
                {
                    str=(u16char *)(data+2);
                    char_count=(size-2)>>1;
                    LittleToCurrentEndian(str,char_count);
                }
            }
            else
            if(data[0]==0&&data[1]==0&&data[2]==0xFE&&data[3]==0xFF)        //utf32-be
            {
                str=(u16char *)data;            ////32->16使用原缓冲区覆盖
                char_count=(size-4)>>2;
                BigToCurrentEndian(str,(uint32 *)(data+4),char_count);
            }
            else
            if(data[0]==0xFE&&data[1]==0xFF)                                //utf16-be
            {
                str=(u16char *)(data+2);
                char_count=(size-2)>>1;
                BigToCurrentEndian(str,char_count);
            }
        }

        if((uchar *)str>=data&&(uchar *)str<=data+size)                      //如果str的地址在data的范围内
        {
            make_string->set(str,char_count);
        }
        else
        {
            if(size>=3&&data[0]==0xEF&&data[1]==0xBB&&data[2]==0xBF)        //utf8
                str=u8_to_u16((u8char *)(data+3),size-3,char_count);
            else
            if(cs==UTF8CharSet)
                str=u8_to_u16((u8char *)data,size,char_count);
            else
            if(cs==UTF16BECharSet||cs==UTF16LECharSet)
            {
                str=(u16char *)data;
                char_count=size>>1;
            }
            else
            {
#ifdef __ANDROID__
                delete[] data;
                return 0;
            }
#else
                char_count=to_utf16(cs,&str,(char *)data,size);
#endif//
            }

            make_string->set(str,char_count);
            delete[] str;
        }

        return char_count;
    }

    int LoadStringFromText(U8String &full_text,const void *source_data,const int size,const CharSet &cs)
    {
        MakeU8String make_string(&full_text);

        return LoadStringFromText(&make_string,source_data,size,cs);
    }

    int LoadStringFromText(U16String &full_text,const void *source_data,const int size,const CharSet &cs)
    {
        MakeU16String make_string(&full_text);

        return LoadStringFromText(&make_string,source_data,size,cs);
    }

    template<typename T>
    int LoadStringFromTextFile(MakeString<T> *make_string,const OSString &filename,const CharSet &cs)
    {
        uchar *data;

        const int size=filesystem::LoadFileToMemory(filename,(void **)&data);

        if(size<=0)
            return size;

        return LoadStringFromText(make_string,data,size,cs);
    }

    int LoadStringFromTextFile(U8String &str,const OSString &filename,const CharSet &cs)
    {
        MakeU8String make_string(&str);

        return LoadStringFromTextFile<u8char>(&make_string,filename,cs);
    }

    int LoadStringFromTextFile(U16String &str,const OSString &filename,const CharSet &cs)
    {
        MakeU16String make_string(&str);

        return LoadStringFromTextFile<u16char>(&make_string,filename,cs);
    }

    int LoadStringFromTextFile(std::string &str,const OSString &filename,const CharSet &cs)
    {
        MakeStdString make_string(&str);

        return LoadStringFromTextFile<u8char>((MakeU8String *)&make_string,filename,cs);
    }

    int LoadStringFromTextFile(std::wstring &str,const OSString &filename,const CharSet &cs)
    {
        MakeStdWString make_string(&str);

        return LoadStringFromTextFile<wchar_t>(&make_string,filename,cs);
    }

}//namespace hgl
