#pragma once

#include<hgl/type/EnumUtil.h>
#include<hgl/type/ValueArray.h>

namespace hgl::filesystem
{
    /**
     * 卷信息数据结构
     */
    struct VolumeInfo
    {
        enum class DriverType
        {
            None=0,

            Removable,            ///<可移动设备
            Fixed,                ///<固定设备
            Remote,               ///<远程设备
            CDROM,                ///<光盘驱动器
            RamDisk,              ///<内存虚拟设备

            ENUM_CLASS_RANGE(None,RamDisk)
        };

        u16char             name[HGL_MAX_PATH];         ///<卷名称

        u16char             path[HGL_MAX_PATH];         ///<卷所对应的路径名(注意:不是所有卷都有对应路径)

        DriverType          driver_type;                ///<驱动器类型(注意:不是所有的卷都对应驱动器)

        uint32              serial;                     ///<卷序列号

        u16char             volume_label[HGL_MAX_PATH]; ///<卷标名称

        u16char             file_system[HGL_MAX_PATH];  ///<文件系统名称

        uint32              filename_max_length;        ///<文件名最大长度

        bool                unicode;                    ///<文件名支持UNICODE

        uint64              available_space;            ///<有效容量
        uint64              total_space;                ///<总空量
        uint64              free_space;                 ///<自由容量

        bool operator==(const VolumeInfo& other) const
        {
            return memcmp(this, &other, sizeof(VolumeInfo)) == 0;
        }
    };//struct VolumeInfo

#if HGL_OS == HGL_OS_Windows

    /**
        * 卷检测配置
        */
    struct VolumeCheckConfig
    {
        bool removable  =true;
        bool fixed      =true;
        bool remote     =true;
        bool cdrom      =true;
        bool ram_disk   =true;
        bool unknow     =true;

    public:

        /**
            * 是否无效配置
            */
        bool isErrorConfig()const
        {
            if(removable)return(false);
            if(fixed)return(false);
            if(remote)return(false);
            if(cdrom)return(false);
            if(ram_disk)return(false);
            if(unknow)return(false);

            return(true);
        }
    };

    using VolumeInfoList=ValueArray<VolumeInfo>;

    /**
        * 枚举当前计算机所有卷
        * @param vi_list 储存卷信息的列表
        * @param check_removable 检测可移除设备
        * @param check_remote 检测远程驱动器
        * @param check_cd 检测光盘
        * @return 查找到的卷数量，-1表示失败
        */
    int EnumVolume(VolumeInfoList &vi_list,const VolumeCheckConfig &);
#endif//HGL_OS == HGL_OS_Windows
}//namespace hgl::filesystem
