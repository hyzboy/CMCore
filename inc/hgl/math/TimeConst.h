#ifndef HGL_MATH_TIME_CONST_INCLUDE
#define HGL_MATH_TIME_CONST_INCLUDE
namespace hgl
{
    /**
     * ����ÿ��ö��
     */
    enum class Weekday
    {
        Sunday=0,   ///<������
        Monday,     ///<����һ
        Tuesday,    ///<���ڶ�
        Wednesday,  ///<������
        Thursday,   ///<������
        Friday,     ///<������
        Saturday,   ///<������
        
        BEGIN_RANGE =Sunday,
        END_RANGE   =Saturday,
        RANGE_SIZE  =END_RANGE-BEGIN_RANGE+1        
    };

    /**
     * �·�ö��
     */
    enum class Month
    {
        January=1, ///<һ��
        February,  ///<����
        March,     ///<����
        April,     ///<����
        May,       ///<����
        June,      ///<����
        July,      ///<����
        August,    ///<����
        September, ///<����
        October,   ///<ʮ��
        November,  ///<ʮһ��
        December,  ///<ʮ����

        BEGIN_RANGE =January,
        END_RANGE   =December,
        RANGE_SIZE  =END_RANGE-BEGIN_RANGE+1
    };//enum class Month

    constexpr uint  HGL_TIME_ONE_SECOND     =1;
    constexpr uint  HGL_TIME_HALF_MINUTE    =30;
    constexpr uint  HGL_TIME_ONE_MINUTE     =60;
    constexpr uint  HGL_TIME_HALF_HOUR      =30*HGL_TIME_ONE_MINUTE;
    constexpr uint  HGL_TIME_ONE_HOUR       =60*HGL_TIME_ONE_MINUTE;
    constexpr uint  HGL_TIME_HALF_DAY       =12*HGL_TIME_ONE_HOUR;
    constexpr uint  HGL_TIME_ONE_DAY        =24*HGL_TIME_ONE_HOUR;
    constexpr uint  HGL_TIME_ONE_WEEK       =7*HGL_TIME_ONE_DAY;
    constexpr uint  HGL_TIME_ONE_YEAR       =365*HGL_TIME_ONE_DAY;

    constexpr uint  HGL_HOUR_HALF_DAY       =12;
    constexpr uint  HGL_HOUR_ONE_DAY        =24;

    constexpr uint  HGL_DAY_ONE_WEEK        =7;
    constexpr uint  HGL_DAY_ONE_YEAR        =365;

    constexpr uint  HGL_MONTH_ONE_YEAR      =12;

    constexpr uint  HGL_MILLI_SEC_PRE_SEC   =1000;              //����
    constexpr uint  HGL_MICRO_SEC_PER_SEC   =1000*1000;         //΢��
    constexpr uint  HGL_NANO_SEC_PER_SEC    =1000*1000*1000;    //����
}//namespace hgl
#endif//HGL_MATH_TIME_CONST_INCLUDE
