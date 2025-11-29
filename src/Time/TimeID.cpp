#include<hgl/time/TimeID.h>
#include<chrono>

namespace hgl
{
    TimeID GetCurrentTimeID()
    {
        TimeID tid;

        using namespace std::chrono;

        auto now    =system_clock::now();

        auto epoch  =now.time_since_epoch();

        auto sec    =time_point_cast<seconds>(now).time_since_epoch().count();

        auto micro  =duration_cast<microseconds>(epoch).count()%1000000;

        std::time_t t   =sec;
        std::tm *   lt  =std::localtime(&t);

        if(!lt)lt=new std::tm;

        tid.year        =lt->tm_year+1900;
        tid.month       =lt->tm_mon+1;
        tid.day         =lt->tm_mday;
        tid.hour        =lt->tm_hour;
        tid.minute      =lt->tm_min;
        tid.second      =lt->tm_sec;
        tid.micro_second=micro;

        return(tid);
    }
}//namespace hgl
