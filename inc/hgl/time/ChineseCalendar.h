#ifndef HGL_TIME_CHINESE_CALENDAR_INCLUDE
#define HGL_TIME_CHINESE_CALENDAR_INCLUDE

#include<hgl/type/String.h>
#include<hgl/time/DateTime.h>

namespace hgl
{
    /**
     * 虚假的简化中国古代历法类
     * 
     * ⚠️ 警告：这是一个虚构的简化历法系统，与真实的中国历法完全不同！
     * 
     * 本类采用简化规则用于演示和游戏场景：
     * - 固定一年360天，12个月，每月30天（真实历法有闰月和大小月）
     * - 天干地支采用简化计算（真实历法需考虑立春等因素）
     * - 24节气均匀分布（真实节气基于太阳黄经）
     * - 皇帝年号简化处理（真实历史更复杂）
     * 
     * 本类仅用于：
     * - 游戏开发中的古代时间表示
     * - 教学演示
     * - 不需要精确历史对应的应用
     * 
     * 请勿用于需要真实历史准确性的场合！
     */
    
    /**
     * 皇帝年号定义
     */
    struct EmperorEra
    {
        AnsiString era_name;       ///< 年号名称，如"永乐"
        int start_year;            ///< 年号开始年份（公历）
        int end_year;              ///< 年号结束年份（公历）
        AnsiString emperor_name;   ///< 皇帝名称
        AnsiString dynasty;        ///< 朝代名称，如"明"
        
        EmperorEra() : start_year(0), end_year(0) {}
        EmperorEra(const AnsiString &era, int start, int end, const AnsiString &emp = "", const AnsiString &dyn = "")
            : era_name(era), start_year(start), end_year(end), emperor_name(emp), dynasty(dyn) {}
    };
    
    /**
     * 虚假的简化中国古代历法日期类
     * 
     * ⚠️ 注意：这不是真实的中国历法！这是一个简化的虚构系统。
     */
    class FakeChineseCalendarDate
    {
    private:
        int year;          ///< 年份（公历年份）
        int month;         ///< 月份 (1-12)
        int day;           ///< 日期 (1-30)
        
        // 年号上下文（用于历史视角转换）
        int reference_year; ///< 参考年份（当前时间点），用于年号转换
        
    public:
        FakeChineseCalendarDate();
        FakeChineseCalendarDate(int y, int m, int d);
        
        int GetYear() const { return year; }
        int GetMonth() const { return month; }
        int GetDay() const { return day; }
        
        void SetReferenceYear(int ref_year) { reference_year = ref_year; }
        int GetReferenceYear() const { return reference_year; }
        
        // 天干地支纪年
        AnsiString GetHeavenlyStemEarthlyBranch() const;
        
        // 获取天干
        AnsiString GetHeavenlyStem() const;
        
        // 获取地支
        AnsiString GetEarthlyBranch() const;
        
        // 获取生肖
        AnsiString GetZodiac() const;
        
        // 获取季节
        AnsiString GetSeason() const;
        
        // 获取节气索引 (0-23)
        int GetSolarTermIndex() const;
        
        // 获取节气名称
        AnsiString GetSolarTermName() const;
        
        // 月份名称（如"正月"、"腊月"）
        AnsiString GetMonthName() const;
        
        // 日期名称（如"初一"、"初八"、"十五"）
        AnsiString GetDayName() const;
    };
    
    /**
     * 年号管理器
     */
    class EraNameManager
    {
    private:
        static EraNameManager *instance;
        
        // 年号列表（按时间顺序）
        EmperorEra *eras;
        int era_count;
        int era_capacity;
        
        EraNameManager();
        ~EraNameManager();
        
    public:
        static EraNameManager* GetInstance();
        
        // 添加年号
        void AddEra(const EmperorEra &era);
        
        // 根据年份查找年号（从参考年份视角）
        const EmperorEra* FindEra(int year, int reference_year) const;
        
        // 获取年号年份（如"永乐元年"的"元年"）
        int GetEraYear(int year, const EmperorEra *era) const;
        
        // 加载预定义的年号表
        void LoadDefaultEras();
    };
    
    /**
     * 格式化虚假的中国古代历法日期和时间
     * 
     * ⚠️ 注意：这是虚构的简化格式，与真实历史不符！
     * 
     * 示例：
     * - "永乐元年腊月初八午时三刻"
     * - "甲子年冬月十五申时初刻"
     */
    AnsiString FormatFakeChineseDateTime(const FakeChineseCalendarDate &date, const TimeOfDay &time, 
                                         bool use_era_name = true, int reference_year = 0);
    
    /**
     * 仅格式化虚假的中国古代历法日期
     * 
     * ⚠️ 注意：这是虚构的简化格式，与真实历史不符！
     * 
     * 示例：
     * - "永乐元年腊月初八"
     * - "甲子年冬月十五"
     */
    AnsiString FormatFakeChineseDate(const FakeChineseCalendarDate &date, bool use_era_name = true, int reference_year = 0);
    
} // namespace hgl

#endif // HGL_TIME_CHINESE_CALENDAR_INCLUDE
