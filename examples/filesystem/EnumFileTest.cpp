#include<hgl/filesystem/EnumFile.h>
#include<iostream>

using namespace hgl;
using namespace hgl::filesystem;

class MyEnumFile:public EnumFile
{
public:

    void ProcFolderBegin(struct EnumFileConfig *parent_efc,struct EnumFileConfig *cur_efc,FileInfo &fi) override
    {
        os_out<<OS_TEXT("[Folder] ")<<fi.fullname<<std::endl;
    }

    void ProcFile(struct EnumFileConfig *,FileInfo &fi) override
    {
        os_out<<OS_TEXT("[File] ")<<fi.fullname<<std::endl;
    }
};//class EnumFile

int os_main(int argc,os_char **argv)
{
    os_out<<OS_TEXT("EnumFile test")<<std::endl<<std::endl;

    if(argc!=2)
    {
        os_out<<OS_TEXT("example: EnumFileTest [path]")<<std::endl;
        return(0);
    }

    os_out<<OS_TEXT("enum folder: ")<<argv[1]<<std::endl;

    EnumFileConfig efc(argv[1]);
    MyEnumFile ef;

    efc.sub_folder=true;

    ef.Enum(&efc);
    return 0;
}
