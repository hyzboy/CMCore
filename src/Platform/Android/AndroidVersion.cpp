#include<hgl/platform/Android.h>

namespace hgl
{
    /*
        https://apilevels.com/
    */

    const int GetAndroidAPILevel(const AndroidVersion &av)
    {
        int api_level=-1;

        if(av.major==1)
        {
            if(av.minor==0)api_level=1;else
            if(av.minor==1)api_level=2;else
            if(av.minor==5)api_level=3;else
            if(av.minor==6)api_level=4;
        }
        else
        if(av.major==2)
        {
            if(av.minor==0)api_level=(av.patch>=1)?6:5;else
            if(av.minor==1)api_level=7;else
            if(av.minor==2)api_level=8;else
            if(av.minor==3)api_level=(av.patch>=3)?10:9;
        }
        else
        if(av.major==3)
        {
            if(av.minor==0)api_level=11;else
            if(av.minor==1)api_level=12;else
            api_level=13;
        }
        else
        if(av.major==4)
        {
            if(av.minor==0)api_level=(av.patch>=3)?15:14;else
            if(av.minor==1)api_level=16;else
            if(av.minor==2)api_level=17;else
            if(av.minor==3)api_level=18;else
            if(av.minor==4)api_level=(av.postfix=='W')?20:19;
        }
        else
        if(av.major==5)
        {
            if(av.minor==0)api_level=21;else
            if(av.minor==1)api_level=22;
        }
        else
        if(av.major==6)api_level=23;else
        if(av.major==7)api_level=(av.minor==0)?24:25;else
        if(av.major==8)api_level=(av.minor==0)?26:27;else
        if(av.major==9)api_level=28;else
        if(av.major==10)api_level=29;else
        if(av.major==11)api_level=30;else
        if(av.major==12)api_level=(av.postfix=='L')?32:31;else
        if(av.major==13)api_level=33;else
        if(av.major>=14)api_level=34;

        return(api_level);
    }
}//namespace hgl
