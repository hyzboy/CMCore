#include<hgl/time/ChineseCalendar.h>
#include<hgl/time/DateTime.h>
#include<stdio.h>

namespace hgl
{
    // 天干（10个）
    static const char* HEAVENLY_STEMS[] = {
        "甲", "乙", "丙", "丁", "戊", "己", "庚", "辛", "壬", "癸"
    };
    
    // 地支（12个）
    static const char* EARTHLY_BRANCHES[] = {
        "子", "丑", "寅", "卯", "辰", "巳", "午", "未", "申", "酉", "戌", "亥"
    };
    
    // 生肖（12个，对应地支）
    static const char* ZODIAC_ANIMALS[] = {
        "鼠", "牛", "虎", "兔", "龙", "蛇", "马", "羊", "猴", "鸡", "狗", "猪"
    };
    
    // 24节气
    static const char* SOLAR_TERMS[] = {
        "立春", "雨水", "惊蛰", "春分", "清明", "谷雨",
        "立夏", "小满", "芒种", "夏至", "小暑", "大暑",
        "立秋", "处暑", "白露", "秋分", "寒露", "霜降",
        "立冬", "小雪", "大雪", "冬至", "小寒", "大寒"
    };
    
    // 月份名称
    static const char* MONTH_NAMES[] = {
        "", "正月", "二月", "三月", "四月", "五月", "六月",
        "七月", "八月", "九月", "十月", "冬月", "腊月"
    };
    
    // 日期名称前缀
    static const char* DAY_PREFIX[] = {
        "初", "十", "廿"
    };
    
    // 数字转中文
    static const char* DAY_NUMBERS[] = {
        "", "一", "二", "三", "四", "五", "六", "七", "八", "九", "十"
    };
    
    // ====================
    // ChineseCalendarDate
    // ====================
    
    ChineseCalendarDate::ChineseCalendarDate()
        : year(1900), month(1), day(1), reference_year(1900)
    {
    }
    
    ChineseCalendarDate::ChineseCalendarDate(int y, int m, int d)
        : year(y), month(m), day(d), reference_year(y)
    {
        // 简单验证
        if (month < 1) month = 1;
        if (month > 12) month = 12;
        if (day < 1) day = 1;
        if (day > 30) day = 30;
    }
    
    AnsiString ChineseCalendarDate::GetHeavenlyStemEarthlyBranch() const
    {
        // 天干地支纪年
        // 以公元4年为甲子年（甲子年的年份 mod 60 = 4）
        int cycle_year = (year - 4) % 60;
        if (cycle_year < 0) cycle_year += 60;
        
        int stem_index = cycle_year % 10;
        int branch_index = cycle_year % 12;
        
        AnsiString result = HEAVENLY_STEMS[stem_index];
        result = result + EARTHLY_BRANCHES[branch_index];
        
        return result;
    }
    
    AnsiString ChineseCalendarDate::GetHeavenlyStem() const
    {
        int cycle_year = (year - 4) % 60;
        if (cycle_year < 0) cycle_year += 60;
        return HEAVENLY_STEMS[cycle_year % 10];
    }
    
    AnsiString ChineseCalendarDate::GetEarthlyBranch() const
    {
        int cycle_year = (year - 4) % 60;
        if (cycle_year < 0) cycle_year += 60;
        return EARTHLY_BRANCHES[cycle_year % 12];
    }
    
    AnsiString ChineseCalendarDate::GetZodiac() const
    {
        int cycle_year = (year - 4) % 60;
        if (cycle_year < 0) cycle_year += 60;
        return ZODIAC_ANIMALS[cycle_year % 12];
    }
    
    AnsiString ChineseCalendarDate::GetSeason() const
    {
        // 简化的四季划分：每季度3个月
        if (month >= 1 && month <= 3)
            return "春";
        else if (month >= 4 && month <= 6)
            return "夏";
        else if (month >= 7 && month <= 9)
            return "秋";
        else
            return "冬";
    }
    
    int ChineseCalendarDate::GetSolarTermIndex() const
    {
        // 简化计算：每个月2个节气，每个节气15天
        // 360天/年 ÷ 24节气 = 15天/节气
        int day_of_year = (month - 1) * 30 + day;  // 1-360
        int term_index = (day_of_year - 1) / 15;   // 0-23
        
        if (term_index < 0) term_index = 0;
        if (term_index > 23) term_index = 23;
        
        return term_index;
    }
    
    AnsiString ChineseCalendarDate::GetSolarTermName() const
    {
        return SOLAR_TERMS[GetSolarTermIndex()];
    }
    
    AnsiString ChineseCalendarDate::GetMonthName() const
    {
        if (month < 1 || month > 12)
            return "正月";
        return MONTH_NAMES[month];
    }
    
    AnsiString ChineseCalendarDate::GetDayName() const
    {
        if (day < 1 || day > 30)
            return "初一";
        
        if (day <= 10)
        {
            // 初一到初十
            if (day == 10)
                return "初十";
            else
                return AnsiString(DAY_PREFIX[0]) + DAY_NUMBERS[day];
        }
        else if (day < 20)
        {
            // 十一到十九
            return AnsiString(DAY_PREFIX[1]) + DAY_NUMBERS[day - 10];
        }
        else if (day == 20)
        {
            return "二十";
        }
        else if (day < 30)
        {
            // 廿一到廿九
            return AnsiString(DAY_PREFIX[2]) + DAY_NUMBERS[day - 20];
        }
        else // day == 30
        {
            return "三十";
        }
    }
    
    // ====================
    // EraNameManager
    // ====================
    
    EraNameManager* EraNameManager::instance = nullptr;
    
    EraNameManager::EraNameManager()
        : eras(nullptr), era_count(0), era_capacity(0)
    {
        era_capacity = 50;
        eras = new EmperorEra[era_capacity];
    }
    
    EraNameManager::~EraNameManager()
    {
        if (eras)
        {
            delete[] eras;
            eras = nullptr;
        }
    }
    
    EraNameManager* EraNameManager::GetInstance()
    {
        // Note: 简化实现，非线程安全
        // 实际应用中应使用互斥锁或std::call_once保证线程安全
        if (!instance)
        {
            instance = new EraNameManager();
            instance->LoadDefaultEras();
        }
        return instance;
    }
    
    void EraNameManager::AddEra(const EmperorEra &era)
    {
        if (era_count >= era_capacity)
        {
            // 扩容
            int new_capacity = era_capacity * 2;
            EmperorEra *new_eras = new EmperorEra[new_capacity];
            
            for (int i = 0; i < era_count; i++)
                new_eras[i] = eras[i];
            
            delete[] eras;
            eras = new_eras;
            era_capacity = new_capacity;
        }
        
        eras[era_count++] = era;
    }
    
    const EmperorEra* EraNameManager::FindEra(int year, int reference_year) const
    {
        // 如果没有设置参考年份，直接按年份查找
        if (reference_year == 0)
            reference_year = year;
        
        // 从参考年份视角查找
        // 如果参考年份在年号有效期内，使用该年号
        // 如果参考年份在年号结束后，需要转换为后续年号
        
        for (int i = 0; i < era_count; i++)
        {
            const EmperorEra &era = eras[i];
            
            // 如果查询年份在此年号期间
            if (year >= era.start_year && year <= era.end_year)
            {
                // 如果参考年份也在此年号期间，直接使用
                if (reference_year >= era.start_year && reference_year <= era.end_year)
                    return &era;
                
                // 如果参考年份在此年号之后，查找参考年份对应的年号
                if (reference_year > era.end_year)
                {
                    // 使用参考年份所在的年号
                    for (int j = i + 1; j < era_count; j++)
                    {
                        if (reference_year >= eras[j].start_year && reference_year <= eras[j].end_year)
                            return &eras[j];
                    }
                }
                
                // 默认返回原年号
                return &era;
            }
        }
        
        return nullptr;
    }
    
    int EraNameManager::GetEraYear(int year, const EmperorEra *era) const
    {
        if (!era)
            return 1;
        
        int era_year = year - era->start_year + 1;
        if (era_year < 1)
            era_year = 1;
        
        return era_year;
    }
    
    void EraNameManager::LoadDefaultEras()
    {
        // 明朝部分年号示例
        AddEra(EmperorEra("洪武", 1368, 1398, "朱元璋", "明"));
        AddEra(EmperorEra("建文", 1399, 1402, "朱允炆", "明"));
        AddEra(EmperorEra("永乐", 1403, 1424, "朱棣", "明"));
        AddEra(EmperorEra("洪熙", 1425, 1425, "朱高炽", "明"));
        AddEra(EmperorEra("宣德", 1426, 1435, "朱瞻基", "明"));
        AddEra(EmperorEra("正统", 1436, 1449, "朱祁镇", "明"));
        AddEra(EmperorEra("景泰", 1450, 1456, "朱祁钰", "明"));
        AddEra(EmperorEra("天顺", 1457, 1464, "朱祁镇", "明"));  // 英宗复辟
        AddEra(EmperorEra("成化", 1465, 1487, "朱见深", "明"));
        AddEra(EmperorEra("弘治", 1488, 1505, "朱祐樘", "明"));
        AddEra(EmperorEra("正德", 1506, 1521, "朱厚照", "明"));
        AddEra(EmperorEra("嘉靖", 1522, 1566, "朱厚熜", "明"));
        AddEra(EmperorEra("隆庆", 1567, 1572, "朱载坖", "明"));
        AddEra(EmperorEra("万历", 1573, 1620, "朱翊钧", "明"));
        AddEra(EmperorEra("泰昌", 1620, 1620, "朱常洛", "明"));
        AddEra(EmperorEra("天启", 1621, 1627, "朱由校", "明"));
        AddEra(EmperorEra("崇祯", 1628, 1644, "朱由检", "明"));
        
        // 清朝部分年号示例
        AddEra(EmperorEra("顺治", 1644, 1661, "福临", "清"));
        AddEra(EmperorEra("康熙", 1662, 1722, "玄烨", "清"));
        AddEra(EmperorEra("雍正", 1723, 1735, "胤禛", "清"));
        AddEra(EmperorEra("乾隆", 1736, 1795, "弘历", "清"));
        AddEra(EmperorEra("嘉庆", 1796, 1820, "颙琰", "清"));
        AddEra(EmperorEra("道光", 1821, 1850, "旻宁", "清"));
        AddEra(EmperorEra("咸丰", 1851, 1861, "奕詝", "清"));
        AddEra(EmperorEra("同治", 1862, 1874, "载淳", "清"));
        AddEra(EmperorEra("光绪", 1875, 1908, "载湉", "清"));
        AddEra(EmperorEra("宣统", 1909, 1911, "溥仪", "清"));
    }
    
    // ====================
    // 格式化函数
    // ====================
    
    AnsiString FormatChineseDate(const ChineseCalendarDate &date, bool use_era_name, int reference_year)
    {
        AnsiString result;
        
        if (reference_year == 0)
            reference_year = date.GetReferenceYear();
        
        if (use_era_name)
        {
            // 使用年号
            EraNameManager *mgr = EraNameManager::GetInstance();
            const EmperorEra *era = mgr->FindEra(date.GetYear(), reference_year);
            
            if (era)
            {
                // 计算年号年份
                int era_year = mgr->GetEraYear(date.GetYear(), era);
                
                result = era->era_name;
                
                // 添加年份
                if (era_year == 1)
                    result = result + "元年";
                else
                {
                    char year_buf[20];
                    snprintf(year_buf, sizeof(year_buf), "%d年", era_year);
                    result = result + year_buf;
                }
            }
            else
            {
                // 没有年号，使用天干地支
                result = date.GetHeavenlyStemEarthlyBranch() + "年";
            }
        }
        else
        {
            // 使用天干地支
            result = date.GetHeavenlyStemEarthlyBranch() + "年";
        }
        
        // 添加月份和日期
        result = result + date.GetMonthName();
        result = result + date.GetDayName();
        
        return result;
    }
    
    AnsiString FormatChineseDateTime(const ChineseCalendarDate &date, const TimeOfDay &time, 
                                     bool use_era_name, int reference_year)
    {
        AnsiString result = FormatChineseDate(date, use_era_name, reference_year);
        
        // 添加时辰刻
        AnsiString time_str = TimeToString(time, TimeFormat("时辰刻"));
        result = result + time_str;
        
        return result;
    }
    
} // namespace hgl
