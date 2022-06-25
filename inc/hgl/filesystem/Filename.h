#ifndef HGL_FILESYSTEM_FILENAME_INCLUDE
#define HGL_FILESYSTEM_FILENAME_INCLUDE

#include<hgl/type/StringList.h>
#include<hgl/math/PrimaryMathematics.h>

/**
* Maximum Path Length Limitation
* https://docs.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation?tabs=cmd
*/

namespace hgl
{
    namespace filesystem
    {
        /**
        * 组合文件名.<Br>
        * 根据离散的每一级目录名称和最终名称合成完整文件名
        */
        template<typename T>
        inline const String<T> ComboFilename(T **str_list,int *str_len,const int count,const T spear_char=(T)HGL_DIRECTORY_SEPARATOR_RAWCHAR)
        {
            T *fullname=nullptr;

            {
                const int total=sum(str_len,count)+count;

                fullname=new T[total];
            }

            T *p=fullname;
            const T *tmp;
            int len;
            bool first=true;

            for(int i=0;i<count;i++)
            {
                len=str_len[i];

                if(len<=0)continue;
                
                tmp=trim<T>(str_list[i],len,isslash<T>);

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
        * 组合文件名.<Br>
        * 根据离散的每一级目录名称和最终名称合成完整文件名
        */
        template<typename T>
        inline const String<T> ComboFilename(T **str_list,const int count,const T spear_char=(T)HGL_DIRECTORY_SEPARATOR_RAWCHAR)
        {
            int str_len[count];

            for(int i=0;i<count;i++)
                str_len=strlen(str_list[i]);

            return ComboFilename(str_list,str_len,count,spear_char);
        }

        /**
        * 组合文件名.<Br>
        * 根据离散的每一级目录名称和最终名称合成完整文件名
        */
        template<typename T>
        inline const String<T> ComboFilename(const StringList<T> &sl,const T spear_char=(T)HGL_DIRECTORY_SEPARATOR_RAWCHAR)
        {
            T **str_list=AutoDeleteArray<T *>(sl.GetCount());
            int *str_len=AutoDeleteArray<int>(sl.GetCount());

            int index=0;

            for(auto str:sl)
            {
                if(str->IsEmpty())
                    continue;

                str_list[index]=str->c_str();
                str_len[index]=str->Length();

                ++index;
            }

            return ComboFilename(str_list,str_len,sl.GetCount(),spear_char);
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
        * @param clear_dot 是否包括点也清除
        */
        template<typename T>
        inline String<T> TrimFileExtName(const String<T> &fullname,bool clear_dot=false)
        {
            int end=fullname.FindChar(T('?'));         //url的文件名，以?为结束

            if(end==-1)
                end=fullname.Length();

            int pos=fullname.FindRightChar(fullname.Length()-end,T('.'));

            if(pos==-1)
                return String<T>();

            return clear_dot?   fullname.SubString(0,pos):
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

        /**
        * 截取完整路径中的路径名和文件名
        * @param pathname 拆分后的路径名
        * @param filename 拆分后的文件名
        * @param fullname 拆分前的完整路径文件名
        */
        template<typename T>
        inline bool SplitFilename(String<T> &pathname,String<T> &filename,const String<T> &fullname)
        {
            if(fullname.Length()<=1)
                return false;

            const T spear_char[] = { '/','\\' };

            const int pos=fullname.FindRightChars(spear_char);

            if(pos==-1)
                return(false);

            pathname.Strcpy(fullname,pos);
            filename.Strcpy(fullname.c_str()+pos+1);

            return(true);
        }

        template<typename T>
        inline String<T> ReplaceExtName(const String<T> &old_name,const String<T> &new_extname,const T split_char='.')
        {
            if(old_name.Length()<=1)
                return(String<T>::charOf(split_char)+new_extname);

            const int pos=old_name.FindRightChar(split_char);

            if(pos!=-1)
                return old_name.SubString(0,pos)+new_extname;
            else
                return old_name+String<T>::charOf(split_char)+new_extname;
        }

        inline UTF8String MergeFilename(const UTF8String &pathname,const UTF8String &filename)          ///<组合路径名与文件名
        {return MergeFilename<u8char>(pathname,filename,HGL_DIRECTORY_SEPARATOR,HGL_DIRECTORY_SEPARATOR_U8STR);}

        inline WideString MergeFilename(const WideString &pathname,const WideString &filename)          ///<组合路径名与文件名
        {return MergeFilename<wchar_t>(pathname,filename,L'\\',L"\\");}

        OSString FixFilename(const OSString &filename);                                                 ///<修正部分文件名问题
    }//namespace filesystem
}//namespace hgl
#endif//HGL_FILESYSTEM_FILENAME_INCLUDE
