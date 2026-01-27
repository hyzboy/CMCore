#include <hgl/time/DateTime.h>
#include <hgl/time/Time.h>
#include <iostream>

using namespace hgl;
using namespace std;

/**
 * 演示程序：自动生成古代处决公告
 *
 * 从数字格式的日期时间数据自动合成中文句子
 * 例如：1900年3月15日 11:45 -> "囚犯斩于一九〇〇年三月十五日午时三刻"
 */

// 数字转中文数字 (简化版本，用于年月日)
AnsiString NumberToChinese(int num, bool use_zero_ling = true)
{
    const char* digits[] = {"〇", "一", "二", "三", "四", "五", "六", "七", "八", "九"};

    if (num == 0)
        return use_zero_ling ? "〇" : "零";

    AnsiString result;
    AnsiString num_str;

    // 转换为字符串
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%d", num);
    num_str = buffer;

    // 逐位转换为中文
    size_t len = strlen(num_str.c_str());
    for (size_t i = 0; i < len; i++)
    {
        int digit = num_str.c_str()[i] - '0';
        result = result + digits[digit];
    }

    return result;
}

// 生成处决公告句子
AnsiString GenerateExecutionSentence(const CalendarDate &date, const TimeOfDay &time)
{
    int year = date.GetYear();
    int month = date.GetMonth();
    int day = date.GetDay();

    // 年份 - 每位数字单独转换
    AnsiString year_str = NumberToChinese(year, true);

    // 月份 - 使用中文数字 (1-12)
    const char* month_chinese[] = {
        "", "一", "二", "三", "四", "五", "六",
        "七", "八", "九", "十", "十一", "十二"
    };

    // 确保月份在有效范围内
    if (month < 1 || month > 12)
        month = 1;  // 默认值

    AnsiString month_str = month_chinese[month];

    // 日期 - 使用中文数字
    AnsiString day_str;
    if (day <= 10)
    {
        if (day == 10)
            day_str = "十";
        else
            day_str = month_chinese[day];  // 复用月份的数字
    }
    else if (day < 20)
    {
        day_str = AnsiString("十") + month_chinese[day - 10];
    }
    else if (day == 20)
    {
        day_str = "二十";
    }
    else if (day < 30)
    {
        day_str = AnsiString("二十") + month_chinese[day - 20];
    }
    else if (day == 30)
    {
        day_str = "三十";
    }
    else  // 31
    {
        day_str = "三十一";
    }

    // 时辰刻 - 使用格式化功能
    AnsiString time_str = TimeToString(time, TimeFormat("时辰刻"));

    // 合成完整句子
    AnsiString sentence = "囚犯斩于";
    sentence = sentence + year_str + "年";
    sentence = sentence + month_str + "月";
    sentence = sentence + day_str + "日";
    sentence = sentence + time_str;

    return sentence;
}

int main(int, char**)
{
    cout << "========================================" << endl;
    cout << "古代处决公告生成器" << endl;
    cout << "========================================" << endl << endl;

    // 示例1：经典的午时三刻
    cout << "示例 1: 经典午时三刻处决" << endl;
    cout << "----------------------------------------" << endl;

    CalendarDate date1(1900, 3, 15);
    TimeOfDay time1(11, 45, 0, 0);  // 午时三刻

    cout << "输入数据: " << endl;
    cout << "  日期: " << date1.GetYear() << "年" << date1.GetMonth() << "月" << date1.GetDay() << "日" << endl;
    cout << "  时间: " << time1.GetHour() << ":" << (time1.GetMinute() < 10 ? "0" : "") << time1.GetMinute() << endl;
    cout << endl;

    AnsiString sentence1 = GenerateExecutionSentence(date1, time1);
    cout << "生成句子: " << sentence1.c_str() << endl;
    cout << endl << endl;

    // 示例2：子时处决
    cout << "示例 2: 子时处决" << endl;
    cout << "----------------------------------------" << endl;

    CalendarDate date2(1850, 12, 1);
    TimeOfDay time2(23, 30, 0, 0);  // 子时二刻

    cout << "输入数据: " << endl;
    cout << "  日期: " << date2.GetYear() << "年" << date2.GetMonth() << "月" << date2.GetDay() << "日" << endl;
    cout << "  时间: " << time2.GetHour() << ":" << (time2.GetMinute() < 10 ? "0" : "") << time2.GetMinute() << endl;
    cout << endl;

    AnsiString sentence2 = GenerateExecutionSentence(date2, time2);
    cout << "生成句子: " << sentence2.c_str() << endl;
    cout << endl << endl;

    // 示例3：未时处决
    cout << "示例 3: 未时处决" << endl;
    cout << "----------------------------------------" << endl;

    CalendarDate date3(1888, 8, 28);
    TimeOfDay time3(14, 0, 0, 0);  // 未时四刻

    cout << "输入数据: " << endl;
    cout << "  日期: " << date3.GetYear() << "年" << date3.GetMonth() << "月" << date3.GetDay() << "日" << endl;
    cout << "  时间: " << time3.GetHour() << ":" << (time3.GetMinute() < 10 ? "0" : "") << time3.GetMinute() << endl;
    cout << endl;

    AnsiString sentence3 = GenerateExecutionSentence(date3, time3);
    cout << "生成句子: " << sentence3.c_str() << endl;
    cout << endl << endl;

    // 示例4：展示多个不同时间的处决
    cout << "示例 4: 多个不同时间点" << endl;
    cout << "----------------------------------------" << endl;

    struct ExecutionRecord {
        int year, month, day;
        int hour, minute;
    };

    ExecutionRecord records[] = {
        {1800, 1, 1, 11, 45},   // 午时三刻
        {1820, 5, 10, 12, 0},   // 午时四刻
        {1840, 6, 20, 14, 30},  // 未时六刻
        {1860, 10, 30, 18, 0},  // 酉时四刻
    };

    for (int i = 0; i < 4; i++)
    {
        CalendarDate d(records[i].year, records[i].month, records[i].day);
        TimeOfDay t(records[i].hour, records[i].minute, 0, 0);

        cout << "记录 " << (i + 1) << ": "
             << records[i].year << "-" << records[i].month << "-" << records[i].day
             << " " << records[i].hour << ":" << (records[i].minute < 10 ? "0" : "") << records[i].minute
             << endl;

        AnsiString sentence = GenerateExecutionSentence(d, t);
        cout << "  -> " << sentence.c_str() << endl;
    }

    cout << endl;
    cout << "========================================" << endl;
    cout << "演示完成！" << endl;
    cout << "========================================" << endl;

    return 0;
}
