﻿#ifndef HGL_IO_FILE_OUTPUT_STREAM_INCLUDE
#define HGL_IO_FILE_OUTPUT_STREAM_INCLUDE

#include<hgl/io/OutputStream.h>
#include<hgl/io/FileAccess.h>
namespace hgl
{
    namespace io
    {
        /**
        * 文件输出流类
        */
        class FileOutputStream:public OutputStream                                                  ///文件输出流类
        {
        private:

            SharedPtr<FileAccess> file;                                                             ///<文件访问指针

            virtual bool OpenFile(const OSString &,FileOpenMode);

        public:

            FileOutputStream();
            FileOutputStream(SharedPtr<FileAccess> &);
            virtual ~FileOutputStream();

            virtual bool    Open        (const OSString &fn,FileOpenMode mode){return OpenFile(fn,mode);}       ///<打开文件，指定一个模式
            virtual bool    Open        (const OSString &fn){return OpenFile(fn,FileOpenMode::OnlyWrite);}      ///<打开文件
            virtual bool    Create      (const OSString &fn){return OpenFile(fn,FileOpenMode::Create);}         ///<创建文件，如存在创建失败
            virtual bool    CreateTrunc (const OSString &fn){return OpenFile(fn,FileOpenMode::CreateTrunc);}    ///<创建文件，如存在则抹去
            virtual bool    OpenAppend  (const OSString &fn){return OpenFile(fn,FileOpenMode::Append);}         ///<打开文件，追加模式

            virtual void    Close();                                                                ///<关闭文件

            virtual int64   Write(const void *,int64);                                              ///<写入数据

            virtual bool    CanSeek()const;                                                         ///<是否可移动访问指针
            virtual bool    CanRestart()const;                                                      ///<是否可复位访问
            virtual bool    CanSize()const;                                                         ///<是否可访问文件长度

            virtual int64   Tell()const;                                                            ///<取当前位置
            virtual int64   GetSize()const;                                                         ///<取得文件长度
            virtual bool    Restart();                                                              ///<复位访问指针
            virtual int64   Seek(int64,SeekOrigin=SeekOrigin::Begin);                               ///<移动访问指针
            virtual int64   Available()const{return -1;}                                            ///<可不受影响写入的字节数(此值无法估算，所以一般情况下返回-1)

            virtual int64   Write(int64,const void *,int64);                                        ///<在指定位置写入指定长度的数据
        };//class FileOutputStream

        /**
         * 打开一个文件输出流
         */
        class OpenFileOutputStream
        {
            FileOutputStream *fos;

        public:

            OpenFileOutputStream()
            {
                fos=nullptr;
            }

            /**
             * 打开一个文件输出流构造函数
             * @param filename 文件名
             * @param mode 打开模式，默认只写(必然可读)
             * @see FileOpenMode
             */
            OpenFileOutputStream(const OSString &filename,FileOpenMode mode=FileOpenMode::OnlyWrite)
            {
                fos=new FileOutputStream();

                if(!fos->Open(filename,mode))
                {
                    delete fos;
                    fos=nullptr;
                }
            }

            ~OpenFileOutputStream()
            {
                SAFE_CLEAR(fos);
            }

            const bool operator !(){return !fos;}
            operator FileOutputStream *(){return fos;}
            FileOutputStream *operator &(){return fos;}
            FileOutputStream *operator ->(){return fos;}
        };//class OpenFileInputStream

        inline FileOutputStream *CreateFileOutputStream(const OSString &filename,FileOpenMode fom=FileOpenMode::CreateTrunc)///<创建一个文件输出流
        {
            auto *fos=new FileOutputStream();

            if(!fos->Open(filename,fom))
            {
                delete fos;
                return(nullptr);
            }

            return fos;
        }
    }//namespace io
}//namespace hgl
#endif//HGL_IO_FILE_OUTPUT_STREAM_INCLUDE
