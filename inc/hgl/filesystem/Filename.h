#ifndef HGL_FILESYSTEM_FILENAME_INCLUDE
#define HGL_FILESYSTEM_FILENAME_INCLUDE

#include<hgl/type/StringList.h>
namespace hgl
{
    namespace filesystem
    {
        /**
        * 组合文件名.<Br>
        * 根据离散的每一级目录名称和最终名称合成完整文件名
        */
        template<typename T>
        inline const String<T> ComboFilename(const StringList<String<T>> &sl,const T spear_char=(T)HGL_DIRECTORY_SEPARATOR_RAWCHAR)
        {
            T *fullname=nullptr;

            {
                int total=0;

                for(auto str:sl)
                    total+=str->Length();

                total+=sl.GetCount();
                ++total;

                fullname=new T[total+1];
            }

            T *p=fullname;
            const T *tmp;
            int len;
            bool first=true;

            for(auto str:sl)
            {
                len=str->Length();

                tmp=trim<T>(str->c_str(),len,isslash<T>);

                if(!first)
                {
                    *p=spear_char;
                    ++p;
                }
                else
                {
                    first=false;
                }

                hgl_cpy<T>(p,tmp,len);
                p+=len;
            }

            *p=0;

            return String<T>::newOf(fullname,p-fullname);
        }

        /**
        * 合成文件名<br>
        * 根据路径名和文件名
        */
        template<typename T>
        inline String<T> MergeFilename(const String<T> &pathname,const String<T> &filename,const T directory_separator_char,const T *directory_separator_str)
        {
            String<T> fullname;

            if(pathname.GetEndChar()==directory_separator_char)                 //结尾有分隔符
            {
                if(filename.GetBeginChar()==directory_separator_char)           //开头有分隔符
                {
                    fullname.SetString(pathname.c_str(),pathname.Length()-1);   //少取一个字符
                }
                else
                {
                    fullname=pathname;
                }
            }
            else                                                                //结尾没有分隔符
            {
                fullname=pathname;

                if(filename.GetBeginChar()!=directory_separator_char)           //开头没有分隔符
                {
                    fullname.Strcat(directory_separator_str);                   //添加分隔符
                }
            }

            fullname.Strcat(filename);
            return fullname;
        }

        /**
        * 截取完整路径中的文件名
        * @param fullname 完整路径文件名
        */
        template<typename T>
        inline String<T> ClipFilename(const String<T> &fullname)
        {
            if(fullname.Length()<=1)
                return(String<T>());

            const T spear_char[] = { '/','\\',0 };

            const int pos=fullname.FindRightChars(spear_char);

            if(pos==-1)
                return String<T>(fullname);

            return fullname.SubString(pos+1);
        }

        /**
        * 截取一个文件名中的主名称
        * @param filename 文件名
        * @param split_char 扩展名分隔符,一般为'.'
        */
        template<typename T>
        inline String<T> ClipFileMainname(const String<T> &filename,const T split_char='.')
        {
            if(filename.Length()<=1)
                return(String<T>());

            const T spear_char[] = { '/','\\',0 };

            const int dot=filename.FindRightChar(split_char);
            const int pos=filename.FindRightChars(spear_char);

            if(dot==-1)
            {
                if(pos==-1)
                    return String<T>(filename);
                else
                    return filename.SubString(pos+1);
            }
            else
            {
                if(pos==-1)
                    return filename.SubString(0,dot);
                else
                    return filename.SubString(pos+1,dot-pos-1);
            }
        }

        /**
        * 截取完整文件名中的扩展名
        * @param fullname 完整文件名
        * @param include_dot 是否包括点
        */
        template<typename T>
        inline String<T> ClipFileExtName(const String<T> &fullname,bool include_dot=true)
        {
            int end=fullname.FindChar(T('?'));         //url的文件名，以?为结束

            if(end==-1)
                end=fullname.Length();

            int pos=fullname.FindRightChar(fullname.Length()-end,T('.'));

            if(pos==-1)
                return String<T>();

            return include_dot? fullname.SubString(pos,     end- pos   ):
                                fullname.SubString(pos+1,   end-(pos+1));
        }

        /**
        * 清除完整文件名中的扩展名
        * @param fullname 完整文件名
        * @param include_dot 是否包括点也清除
        */
        template<typename T>
        inline String<T> TrimFileExtName(const String<T> &fullname,bool include_dot=false)
        {
            int end=fullname.FindChar(T('?'));         //url的文件名，以?为结束

            if(end==-1)
                end=fullname.Length();

            int pos=fullname.FindRightChar(fullname.Length()-end,T('.'));

            if(pos==-1)
                return String<T>();

            return include_dot? fullname.SubString(0,pos):
                                fullname.SubString(0,pos+1);
        }

        /**
        * 截取一个文件名中的路径名
        * @param filename 文件名
        * @param include_spear_char 是否包含最后的分隔符
        */
        template<typename T>
        inline String<T> ClipPathname(const String<T> &filename,bool include_spear_char=true)
        {
            if(filename.Length()<=1)
                return(String<T>());

            const T spear_char[] = { '/','\\',':',0};

            const int pos=filename.FindRightChars(spear_char);

            if(pos==-1)
                return filename;
            else
            if(include_spear_char)
                return filename.SubString(0,pos);
            else
                return filename.SubString(0,pos-1);
        }

        /**
        * 截取路径最后一个名字
        */
        template<typename T>
        inline String<T> ClipLastPathname(const String<T> &fullname)
        {
            if(fullname.Length()<=1)
                return(String<T>());

            const T gap_char[]={'\\','/',0};

            T *p=nullptr;

            T *s=fullname.c_str();
            T *e=fullname.c_str()+fullname.Length()-1;

            while(e>s)
            {
                if(!p)
                {
                    if(*e==gap_char[0]||*e==gap_char[1])
                    {
                        --e;
                        continue;
                    }

                    p=e;
                    --e;
                }
                else
                {
                    if(*e==gap_char[0]||*e==gap_char[1])
                    {
                        return String<T>(e+1,p-e);
                    }

                    --e;
                }
            }

            return(String<T>());
        }

        inline UTF8String MergeFilename(const UTF8String &pathname,const UTF8String &filename)          ///<组合路径名与文件名
        {return MergeFilename<u8char>(pathname,filename,HGL_DIRECTORY_SEPARATOR,HGL_DIRECTORY_SEPARATOR_U8STR);}

        inline WideString MergeFilename(const WideString &pathname,const WideString &filename)          ///<组合路径名与文件名
        {return MergeFilename<wchar_t>(pathname,filename,L'\\',L"\\");}

        OSString FixFilename(const OSString &filename);                                                 ///<修正部分文件名问题
    }//namespace filesystem
}//namespace hgl
#endif//HGL_FILESYSTEM_FILENAME_INCLUDE
