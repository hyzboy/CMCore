#pragma once

#include<../../../CMCore/inc/hgl/type/EnumUtil.h>
#include<hgl/CompOperator.h>

namespace hgl
{
    enum class SOCVendor
    {
        Unknow=0,

        Qualcomm,       ///<高通
        Hisilicon,      ///<海思
        Samsung,        ///<三星
        MediaTek,       ///<联发科
        Unisoc,         ///<展锐
        Spreadtrum,     ///<展讯
        JLQ,            ///<瓴盛

        //Maxvell,        ///<马维尔
        //Broadcom,       ///<博通
        //TI,             ///<德州仪器
        //Rockchip,       ///<瑞芯微
        //Intel,          ///<英特尔
        //nVidia,         ///<英伟达
        //AMD,            ///<AMD
        //Google,         ///<谷歌

        ENUM_CLASS_RANGE(Unknow,JLQ)
    };

    constexpr const char *SOCVendorName[]=
    {
        "Unknow",
        "Qualcomm",
        "Hisilicon",
        "Samsung",
        "MediaTek",
        "Unisoc",
        "Spreadtrum",
        "JLQ",
    };

    const SOCVendor ParseSOCVendor(const char *str,const int len=0);

    enum class CpuArch
    {
        Unknow=0,

        ARMv7,          ///<ARMv7
        ARMv8,          ///<ARMv8
        ARMv9,          ///<ARMv9

        MIPS,

        X86_32,
        X86_64,

        ENUM_CLASS_RANGE(Unknow,X86_64)
    };

    enum class ARMArch
    {
        Unknow=0,

        ARMv7,          ///<ARMv7A
        ARMv8,
        ARMv8_2,
        ARMv9,
        ARMv9_2,

        ENUM_CLASS_RANGE(Unknow,ARMv9_2)
    };

    union ARMCpuName
    {
        //如A76，level为7,gen为6
        //A710，level为7,gen为10

        struct
        {
            char    family;     ///<A或X
            uint8   level;      ///<级别,目前就0,1,3,5,7
            uint8   gen;        ///<代数
            bool    ae:1;
            bool    c:1;
        };

        uint32  value;
    };

    /**
    * ARM CPU 核心信息
    */
    struct ARMCpuCoreInfo
    {
        ARMCpuName  name;

        ARMArch     arch;

        bool        support_32bit;
        bool        support_64bit;
    };

    const uint32 ParseARMCpuName(ARMCpuName &,const char *);                                     ///<根据字符串解晰ARM CPU名称ID

    bool ParseARMCpuCoreInfo(ARMCpuCoreInfo *,const uint32 &cpu_name_id);           ///<根据ARM CPU名字ID解晰CPU信息

    enum class KryoArchLevel
    {
        Prime,
        Gold,
        Silver,

        ENUM_CLASS_RANGE(Prime,Silver)
    };

    /**
    * ARM CPU簇信息<br>
    * 注：这里的频率单位为MHz，而且只是默认频率，实际频率可能会更高或更低
    */
    struct ARMCpuClusterInfo
    {
        ARMCpuCoreInfo arm;             ///<对应的ARM核心信息

        uint core_count;                ///<核心数量

        uint base_freq;                 ///<基础频率
        uint boost_freq;                ///<最高频率
    };

    struct SOCInfo
    {
        SOCVendor       vendor;
        char            model[32];

    public:

        CompOperatorMemcmp(const SOCInfo &);
    };

    /**
     * 根据收集到的SOC信息，解晰具体的SOC厂商和主要型号
     */
    bool ParseSOCInfo(SOCInfo &,const char *);

    enum class SOCGPUVendor
    {
        Unknow=0,

        PowerVR,
        Mali,
        Adreno,
        Vivante,
        Intel,
        Radeon,
        Tegra,
        Maleoon,
        ENUM_CLASS_RANGE(Unknow,Maleoon)
    };

    constexpr const char *SOCGPUVendorName[]=
    {
        "Unknow",

        "PowerVR",
        "Mali",
        "Adreno",
        "Vivante",
        "Intel",
        "Radeon",
        "Tegra",
        "Maleoon",
    };

    const SOCGPUVendor ParseSOCGPUVendor(const char *str,const int len=0);

    struct SOCGPUInfo
    {
        SOCGPUVendor    vendor;
        char            model[32];

        uint            core_count;
        uint            freq;

    public:

        int Comp(const SOCGPUInfo &info)const
        {
            int result;

            result=int(vendor)-int(info.vendor);

            if(result)return result;

            result=strcmp(model,info.model);

            if(result)return result;

            return core_count-info.core_count;
        }

        CompOperator(const SOCGPUInfo &,Comp);
    };

    /**
    * SOC 产品信息
    */
    struct SOCProductInfo
    {
        char                product_name[64];   ///<产品名称

        SOCInfo             soc_info;           ///<SOC基本信息

        SOCGPUInfo          gpu_info;           ///<GPU信息

        uint                cpu_cluster_count;  ///<CPU簇数量
        struct
        {
            char            core[32];           ///<CPU核心名称
            uint            count;              ///<核心数量
            float           freq;               ///<频率
        }cpu_cluster[4];                        ///<CPU簇信息
    };//struct SOCProductInfo
}//namespace hgl
