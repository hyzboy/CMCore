#include<hgl/filesystem/FileSystem.h>
#include<hgl/io/FileInputStream.h>
#include<hgl/io/FileOutputStream.h>
#include<hgl/time/Time.h>

int main()
{
    using namespace hgl;

    OSString current_path;

    if(!filesystem::GetCurrentPath(current_path))
        return 1;

    OSString filename;
    io::FileOutputStream output;
    bool created=false;

    for(uint i=0;i<32;i++)
    {
        const OSString leaf=OS_TEXT("HGLCreateExclusiveTest.")+
                            OSString::numberOf(GetTimeUs())+OS_TEXT(".")+
                            OSString::numberOf(i)+OS_TEXT(".tmp");

        filename=filesystem::JoinPathWithFilename(current_path,leaf);

        if(output.Create(filename))
        {
            created=true;
            break;
        }
    }

    if(!created)
        return 2;

    constexpr char expected[]="preserve";

    if(output.Write(expected,sizeof(expected)-1)!=sizeof(expected)-1)
    {
        output.Close();
        filesystem::FileDelete(filename);
        return 3;
    }

    output.Close();

    io::FileOutputStream duplicate;

    if(duplicate.Create(filename))
    {
        duplicate.Close();
        filesystem::FileDelete(filename);
        return 4;
    }

    char actual[sizeof(expected)-1];
    int64 bytes_read=0;

    {
        io::OpenFileInputStream input(filename);

        if(!input)
        {
            filesystem::FileDelete(filename);
            return 5;
        }

        bytes_read=input->Read(actual,sizeof(actual));
    }

    bool content_preserved=bytes_read==sizeof(actual);

    for(size_t i=0;content_preserved&&i<sizeof(actual);i++)
        content_preserved=actual[i]==expected[i];

    if(!filesystem::FileDelete(filename))
        return 6;

    return content_preserved?0:7;
}
