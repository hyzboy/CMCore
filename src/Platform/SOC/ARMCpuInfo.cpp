#include<hgl/platform/SOC.h>
#include<hgl/type/UnorderedMap.h>

namespace hgl
{
    const uint32 ParseARMCpuName(ARMCpuName &arm_cpu_name,const char *str)
    {
        if(!str)return(0);

        arm_cpu_name.value=0;

        const char *p=str;

        if(*p=='A')arm_cpu_name.family='A';else
        if(*p=='X')arm_cpu_name.family='X';else
            return(0);

        ++p;

        uint num;

        if(hgl::stou(p,num)==false)
            return(0);

        if(num<0)
            return(0);

        if(arm_cpu_name.family=='X')
        {
            arm_cpu_name.gen=num;
        }
        else
        if(arm_cpu_name.family=='A')
        {
            if(num<30)
            {
                arm_cpu_name.gen=num;
            }
            else
            {
                arm_cpu_name.level=*p-'0';
                ++p;
                hgl::stou(p,arm_cpu_name.gen);
            }
        }

        while(hgl::isdigit(*p))++p;     //跳过所有数字

        if(*p=='C')
            arm_cpu_name.c=true;
        else
        if(p[0]=='A'
         &&p[1]=='E')
            arm_cpu_name.ae=true;

        return arm_cpu_name.value;
    }

    bool ParseARMCpuCoreInfo(ARMCpuCoreInfo *info,const uint32 &cpu_name_id)
    {
        if(!info)return(false);
        if(cpu_name_id==0)return(false);

        info->name.value=cpu_name_id;

        if(info->name.family=='X')
        {
            info->support_64bit=true;

            if(info->name.gen==1)
            {
                info->support_32bit=true;
                info->arch=ARMArch::ARMv8_2;
            }
            else if(info->name.gen>4)
            {
                info->arch=ARMArch::ARMv9_2;
            }
            else
            {
                info->arch=ARMArch::ARMv9;
            }
        }
        else if(info->name.family=='A')
        {
            if(info->name.level<3)
            {
                info->support_32bit=true;
                info->arch=ARMArch::ARMv7;
            }
            else
            if((info->name.level==3&&info->name.gen==4))    //A34
            {
                info->support_64bit=true;
                info->arch=ARMArch::ARMv8;
            }
            else
            if((info->name.level==6&&info->name.gen==5))     //A65
            {
                info->support_64bit=true;
                info->arch=ARMArch::ARMv8_2;
            }
            else
            {
                if(info->name.gen>2)
                    info->support_64bit=true;

                if(info->name.gen<10)
                    info->support_32bit=true;

                if((info->name.level==3&&info->name.gen==5)     //A35
                 ||(info->name.level==5&&info->name.gen==7))    //A57
                    info->arch=ARMArch::ARMv8;
                else
                if(info->name.gen<5)
                    info->arch=ARMArch::ARMv8;
                else
                if(info->name.gen<10)
                    info->arch=ARMArch::ARMv8_2;
                else
                if(info->name.gen<20)
                    info->arch=ARMArch::ARMv9;
                else
                    ARMArch::ARMv9_2;
            }
        }
        else
            return(false);

        return(true);
    }
}//namespace hgl
