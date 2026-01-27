#include<hgl/filesystem/EnumVolume.h>
#include<iostream>

using namespace hgl;
using namespace hgl::filesystem;
using namespace std;

void out_size(const os_char *front,const uint64 size)
{
    os_char str[64];

    if(!int_to_size(str,64,size))
    {
        os_out<<front<<size<<endl;
    }
    else
    {
        os_out<<front<<size<<OS_TEXT("(")<<str<<OS_TEXT(")")<<endl;
    }
}

void out(const VolumeInfo &vi)
{
    constexpr const os_char *DriverTypeName[(size_t)VolumeInfo::DriverType::RANGE_SIZE]=
    {
        OS_TEXT("None"),
        OS_TEXT("Removable"),
        OS_TEXT("Fixed"),
        OS_TEXT("Remote"),
        OS_TEXT("CDROM"),
        OS_TEXT("RamDisk")
    };

        os_out<<OS_TEXT("        Volume Name: ")<<vi.name<<endl;
        os_out<<OS_TEXT("        Volume Path: ")<<vi.path<<endl;

    if(RangeCheck<VolumeInfo::DriverType>(vi.driver_type))
    {
        os_out<<OS_TEXT("        Driver Type: ")<<DriverTypeName[(size_t)vi.driver_type-(size_t)VolumeInfo::DriverType::BEGIN_RANGE]<<endl;
    }
    else
    {
        os_out<<OS_TEXT("        Driver Type: unknow!")<<endl;
    }

        os_out<<OS_TEXT("             Serial: ")<<vi.serial<<endl;

        os_out<<OS_TEXT("       Volume lable: ")<<vi.volume_label<<endl;
        os_out<<OS_TEXT("        File system: ")<<vi.file_system<<endl;
        os_out<<OS_TEXT("Max filename length: ")<<vi.filename_max_length<<endl;
        os_out<<OS_TEXT("    Unicode support: ")<<(vi.unicode?OS_TEXT("true"):OS_TEXT("false"))<<endl;

        out_size(OS_TEXT("    Available space: "),vi.available_space);
        out_size(OS_TEXT("        Total space: "),vi.total_space);
        out_size(OS_TEXT("         Free space: "),vi.free_space);

    os_out<<endl<<endl;
}

int os_main(int,os_char **)
{
    os_out<<OS_TEXT("Enum Volumes")<<endl<<endl;

    VolumeCheckConfig vcc;
    VolumeInfoList vil;

    EnumVolume(vil,vcc);

    os_out<<OS_TEXT("Volume count: ")<<vil.GetCount()<<endl<<endl;

    for(auto vi:vil)
        out(vi);

    return 0;
}
