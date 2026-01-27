#include<hgl/filesystem/Filename.h>
#include<iostream>

using namespace std;
using namespace hgl;
using namespace hgl::filesystem;

int os_main(int,os_char **)
{
    constexpr os_char incorrect_filename[]=OS_TEXT("c:/win95\\\\ms-dos");

    os_out<<OS_TEXT("fix filename test that they have a few problems.\n\n");

    OSString correct_filename=NormalizeFilename(incorrect_filename);

    os_out<<OS_TEXT("origin filename: ")<<incorrect_filename<<endl;
    os_out<<OS_TEXT(" fixed filename: ")<< correct_filename.c_str()<<endl;

    OSString path,filename;

    SplitPath(path,filename,correct_filename);

    os_out << OS_TEXT(" SplitPath to path: ") << path.c_str() << endl;
    os_out << OS_TEXT("          filename: ") << filename.c_str() << endl;

    cout<<"JoinPath(1,2,3): "<<JoinPath({"1","2","3"}).c_str()<<endl;

    os_out<<OS_TEXT("JoinPath(A,B,C): ")<<JoinPath({OS_TEXT("A"),OS_TEXT("B"),OS_TEXT("C")}).c_str()<<endl;

    os_out<<OS_TEXT("JoinPath(a,b,c): ")<< JoinPath({OSString(OS_TEXT("A")),OSString(OS_TEXT("B")),OSString(OS_TEXT("C"))}).c_str()<<endl;

    return 0;
}
