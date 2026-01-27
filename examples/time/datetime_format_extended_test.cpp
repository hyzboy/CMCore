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
        CalendarDate(2024, 1, 1),   // 2024-01-01 (Monday, weekday=1)
        CalendarDate(2024, 1, 7),   // 2024-01-07 (Sunday, weekday=0)
        CalendarDate(2024, 6, 15),  // 2024-06-15 (Saturday, weekday=6)
    };
    
    TimeOfDay times[] = {
        TimeOfDay(0, 30, 0, 0),     // 00:30 子时六刻
        TimeOfDay(3, 0, 0, 0),      // 03:00 寅时初刻
        TimeOfDay(7, 30, 0, 0),     // 07:30 辰时二刻
        TimeOfDay(11, 0, 0, 0),     // 11:00 午时初刻
        TimeOfDay(11, 45, 0, 0),    // 11:45 午时三刻 (经典)
        TimeOfDay(14, 30, 0, 0),    // 14:30 未时六刻
        TimeOfDay(18, 0, 0, 0),     // 18:00 酉时四刻
        TimeOfDay(23, 30, 0, 0),    // 23:30 子时二刻
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
    
    // 测试时间格式 - 时辰和刻
    cout << "=== Time Formatting with Shichen (时辰) and Ke (刻) ===" << endl;
    
    for (int i = 0; i < 8; i++)
    {
        TimeOfDay &time = times[i];
        
        // 标准格式
        cout << "Time " << (i+1) << ": " << TimeToString(time, TimeFormat("HH:MM:SS")).c_str() << endl;
        
        // 带时辰数字
        cout << "  With SC:     " << TimeToString(time, TimeFormat("HH:MM:SS (SC)")).c_str() << endl;
        
        // 带时辰中文
        cout << "  With 时辰:   " << TimeToString(time, TimeFormat("HH时MM分SS秒 时辰")).c_str() << endl;
        
        // 带刻数字
        cout << "  With KE:     " << TimeToString(time, TimeFormat("HH:MM:SS (KE)")).c_str() << endl;
        
        // 带刻中文
        cout << "  With 刻:     " << TimeToString(time, TimeFormat("HH时MM分SS秒 刻")).c_str() << endl;
        
        // 时辰刻组合 (经典格式)
        cout << "  时辰刻:      " << TimeToString(time, TimeFormat("时辰刻")).c_str() << endl;
        
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
    
    DateTimeFormat fmt4(
        DateFormat("YYYY年MM月DD日"),
        TimeFormat("时辰刻"),
        " "
    );
    cout << "Format 4 (traditional): " << DateTimeToString(test_date, test_time, fmt4).c_str() << endl;
    
    cout << endl;
    
    // 经典的"午时三刻"测试
    cout << "=== Classic 'Wu Shi San Ke' (午时三刻) Test ===" << endl;
    
    CalendarDate execution_date(1900, 1, 1);
    TimeOfDay wushi_sanke(11, 45, 0, 0);  // 11:45 = 午时三刻
    
    cout << "Execution time (11:45): " << TimeToString(wushi_sanke, TimeFormat("时辰刻")).c_str() << endl;
    cout << "Full format: " << DateTimeToString(execution_date, wushi_sanke, 
                                DateTimeFormat(DateFormat("YYYY年MM月DD日"), TimeFormat("时辰刻"), " ")).c_str() << endl;
    
    // 更多刻的示例
    TimeOfDay noon(12, 0, 0, 0);          // 12:00 = 午时四刻
    TimeOfDay afternoon(12, 45, 0, 0);    // 12:45 = 午时七刻
    
    cout << "Noon (12:00): " << TimeToString(noon, TimeFormat("时辰刻")).c_str() << endl;
    cout << "12:45: " << TimeToString(afternoon, TimeFormat("时辰刻")).c_str() << endl;
    
    cout << endl;
    cout << "All tests completed!" << endl;
    
    return 0;
}
