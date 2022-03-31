#include <hgl/filesystem/FileSystem.h>
#include <hgl/log/LogInfo.h>
#include <hgl/io/FileInputStream.h>
#include <hgl/io/FileOutputStream.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

namespace hgl
{
    namespace filesystem
    {
        /**
         * 修正部分文件名问题(目前仅处理错误斜杠和重复斜杠问题)
         */
        OSString FixFilename(const OSString &filename)
        {
            int old_len=filename.Length();
            int new_len;

            os_char *new_string=new os_char[old_len+1];

            os_char *sp=filename.c_str();
            os_char *tp=new_string;
            os_char last_char=0;
            bool fix=false;

            while(*sp)
            {
                if(*sp==HGL_INCORRECT_DIRECTORY_SEPARATOR)
                {
                    if(last_char==HGL_INCORRECT_DIRECTORY_SEPARATOR)
                    {
                        fix=true;
                        ++sp;
                        continue;
                    }
                    else
                    {
                        last_char=*sp;
                        fix=true;
                        ++sp;
                        *tp=HGL_DIRECTORY_SEPARATOR;
                    }
                }
                else
                {
                    if(*sp==HGL_DIRECTORY_SEPARATOR
                     &&last_char==HGL_DIRECTORY_SEPARATOR)
                    {
                        last_char=*sp;
                        fix=true;

                        ++sp;
                        continue;
                    }
                    else
                    {
                        last_char=*sp;

                        *tp=*sp;
                        ++sp;
                    }
                }

                ++tp;
            }

            *tp=0;

            new_len=tp-new_string;

            if(!fix)
            {
                delete[] new_string;
                return filename;
            }

            return OSString::newOf(new_string,new_len);
        }

        /**
        * 比较两个文件是否一样
        * @param filename1 第一个文件的名称
        * @param filename2 第二个文件的名称
        * @param buf_size
        * @return 文件是否一样
        */
        bool FileComp(const OSString &filename1,const OSString &filename2,const size_t buf_size)
        {
            io::FileInputStream fp1,fp2;
            int64 fs1,fs2;

            if(!fp1.Open(filename1))return(false);
            if(!fp2.Open(filename2))return(false);

            fs1=fp1.GetSize();
            fs2=fp2.GetSize();

            if(fs1!=fs2)
                return(false);

            int64 pos=0,size;
            AutoDeleteArray<char> data1(buf_size);
            AutoDeleteArray<char> data2(buf_size);

            while(pos<fs1)
            {
                size=buf_size;
                if(pos+size>fs1)size=fs1-pos;

                fp1.Read(data1,size);
                fp2.Read(data2,size);

                if(memcmp(data1,data2,size))
                    return(false);

                pos+=size;
            }

            return(true);
        }
        
        void *LoadFileToMemory(const OSString &filename,int64 &size,bool append_zero)
        {
            io::FileInputStream fs;

            if(!fs.Open(filename))
                return(nullptr);

            size=fs.GetSize();

            char *fb=new char[append_zero?size+1:size];

            if(fs.Read(fb,size)==size)
            {
                if(append_zero)
                    fb[size]=0;

                return fb;
            }

            delete[] fb;
            return(nullptr);
        }

        /**
        * 加载一个文件到内存,文件数据请自己delete[]掉
        * @param filename 要载入的文件名称
        * @param buf 用来保存数据的内存指针的指针
        * @return 文件长度
        */
        int64 LoadFileToMemory(const OSString &filename,void **buf,bool append_zero)
        {
            int64 size;

            *buf=LoadFileToMemory(filename,size,append_zero);

            return size;
        }

        /**
        * 保存一块内存成文件
        * @param filename 要保存的文件名称
        * @param buf 要保存的内存指针
        * @param size 要保存的内存数据长度
        * @return 成功写入的字节数
        * @return -1 失败
        */
        int64 SaveMemoryToFile(const OSString &filename,const void *buf,const int64 &size)
        {
            io::FileOutputStream fs;

            if(!fs.CreateTrunc(filename))
                return(-1);

            return fs.Write(buf,size);
        }

        /**
        * 保存多块内存成文件
        * @param filename 要保存的文件名称
        * @param buf_list 要保存的内存块指针列表
        * @param buf_size 要保存的内存块数据长度列表
        * @param buf_count 要保存的内存块数量
        * @return 成功写入的字节数
        * @return -1 失败
        */
        int64 SaveMemoryToFile(const OSString &filename,void **buf_list,const int64 *buf_size,const int &buf_count)
        {
            io::FileOutputStream fs;

            if(!fs.CreateTrunc(filename))
                return(-1);

            int64 total=0;
            int64 result;

            for(int i=0;i<buf_count;i++)
            {
                result=fs.Write(buf_list[i],buf_size[i]);

                if(result!=buf_size[i])
                    return -1;

                total+=result;
            }

            return total;
        }

        os_char *GetRootPath(os_char *str);
        bool MakeDirectory(const os_char *name);

        /**
        * 创建一个子目录,该函数可自动创建多级目录。
        * @param dirname 目录名称
        * @return 目录是否创建成功
        */
        bool MakePath(const OSString &dirname)
        {
            constexpr os_char directory_separator=HGL_DIRECTORY_SEPARATOR;
            os_char *p;

            os_char str[HGL_MAX_PATH];
            os_char *sp;

            FileInfo fi;

            strcpy(str,HGL_MAX_PATH,dirname.c_str());

            sp=GetRootPath(str);

            while(1)
            {
                p=hgl::strchr(sp,directory_separator);

                if(p)
                    *p=0;

                if(*sp==0)
                    return(true);

                if(!GetFileInfo(str,fi))    //没有找到
                {
                    if(!MakeDirectory(str))
                        return(false);
                }
                else
                {
                    if(!fi.is_directory)        //不是目录
#if HGL_OS != HGL_OS_Windows
                        if(!fi.is_link)         //还不是链接
#endif//HGL_OS != HGL_OS_Windows
                            return(false);
                }

                if(p)
                    *p++=directory_separator;
                else
                    return(true);

                while(*p==directory_separator)  //跳过那种连续的分隔符,比如C:\\Windows
                    ++p;

                sp=p;
            }
        }

        /**
        * 删除一个子目录,包含所有文件和子目录
        * @param dirname 目录名称
        * @return 目录是否删除成功
        */
    //     void DeleteTree(const OSString &dirname)
    //     {
    //     }

        /**
        * 读取文件的一部分到内存
        * @param filename 文件名
        * @param offset 起始地址
        * @param length 读取长度
        * @param buf 数据存放地址,为NULL表示要求函数分配
        * @return NULL 读取失败
        * @return 读取出来的数据存放的地址
        */
        void *LoadFileToMemory(const OSString &filename,int64 offset,void *buf,int64 length)
        {
            if(!filename||!*filename||length==0)
                return(nullptr);

            io::FileInputStream fs;

            if(!fs.Open(filename))
                return(nullptr);

            const int64 file_length=fs.GetSize();

            if(offset+length>file_length)
            {
                LOG_PROBLEM(OS_TEXT("读取文件<")+filename+OS_TEXT("><")+OSString::valueOf(offset)+OS_TEXT(",")+OSString::valueOf(length)+OS_TEXT(">超出了范围，文件长度为<")+OSString::valueOf(file_length));
                return(nullptr);
            }

            char *fb;

            if(buf)
                fb=(char *)buf;
            else
                fb=new char[length];

            if(fs.Read(offset,fb,length)==length)
            {
                LOG_INFO(OS_TEXT("加载文件<")+filename+OS_TEXT("><")+OSString::valueOf(offset)+OS_TEXT(",")+OSString::valueOf(length)+OS_TEXT(">到缓冲区成功."));

                return(buf);
            }
            else
            {
                if(fb!=buf)
                    delete[] fb;

                return(nullptr);
            }
        }

        /**
        * 保存内存中的数据到文件中
        * @param filename 文件名
        * @param offset 起始地址
        * @param length 数据长度
        * @param data 数据长度
        * @return 是否成功
        */
        bool SaveMemoryToFile(const OSString &filename,int64 offset,const void *data,int64 length)
        {
            io::FileOutputStream fs;

            if(!fs.Open(filename))
                return(false);

            return fs.Write(offset,data,length);
        }

        bool GetFileInfo(const os_char *filename,struct FileInfo &fi)
        {
            if(!filename||!*filename)
                return(false);

            struct_stat64 file_state;

            hgl_zero(file_state);

            if(hgl_lstat64(filename,&file_state)==-1)
                return(false);

            hgl_zero(fi);

            if(file_state.st_mode&S_IFREG)
                fi.is_file=true;

            if(file_state.st_mode&S_IFDIR)
                fi.is_directory=true;

            if (file_state.st_mode&S_IREAD)
                fi.can_read = true;

            if (file_state.st_mode&S_IWRITE)
                fi.can_write = true;

#if HGL_OS != HGL_OS_Windows
            if(file_state.st_mode&S_IFLNK)
                fi.is_link=true;
#endif//HGL_OS != HGL_OS_Windows

            fi.size=file_state.st_size;

            return(true);
        }

        /**
         * 在多个目录内查找一个文件
         * @param filename 要查找的文件名称
         * @param paths 要查找的目录
         * @param user_data 用户自定义数据
         * @param ff 查找响应事件函数
         */
        const uint FindFileOnPaths(const OSString &filename,const OSStringList &paths,void *user_data,OnFindedFileFUNC ff)
        {
            if(filename.IsEmpty()||paths.GetCount()<=0)return(0);
            if(ff==nullptr)return 0;

            uint count=0;
            bool exist;
            OSString full_filename;

            for(const OSString *pn:paths)
            {
                full_filename=MergeFilename(*pn,filename);

                exist=FileExist(full_filename);

                if(exist)
                    ++count;

                if(!ff(full_filename,user_data,exist))
                    return(count);
            }

            return count;
        }
        
        /**
         * 在多个目录内查找一个文件，这个文件可能有多个文件名
         * @param filenames 要查找的文件名称
         * @param paths 要查找的目录
         * @param user_data 用户自定义数据
         * @param ff 查找响应事件函数
         */
        const uint FindFileOnPaths(const OSStringList &filenames,const OSStringList &paths,void *user_data,OnFindedFileFUNC ff)
        {
            if(filenames.GetCount()<=0||paths.GetCount()<=0)return(0);
            if(ff==nullptr)return 0;

            uint count=0;
            bool exist;
            OSString full_filename;

            for(const OSString *pn:paths)
            {
                for(const OSString *fn:filenames)
                {
                    full_filename=MergeFilename(*pn,*fn);

                    exist=FileExist(full_filename);

                    if(exist)
                        ++count;

                    if(!ff(full_filename,user_data,exist))
                        return(count);
                }
            }

            return count;
        }
    }//namespace filesystem
}//namespace hgl
