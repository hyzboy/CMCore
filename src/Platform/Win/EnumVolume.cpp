#include<hgl/filesystem/EnumVolume.h>
#include<hgl/type/String.h>
#include<hgl/log/Log.h>
#include<windows.h>
#include<winioctl.h>

namespace hgl::filesystem
{
    /**
     * 查询卷对应的底层物理磁盘号
     * @param volume_name     卷设备名(形如 \\\\?\\Volume{GUID}\\)
     * @param out_disk_id     输出物理磁盘号(PhysicalDriveN),失败时为 InvalidPhysicalDisk
     * @param out_extent_count 输出卷跨越的物理磁盘数,失败时为 0
     * @return 查询成功返回 true
     */
    static bool QueryVolumePhysicalDisk(const u16char *volume_name,uint32 &out_disk_id,uint32 &out_extent_count)
    {
        out_disk_id=VolumeInfo::InvalidPhysicalDisk;
        out_extent_count=0;

        u16char device_path[HGL_MAX_PATH+2];
        hgl::strcpy(device_path,HGL_MAX_PATH+2,volume_name);

        //卷设备名以反斜杠结尾,CreateFileW 查询卷元数据时要求去掉末尾反斜杠
        size_t len=hgl::strlen<u16char>(device_path);
        while(len>0 && device_path[len-1]==u'\\') device_path[--len]=0;

        //dwDesiredAccess=0:仅查询设备元数据,无需管理员权限
        HANDLE h_vol=CreateFileW(device_path,0,FILE_SHARE_READ|FILE_SHARE_WRITE,nullptr,OPEN_EXISTING,0,nullptr);

        if(h_vol==INVALID_HANDLE_VALUE)return(false);

        alignas(8) uint8 ext_buf[sizeof(VOLUME_DISK_EXTENTS)+16*sizeof(DISK_EXTENT)];
        DWORD returned=0;
        bool  ok=false;

        if(DeviceIoControl(h_vol,IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,nullptr,0,
                           ext_buf,sizeof(ext_buf),&returned,nullptr) &&
           returned>=sizeof(VOLUME_DISK_EXTENTS))
        {
            const VOLUME_DISK_EXTENTS *exts=reinterpret_cast<const VOLUME_DISK_EXTENTS *>(ext_buf);

            if(exts->NumberOfDiskExtents>=1)
            {
                out_disk_id=(uint32)exts->Extents[0].DiskNumber;
                out_extent_count=(uint32)exts->NumberOfDiskExtents;
                ok=true;
            }
        }

        CloseHandle(h_vol);
        return(ok);
    }

    int EnumVolume(ValueArray<VolumeInfo> &vi_list,const VolumeCheckConfig &cfg)
    {
        if(cfg.isErrorConfig())
            return(-1);

        HANDLE handle;
        u16char volume_name[HGL_MAX_PATH+1];
        u16char path_name[HGL_MAX_PATH];
        int count=0;

        handle=FindFirstVolumeW(volume_name,HGL_MAX_PATH);

        if(handle==INVALID_HANDLE_VALUE)return(-2);

        do
        {
            hgl::filesystem::VolumeInfo vi;

            memset(&vi,0,sizeof(hgl::filesystem::VolumeInfo));

            vi.physical_disk_id   =VolumeInfo::InvalidPhysicalDisk;
            vi.disk_extent_count  =0;

            DWORD length;

            GetVolumePathNamesForVolumeNameW(volume_name,path_name,HGL_MAX_PATH,&length);                //这个函数要win xp/2003才能用

            path_name[length]=0;

            hgl::strcpy(vi.name,HGL_MAX_PATH,volume_name);
            hgl::strcpy(vi.path,HGL_MAX_PATH,path_name);

            UINT type=GetDriveTypeW(path_name);

            if(type==DRIVE_REMOVABLE){  if(!cfg.removable   )continue;vi.driver_type=VolumeInfo::DriverType::Removable;  }else
            if(type==DRIVE_FIXED    ){  if(!cfg.fixed       )continue;vi.driver_type=VolumeInfo::DriverType::Fixed;      }else
            if(type==DRIVE_REMOTE   ){  if(!cfg.remote      )continue;vi.driver_type=VolumeInfo::DriverType::Remote;     }else
            if(type==DRIVE_RAMDISK  ){  if(!cfg.ram_disk    )continue;vi.driver_type=VolumeInfo::DriverType::RamDisk;    }else
            if(type==DRIVE_CDROM    ){  if(!cfg.cdrom       )continue;vi.driver_type=VolumeInfo::DriverType::CDROM;      }else
            {
                if(cfg.unknow)vi.driver_type=VolumeInfo::DriverType::None;
                continue;
            }

            uint32 file_system_flags;

            if(GetVolumeInformationW(path_name,
                vi.volume_label,
                255,
                (unsigned long *)&vi.serial,
                (unsigned long *)&vi.filename_max_length,
                (unsigned long *)&file_system_flags,
                vi.file_system,
                255))
            {
                vi.unicode=file_system_flags&FILE_UNICODE_ON_DISK;
            }
            else
            {
                GLogWarning(U16_TEXT("Get <")+U16String(path_name)+U16_TEXT("> info failed！Windows error code: ")+U16String::numberOf((uint)GetLastError()));
            }

            uint32 disk_id;
            uint32 extent_count;

            if(QueryVolumePhysicalDisk(volume_name,disk_id,extent_count))
            {
                vi.physical_disk_id =disk_id;
                vi.disk_extent_count=extent_count;
            }

            if(GetDiskFreeSpaceExW(path_name,
                (ULARGE_INTEGER *)&vi.available_space,
                (ULARGE_INTEGER *)&vi.total_space,
                (ULARGE_INTEGER *)&vi.free_space))
            {
                vi_list.Add(vi);
            }
            else
            {
                GLogWarning(U16_TEXT("Get disk free space <")+U16String(path_name)+U16_TEXT("> data failed,Windows error code: ")+U16String::numberOf((uint)GetLastError()));
            }

            count++;
        } while(FindNextVolumeW(handle,volume_name,HGL_MAX_PATH));

        FindVolumeClose(handle);

        return(count);
    }
}//namespace hgl::filesystem
