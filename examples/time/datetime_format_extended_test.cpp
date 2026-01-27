#include <hgl/time/DateTime.h>
#include <hgl/time/Time.h>
#include <iostream>

using namespace hgl;
using namespace std;

int main(int, char**)
{
    cout << "Date/Time Formatting with Week Day and Shichen (时辰) Test" << endl;
    cout << "==========================================================" << endl << endl;
    
    // 创建一些测试日期和时间
    CalendarDate dates[] = {
        CalendarDate(2024, 1, 1),   // 2024-01-01 Monday
        CalendarDate(2024, 1, 7),   // 2024-01-07 Sunday
        CalendarDate(2024, 6, 15),  // 2024-06-15 Saturday
    };
    
    TimeOfDay times[] = {
        TimeOfDay(0, 30, 0, 0),     // 00:30 子时
        TimeOfDay(3, 0, 0, 0),      // 03:00 寅时
        TimeOfDay(7, 30, 0, 0),     // 07:30 辰时
        TimeOfDay(11, 0, 0, 0),     // 11:00 午时
        TimeOfDay(14, 30, 0, 0),    // 14:30 未时
        TimeOfDay(18, 0, 0, 0),     // 18:00 酉时
        TimeOfDay(23, 30, 0, 0),    // 23:30 子时
    };
    
    // 测试日期格式 - 星期
    cout << "=== Date Formatting with Week Day ===" << endl;
    
    for (int i = 0; i < 3; i++)
    {
        CalendarDate &date = dates[i];
        
        // 标准格式
        cout << "Date " << (i+1) << ": " << DateToString(date, DateFormat("YYYY-MM-DD")).c_str() << endl;
        
        // 带星期数字
        cout << "  With WD:   " << DateToString(date, DateFormat("YYYY-MM-DD (WD)")).c_str() << endl;
        
        // 带星期英文
        cout << "  With WEEK: " << DateToString(date, DateFormat("YYYY-MM-DD WEEK")).c_str() << endl;
        
        // 带星期中文
        cout << "  With 周:   " << DateToString(date, DateFormat("YYYY年MM月DD日 周")).c_str() << endl;
        
        cout << endl;
    }
    
    // 测试时间格式 - 时辰
    cout << "=== Time Formatting with Shichen (时辰) ===" << endl;
    
    for (int i = 0; i < 7; i++)
    {
        TimeOfDay &time = times[i];
        
        // 标准格式
        cout << "Time " << (i+1) << ": " << TimeToString(time, TimeFormat("HH:MM:SS")).c_str() << endl;
        
        // 带时辰数字
        cout << "  With SC:   " << TimeToString(time, TimeFormat("HH:MM:SS (SC)")).c_str() << endl;
        
        // 带时辰中文
        cout << "  With 时辰: " << TimeToString(time, TimeFormat("HH时MM分SS秒 时辰")).c_str() << endl;
        
        cout << endl;
    }
    
    // 组合测试
    cout << "=== Combined Date/Time Formatting ===" << endl;
    
    CalendarDate test_date(2024, 6, 15);  // Saturday
    TimeOfDay test_time(14, 30, 45, 123456);
    
    DateTimeFormat fmt1(
        DateFormat("YYYY-MM-DD WEEK"),
        TimeFormat("HH:MM:SS.ms"),
        " "
    );
    cout << "Format 1: " << DateTimeToString(test_date, test_time, fmt1).c_str() << endl;
    
    DateTimeFormat fmt2(
        DateFormat("YYYY年MM月DD日 周"),
        TimeFormat("HH时MM分SS秒 时辰"),
        " "
    );
    cout << "Format 2: " << DateTimeToString(test_date, test_time, fmt2).c_str() << endl;
    
    DateTimeFormat fmt3(
        DateFormat("WEEK, YYYY-MM-DD"),
        TimeFormat("时辰 HH:MM:SS"),
        " | "
    );
    cout << "Format 3: " << DateTimeToString(test_date, test_time, fmt3).c_str() << endl;
    
    cout << endl;
    cout << "All tests completed!" << endl;
    
    return 0;
}
