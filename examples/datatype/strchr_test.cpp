#include<hgl/type/String.h>
#include<hgl/filesystem/FileSystem.h>
#include<iostream>

void out(const char *source,const char *result)
{
    std::cout<<source<<" return \""<<result<<"\""<<std::endl;
}

void out(const char *source,const hgl::AnsiString &param,const hgl::AnsiString &result)
{
    std::cout<<source<<"(\""<<param.c_str()<<"\") return \""<<result.c_str()<<"\""<<std::endl;
}

int main(int,char **)
{
    constexpr char str[]="hello,world!";

    std::cout<<"str: \""<<str<<"\""<<std::endl;

    out("strchr(str,',')",hgl::strchr(str,','));
    out("strrchr(str,strlen(str),',')",hgl::strrchr(str,hgl::strlen(str),','));

    out("strchr(str,\"el\",2)",hgl::strchr(str,"el",2));
    out("strrchr(str,strlen(str),\"el\",2)",hgl::strrchr(str,hgl::strlen(str),"el",2));

    out("strrchr(str,strlen(str),3,'l')",hgl::strrchr(str,hgl::strlen(str),3,'l'));
    out("strrchr(str,strlen(str),3,\"el\",2)",hgl::strrchr(str,hgl::strlen(str),3,"el",2));

    hgl::AnsiString fn("C:\\1.txt");

    out("ClipFilename",fn,hgl::filesystem::GetFilename(fn));

    hgl::AnsiString tp1("C:\\1\\2");
    hgl::AnsiString tp2("C:\\1\\2\\");
    hgl::AnsiString tp3("C:\\1\\2\\\\");

    out("ClipLastPathname",tp1,hgl::filesystem::GetLastPathComponent(tp1));
    out("ClipLastPathname",tp2,hgl::filesystem::GetLastPathComponent(tp2));
    out("ClipLastPathname",tp3,hgl::filesystem::GetLastPathComponent(tp3));

    return 0;
}
