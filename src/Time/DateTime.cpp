#include<hgl/time/DateTime.h>

#include<chrono>
#include<cmath>

namespace hgl
{
    double FromDateTime(const CalendarDate &d,const TimeOfDay &t)
    {
        return FromDateTime(d.GetYear(),d.GetMonth(),d.GetDay(),
                            t.GetHour(),t.GetMinute(),t.GetSecond(),t.GetMicroSecond());
    }

    double FromDateTime(const int year,const int month,const int day,
                        const int hour,const int minute,const int second,const int micro_second)
    {
        std::tm tmv{};
        tmv.tm_year = year - 1900;
        tmv.tm_mon  = month - 1;
        tmv.tm_mday = day;
        tmv.tm_hour = hour;
        tmv.tm_min  = minute;
        tmv.tm_sec  = second;
        tmv.tm_isdst = -1; // let system determine DST
        time_t base = std::mktime(&tmv); // local time to epoch
        if(base == time_t(-1))
            return 0.0;
        double result = double(base) + double(micro_second)/double(HGL_MICRO_SEC_PER_SEC);
        return result;
    }

    void ToDateTime(CalendarDate &d,TimeOfDay &t,const double cur_time)
    {
        double use_time = cur_time;
        if(use_time==0)
        {
            using namespace std::chrono;
            auto now = system_clock::now();
            auto us  = time_point_cast<microseconds>(now).time_since_epoch().count();
            use_time = double(us)/1'000'000.0;
        }
        d.Sync(use_time);
        t.Sync(use_time);
    }

    // =============== CalendarDate/TimeOfDay 字符串转换函数 ===============

    AnsiString DateToString(const CalendarDate &date, const DateFormat &format)
    {
        AnsiString result = format.pattern;
        int year = date.GetYear();
        int month = date.GetMonth();
        int day = date.GetDay();
        int week_day = date.GetWeekDay();  // 0=Sunday, 1=Monday, ..., 6=Saturday

        // 星期英文名称
        static const char* week_names_en[] = {
            "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
        };
        
        // 星期中文名称
        static const char* week_names_cn[] = {
            "日", "一", "二", "三", "四", "五", "六"
        };

        // 替换格式标记
        size_t pos = 0;

        // 替换 YYYY (4位年份)
        while ((pos = result.FindString("YYYY", pos)) != -1)
        {
            char buffer[5];
            snprintf(buffer, sizeof(buffer), "%04d", year);
            result.Replace(pos, 4, buffer);
            pos += 4;
        }

        // 替换 WEEK (星期英文全称) - 必须在 WD 之前处理
        pos = 0;
        while ((pos = result.FindString("WEEK", pos)) != -1)
        {
            const char* week_name = (week_day >= 0 && week_day <= 6) ? week_names_en[week_day] : "Unknown";
            size_t name_len = strlen(week_name);
            result.Replace(pos, 4, week_name);
            pos += name_len;
        }

        // 替换 周 (星期中文，如"周一"、"周日")
        pos = 0;
        while ((pos = result.FindString("周", pos)) != -1)
        {
            const char* week_name = (week_day >= 0 && week_day <= 6) ? week_names_cn[week_day] : "?";
            size_t name_len = strlen(week_name);
            result.Replace(pos, strlen("周"), week_name);  // "周"是3字节UTF-8
            pos += name_len;
        }

        // 替换 WD (星期数字 0-6)
        pos = 0;
        while ((pos = result.FindString("WD", pos)) != -1)
        {
            char buffer[3];  // 需要足够空间存储数字和null终止符
            snprintf(buffer, sizeof(buffer), "%d", week_day);
            result.Replace(pos, 2, buffer);
            pos += 1;
        }

        // 替换 MM (2位月份)
        pos = 0;
        while ((pos = result.FindString("MM", pos)) != -1)
        {
            char buffer[3];
            snprintf(buffer, sizeof(buffer), "%02d", month);
            result.Replace(pos, 2, buffer);
            pos += 2;
        }

        // 替换 DD (2位日期)
        pos = 0;
        while ((pos = result.FindString("DD", pos)) != -1)
        {
            char buffer[3];
            snprintf(buffer, sizeof(buffer), "%02d", day);
            result.Replace(pos, 2, buffer);
            pos += 2;
        }

        return result;
    }

    AnsiString TimeToString(const TimeOfDay &time, const TimeFormat &format)
    {
        AnsiString result = format.pattern;
        int hours = time.GetHour();
        int minutes = time.GetMinute();
        int seconds = time.GetSecond();
        int micro_seconds = time.GetMicroSecond();

        // 计算时辰 (中国传统12时辰制)
        // 子时(23:00-00:59), 丑时(01:00-02:59), 寅时(03:00-04:59), 卯时(05:00-06:59)
        // 辰时(07:00-08:59), 巳时(09:00-10:59), 午时(11:00-12:59), 未时(13:00-14:59)
        // 申时(15:00-16:59), 酉时(17:00-18:59), 戌时(19:00-20:59), 亥时(21:00-22:59)
        static const char* shichen_names[] = {
            "子时", "丑时", "寅时", "卯时", "辰时", "巳时",
            "午时", "未时", "申时", "酉时", "戌时", "亥时"
        };
        
        // 刻的名称 (每个时辰8刻，每刻15分钟)
        static const char* ke_names[] = {
            "初刻", "一刻", "二刻", "三刻", "四刻", "五刻", "六刻", "七刻"
        };
        
        // 计算时辰索引 (0-11)
        int shichen_index;
        if (hours == 23 || hours == 0)
            shichen_index = 0;  // 子时 (23:00-00:59)
        else
            shichen_index = (hours + 1) / 2;  // 其他时辰
        
        // 计算刻 (0-7) - 计算从时辰开始的分钟数，然后除以15
        int minutes_in_shichen;
        if (shichen_index == 0) {  // 子时特殊处理: 23:00-00:59
            if (hours == 23)
                minutes_in_shichen = minutes;           // 23:00-23:59 = 0-59分钟
            else  // hours == 0
                minutes_in_shichen = 60 + minutes;      // 00:00-00:59 = 60-119分钟
        } else {
            // 其他时辰的开始小时 = (索引 × 2) - 1
            // 例如: 午时(索引6) 开始于 11点, 未时(索引7) 开始于 13点
            int shichen_start_hour = (shichen_index * 2) - 1;
            minutes_in_shichen = (hours - shichen_start_hour) * 60 + minutes;
        }
        
        int ke_index = minutes_in_shichen / 15;  // 每15分钟一刻
        if (ke_index > 7) ke_index = 7;          // 最多7刻（初刻到七刻）

        size_t pos = 0;

        // 替换 刻 (刻名称，如"初刻"、"三刻") - 必须在其他替换之前处理
        pos = 0;
        while ((pos = result.FindString("刻", pos)) != -1)
        {
            const char* ke_name = ke_names[ke_index];
            size_t name_len = strlen(ke_name);
            result.Replace(pos, strlen("刻"), ke_name);  // "刻"是3字节UTF-8
            pos += name_len;
        }

        // 替换 时辰 (中文时辰名称) - 必须在 HH 之前处理
        pos = 0;
        while ((pos = result.FindString("时辰", pos)) != -1)
        {
            const char* shichen_name = shichen_names[shichen_index];
            size_t name_len = strlen(shichen_name);
            result.Replace(pos, strlen("时辰"), shichen_name);  // "时辰"是6字节UTF-8
            pos += name_len;
        }
        
        // 替换 KE (刻数字 0-7)
        pos = 0;
        while ((pos = result.FindString("KE", pos)) != -1)
        {
            char buffer[2];
            snprintf(buffer, sizeof(buffer), "%d", ke_index);
            result.Replace(pos, 2, buffer);
            pos += 1;
        }

        // 替换 SC (时辰数字 0-11)
        pos = 0;
        while ((pos = result.FindString("SC", pos)) != -1)
        {
            char buffer[3];
            snprintf(buffer, sizeof(buffer), "%02d", shichen_index);
            result.Replace(pos, 2, buffer);
            pos += 2;
        }

        // 替换 HH (2位小时)
        pos = 0;
        while ((pos = result.FindString("HH", pos)) != -1)
        {
            char buffer[3];
            snprintf(buffer, sizeof(buffer), "%02d", hours);
            result.Replace(pos, 2, buffer);
            pos += 2;
        }

        // 替换 us (6位微秒)，必须在 MM 之前替换以避免冲突
        pos = 0;
        while ((pos = result.FindString("us", pos)) != -1)
        {
            char buffer[7];
            snprintf(buffer, sizeof(buffer), "%06d", micro_seconds);
            result.Replace(pos, 2, buffer);
            pos += 6;
        }

        // 替换 ms (毫秒，3位)
        pos = 0;
        while ((pos = result.FindString("ms", pos)) != -1)
        {
            int milli_seconds = micro_seconds / 1000;
            char buffer[4];
            snprintf(buffer, sizeof(buffer), "%03d", milli_seconds);
            result.Replace(pos, 2, buffer);
            pos += 3;
        }

        // 替换 SS (2位秒)
        pos = 0;
        while ((pos = result.FindString("SS", pos)) != -1)
        {
            char buffer[3];
            snprintf(buffer, sizeof(buffer), "%02d", seconds);
            result.Replace(pos, 2, buffer);
            pos += 2;
        }

        // 替换 MM (2位分钟)，必须在最后替换以避免与 ms 冲突
        pos = 0;
        while ((pos = result.FindString("MM", pos)) != -1)
        {
            char buffer[3];
            snprintf(buffer, sizeof(buffer), "%02d", minutes);
            result.Replace(pos, 2, buffer);
            pos += 2;
        }

        return result;
    }

    AnsiString DateTimeToString(const CalendarDate &date, const TimeOfDay &time, 
                                 const DateTimeFormat &format)
    {
        AnsiString dateStr = DateToString(date, format.dateFormat);
        AnsiString timeStr = TimeToString(time, format.timeFormat);
        return dateStr + format.separator + timeStr;
    }

    AnsiString DateTimeToString(const DateTime &dt, const DateTimeFormat &format)
    {
        return DateTimeToString(dt.date, dt.time, format);
    }
}//namespace hgl
