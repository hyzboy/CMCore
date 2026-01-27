#include<hgl/type/StrChar.h>
#include<hgl/platform/SOC.h>

using namespace hgl;

namespace
{
    bool isQualcomm(SOCInfo &soc,const char *soc_name)
    {
        const char *p=hgl::stristr(soc_name,hgl::strlen(soc_name),"Qualcomm ",9);

        if(!p)
            return(false);

        soc.vendor=SOCVendor::Qualcomm;

        p+=9;
        if(*p=='T') //"Qualcomm Technologies, Inc" 但是里面有写逗号的有写句号的，所以我找最后的Inc
        {
            p=hgl::stristr(p,hgl::strlen(p),"Inc",3);

            if(!p)
                return(false);

            p+=3;

            while(!hgl::isalnum(*p))++p;     //碰到字母再停下来

            hgl::strcpy(soc.model,sizeof(soc.model),p);
        }
        else  //如“Qualcomm APQ8084”这种
        {
            hgl::strcpy(soc.model,sizeof(soc.model),p);
        }

        return(true);
    }

    bool isMediaTek(SOCInfo &soc,const char *soc_name)
    {
        const char *p=hgl::stristr(soc_name,hgl::strlen(soc_name),"MTK ",4);

        if(p)
        {
            p+=4;

            hgl::strcpy(soc.model,sizeof(soc.model),p);
            return(true);
        }

        p=hgl::stristr(soc_name,hgl::strlen(soc_name),"MT",2);

        if(!p)
           return(false);

        soc.vendor=SOCVendor::MediaTek;

        hgl::strcpy(soc.model,sizeof(soc.model),p);

        return(true);
    }

    bool isUnisoc(SOCInfo &soc,const char *soc_name)
    {
        const char *p=hgl::stristr(soc_name,hgl::strlen(soc_name),"Unisoc",6);

        if(!p)
            return(false);

        soc.vendor=SOCVendor::Unisoc;

        if(p==soc_name)     //就在最前面，"Unisoc ...."结构
        {
            hgl::strcpy(soc.model,sizeof(soc.model),p+7);
        }
        else                //在型号后面，"Txxx-Unisoc"结构
        {
            hgl::strcpy(soc.model,sizeof(soc.model),soc_name,p-soc_name-1);
        }

        return(true);
    }

    bool isKirin(SOCInfo &soc,const char *soc_name)
    {
        const char *p=hgl::stristr(soc_name,hgl::strlen(soc_name),"Kirin",5);

        if(!p)
            return(false);

        soc.vendor=SOCVendor::Hisilicon;

        hgl::strcpy(soc.model,sizeof(soc.model),p);

        return(true);
    }

    bool isExynos(SOCInfo &soc,const char *soc_name)
    {
        const char *p=hgl::stristr(soc_name,hgl::strlen(soc_name),"Exynos",6);

        if(!p)
            return(false);

        soc.vendor=SOCVendor::Samsung;

        p+=6;

        if(*p==' ')++p;

        hgl::strcpy(soc.model,sizeof(soc.model),p);

        return(true);
    }

    bool isSpreadtrum(SOCInfo &soc,const char *soc_name)
    {
        const char *p=hgl::stristr(soc_name,hgl::strlen(soc_name),"Spreadtrum",10);

        if(!p)
            return(false);

        soc.vendor=SOCVendor::Spreadtrum;

        p+=10;

        if(*p==' ')++p;

        hgl::strcpy(soc.model,sizeof(soc.model),p);

        return(true);
    }

    bool isJLQ(SOCInfo &soc,const char *soc_name)
    {
        const char *p=hgl::stristr(soc_name,hgl::strlen(soc_name),"JLQ ",4);

        if(!p)
            return(false);

        soc.vendor=SOCVendor::JLQ;

        p+=4;

        hgl::strcpy(soc.model,sizeof(soc.model),p);

        return(true);
    }
}//namespace

namespace hgl
{
    const SOCVendor ParseSOCVendor(const char *str,const int len)
    {
        if(!str||!*str)return(SOCVendor::Unknow);

        int index=hgl::find_str_in_array<char>(int(SOCVendor::RANGE_SIZE),(const char **)SOCVendorName,str,len);

        if(index<0||index>=int(SOCVendor::RANGE_SIZE))
            return(SOCVendor::Unknow);

        return SOCVendor(index);
    }

    const SOCGPUVendor ParseSOCGPUVendor(const char *str,const int len)
    {
        if(!str||!*str)return(SOCGPUVendor::Unknow);

        int index=hgl::find_str_in_array<char>(int(SOCGPUVendor::RANGE_SIZE),(const char **)SOCGPUVendorName,str,len);

        if(index<0||index>=int(SOCGPUVendor::RANGE_SIZE))
            return(SOCGPUVendor::Unknow);

        return SOCGPUVendor(index);
    }

    bool ParseSOCInfo(SOCInfo &soc,const char *soc_name)
    {
        hgl_zero(soc);

        if(hgl::strlen(soc_name)>0)
        {
            if(isQualcomm   (soc,soc_name))return(true);
            if(isKirin      (soc,soc_name))return(true);
            if(isMediaTek   (soc,soc_name))return(true);
            if(isUnisoc     (soc,soc_name))return(true);
            if(isExynos     (soc,soc_name))return(true);
            if(isSpreadtrum (soc,soc_name))return(true);
            if(isJLQ        (soc,soc_name))return(true);
        }

        soc.vendor=SOCVendor::Unknow;

        return(false);
    }
}//namespace hgl
