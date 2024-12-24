#include<hgl/io/LoadString.h>
#include<hgl/filesystem/FileSystem.h>
namespace hgl
{
    /**
     * 加载一个原始文本块到UTF8String
     */
    int LoadStringFromText(UTF8String &full_text,const void *source_data,const int size,const CharSet &cs)
    {
        if(!source_data||size<=0)
            return 0;

        u8char *str=nullptr;
        int char_count=0;

        const uint8 *data=(const uint8 *)source_data;

        if(size>=3&&data[0]==0xEF&&data[1]==0xBB&&data[2]==0xBF)            //utf8
        {
            full_text.fromString((u8char *)(data+3),size-3);
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

            full_text.fromString(str,char_count);
            delete[] str;
        }

        return char_count;
    }

    /**
     * 加载一个原始文本块到UTF16String
     */
    int LoadStringFromText(UTF16String &full_text,const void *source_data,const int size,const CharSet &cs)
    {
        if(!source_data||size<=0)
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
            full_text.fromString(str,char_count);
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

            full_text.fromString(str,char_count);

            delete[] str;
        }

        return char_count;
    }
    
    /**
     * 加载一个原始文本文件到UTF8String
     */
    int LoadStringFromTextFile(UTF8String &str,const OSString &filename,const CharSet &cs)
    {
        uchar *data;

        const int size=filesystem::LoadFileToMemory(filename,(void **)&data);

        if(size<=0)
            return size;

        return LoadStringFromText(str,data,size,cs);
    }

    /**
     * 加载一个原始文本文件到UTF16String
     */
    int LoadStringFromTextFile(UTF16String &str,const OSString &filename,const CharSet &cs)
    {
        uchar *data;

        const int size=filesystem::LoadFileToMemory(filename,(void **)&data);

        if(size<=0)
            return size;

        return LoadStringFromText(str,data,size,cs);
    }
}//namespace hgl
