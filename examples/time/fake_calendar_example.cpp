#include <hgl/time/ChineseCalendar.h>
#include <hgl/time/DateTime.h>
#include <iostream>

using namespace hgl;
using namespace std;

/**
 * 虚假中国古代历法演示程序
 *
 * ⚠️⚠️⚠️ 重要警告 ⚠️⚠️⚠️
 *
 * 本程序使用的是一个虚构的简化历法系统，与真实的中国历法完全不同！
 *
 * 这个"虚假历法"采用了以下简化规则：
 * - 一年固定360天（真实历法有354-355天，加闰月）
 * - 每月固定30天（真实历法有29天或30天的大小月）
 * - 节气均匀分布（真实节气基于太阳黄经，分布不均匀）
 * - 天干地支简化计算（真实需考虑立春等因素）
 *
 * 本程序仅用于：
 * - 游戏开发中的古代时间表示
 * - 教学演示基本概念
 * - 不需要历史准确性的应用场景
 *
 * 请勿用于需要真实历史准确性的场合！
 *
 * 演示特性：
 * 1. 天干地支纪年
 * 2. 生肖
 * 3. 24节气
 * 4. 四季
 * 5. 月份名称（正月、腊月等）
 * 6. 日期名称（初一、十五等）
 * 7. 皇帝年号
 * 8. 年号随时间变化（历史视角）
 * 9. 完整格式化输出，如"永乐元年腊月初八午时三刻"
 */

int main(int, char**)
{
    cout << "========================================" << endl;
    cout << "虚假中国古代历法演示程序" << endl;
    cout << "⚠️ 注意：这不是真实的历法！" << endl;
    cout << "========================================" << endl << endl;

    // ====================
    // 示例1：基本功能演示
    // ====================
    cout << "=== 示例 1: 基本功能演示 ===" << endl;
    cout << "----------------------------------------" << endl;

    FakeChineseCalendarDate date1(1403, 12, 8);  // 永乐元年腊月初八
    TimeOfDay time1(11, 45, 0, 0);           // 午时三刻

    cout << "公历日期: 1403年12月8日 11:45" << endl;
    cout << "天干地支: " << date1.GetHeavenlyStemEarthlyBranch().c_str() << "年" << endl;
    cout << "生肖: " << date1.GetZodiac().c_str() << endl;
    cout << "季节: " << date1.GetSeason().c_str() << endl;
    cout << "节气: " << date1.GetSolarTermName().c_str() << endl;
    cout << "月份: " << date1.GetMonthName().c_str() << endl;
    cout << "日期: " << date1.GetDayName().c_str() << endl;
    cout << endl;

    // 完整格式化
    AnsiString full_format1 = FormatFakeChineseDateTime(date1, time1, true, 0);
    cout << "完整格式: " << full_format1.c_str() << endl;
    cout << endl << endl;

    // ====================
    // 示例2：不同年号的日期
    // ====================
    cout << "=== 示例 2: 不同朝代年号 ===" << endl;
    cout << "----------------------------------------" << endl;

    struct DateExample {
        int year, month, day;
        int hour, minute;
        const char* description;
    };

    DateExample examples[] = {
        {1368, 1, 1, 12, 0, "明朝建立 - 洪武元年"},
        {1399, 7, 15, 14, 30, "建文帝即位"},
        {1403, 12, 8, 11, 45, "永乐元年"},
        {1644, 3, 19, 11, 45, "崇祯末年"},
        {1662, 2, 5, 9, 15, "康熙元年"},
        {1736, 10, 18, 15, 30, "乾隆元年"},
    };

    for (int i = 0; i < 6; i++)
    {
        FakeChineseCalendarDate d(examples[i].year, examples[i].month, examples[i].day);
        TimeOfDay t(examples[i].hour, examples[i].minute, 0, 0);

        AnsiString formatted = FormatFakeChineseDateTime(d, t, true, 0);

        cout << "公历: " << examples[i].year << "-" << examples[i].month << "-" << examples[i].day
             << " " << examples[i].hour << ":" << (examples[i].minute < 10 ? "0" : "") << examples[i].minute << endl;
        cout << "  描述: " << examples[i].description << endl;
        cout << "  古历: " << formatted.c_str() << endl;
        cout << endl;
    }

    cout << endl;

    // ====================
    // 示例3：年号随时间变化
    // ====================
    cout << "=== 示例 3: 年号的历史视角转换 ===" << endl;
    cout << "----------------------------------------" << endl;
    cout << "演示：建文四年在不同时间点的称呼变化" << endl;
    cout << endl;

    FakeChineseCalendarDate jianwen4(1402, 6, 15);  // 建文四年六月十五
    jianwen4.SetReferenceYear(1402);
    TimeOfDay event_time(14, 0, 0, 0);

    // 在建文四年当时
    cout << "在建文四年（1402年）称呼此日期：" << endl;
    AnsiString format_in_era = FormatFakeChineseDateTime(jianwen4, event_time, true, 1402);
    cout << "  " << format_in_era.c_str() << endl;
    cout << endl;

    // 到了永乐年间的称呼
    cout << "到了永乐年间（1403年后）称呼同一日期：" << endl;
    AnsiString format_after = FormatFakeChineseDateTime(jianwen4, event_time, true, 1410);
    cout << "  " << format_after.c_str() << endl;
    cout << "  （注：史书中会改称为洪武年间，因建文帝被废）" << endl;
    cout << endl << endl;

    // ====================
    // 示例4：24节气演示
    // ====================
    cout << "=== 示例 4: 24节气演示 ===" << endl;
    cout << "----------------------------------------" << endl;

    cout << "节气演示（360天历法，每节气15天）：" << endl;
    cout << endl;

    for (int month = 1; month <= 12; month++)
    {
        FakeChineseCalendarDate d1(1403, month, 1);
        FakeChineseCalendarDate d2(1403, month, 16);

        cout << d1.GetMonthName().c_str() << ": "
             << d1.GetSolarTermName().c_str() << " / "
             << d2.GetSolarTermName().c_str() << endl;
    }

    cout << endl << endl;

    // ====================
    // 示例5：不使用年号（使用天干地支）
    // ====================
    cout << "=== 示例 5: 天干地支纪年法 ===" << endl;
    cout << "----------------------------------------" << endl;

    FakeChineseCalendarDate date5(1644, 3, 19);
    TimeOfDay time5(11, 45, 0, 0);

    AnsiString with_era = FormatFakeChineseDateTime(date5, time5, true, 0);
    AnsiString without_era = FormatFakeChineseDateTime(date5, time5, false, 0);

    cout << "同一日期的不同表达：" << endl;
    cout << "  使用年号: " << with_era.c_str() << endl;
    cout << "  天干地支: " << without_era.c_str() << endl;
    cout << endl << endl;

    // ====================
    // 示例6：一年四季
    // ====================
    cout << "=== 示例 6: 四季变化 ===" << endl;
    cout << "----------------------------------------" << endl;

    for (int m = 1; m <= 12; m += 3)
    {
        FakeChineseCalendarDate season_date(1403, m, 15);
        cout << season_date.GetMonthName().c_str() << " - "
             << season_date.GetSeason().c_str() << "季" << endl;
    }

    cout << endl << endl;

    // ====================
    // 示例7：完整的时间描述示例
    // ====================
    cout << "=== 示例 7: 实际应用场景 ===" << endl;
    cout << "----------------------------------------" << endl;

    struct ScenarioExample {
        int year, month, day, hour, minute;
        const char* event;
    };

    ScenarioExample scenarios[] = {
        {1403, 12, 8, 11, 45, "大朝会"},
        {1406, 7, 11, 14, 0, "郑和首次下西洋启程"},
        {1421, 1, 1, 9, 0, "迁都北京"},
        {1722, 12, 20, 11, 45, "雍正登基"},
    };

    for (int i = 0; i < 4; i++)
    {
        FakeChineseCalendarDate d(scenarios[i].year, scenarios[i].month, scenarios[i].day);
        TimeOfDay t(scenarios[i].hour, scenarios[i].minute, 0, 0);

        AnsiString formatted = FormatFakeChineseDateTime(d, t, true, 0);

        cout << "事件: " << scenarios[i].event << endl;
        cout << "  时间: " << formatted.c_str() << endl;
        cout << endl;
    }

    cout << "========================================" << endl;
    cout << "演示完成！" << endl;
    cout << "========================================" << endl;

    return 0;
}
