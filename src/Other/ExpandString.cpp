#include<hgl/type/BaseString.h>
#include<hgl/type/Map.h>
#include<hgl/Info.h>

namespace hgl
{
    Map<OSString,OSString> ExpandString;

    void SetExpandString(const OSString &flag,const OSString &str)
    {
        OSString flag_string=flag;

        flag_string.LowerCase();

        ExpandString.Add(flag_string,str);
    }

    void ClearExpandString(const OSString &flag)
    {
        OSString flag_string=flag;

        flag_string.LowerCase();

        ExpandString.DeleteByKey(flag_string);
    }

    bool GetExpandString(const OSString &flag,OSString &str)
    {
        struct
        {
            os_char flag[16];
            HGLFlagString index;
        }
        FlagStringIndex[]=
        {
            {OS_TEXT("CMGDK"        ),  hfsCMGDKPath        },
            {OS_TEXT("PlugIn"       ),  hfsPlugInPath       },
            {OS_TEXT("GUI"          ),  hfsGUIPath          },
            {OS_TEXT("Start"        ),  hfsStartPath        },

            {OS_TEXT("OS"           ),  hfsOSPath           },
            {OS_TEXT("Desktop"      ),  hfsUserDesktopPath  },
            {OS_TEXT("CommonDesktop"),  hfsCommonDesktopPath},
            {OS_TEXT("Temp"         ),  hfsTempPath         },
            {OS_TEXT("Data"         ),  hfsUserDataPath     },
            {OS_TEXT("Program"      ),  hfsUserProgramPath  },

            {OS_TEXT(""             ),  hfsEnd              },
        };

        HGLFlagString index=hfsNone;

        for(int i=0;;i++)
        {
            if(FlagStringIndex[i].index==hfsEnd)
                break;

            if(flag==FlagStringIndex[i].flag)
            {
                index=FlagStringIndex[i].index;
                break;
            }
        }

        if(index!=hfsNone)
        {
            str=GetString(index);
            return(true);
        }

        OSString flag_string=flag;

        flag_string.LowerCase();

        return ExpandString.Get(flag_string,str);
    }

    /**
    * 转换特殊扩展字符串
    * @param source 原始字符串
    * @param target 转换后的字符串
    */
    bool ConvertExpandString(const OSString &source,OSString &target)
    {
        target=source;

        while(true)
        {
            const os_char *str=target.c_str();
            const os_char *start=hgl::strchr(str,'$');
            const os_char *end;

            if(!start)return(true);

            if(*(start+1)!='(')return(true);

            end=hgl::strchr(start+2,')');

            if(!end)return(true);

            OSString flag(start+2,end-start-2);
            OSString flag_string;

            if(!GetExpandString(flag,flag_string))
            {
                LOG_ERROR(OS_TEXT("无法识别的标识串：")+flag);

                return(false);
            }

            OSString new_str(str,start-str);

            new_str+=flag_string;

            new_str+=(end+1);

            target=new_str;
        }
    }
}//namespace hgl
