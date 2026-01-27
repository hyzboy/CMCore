#ifndef HGL_TIME_CHINESE_CALENDAR_INCLUDE
#define HGL_TIME_CHINESE_CALENDAR_INCLUDE

#include<hgl/type/String.h>
#include<hgl/time/DateTime.h>

namespace hgl
{
    /**
     * 简化的中国古代历法类
     * 
     * 特点：
     * - 固定一年360天，12个月，每月30天
     * - 支持天干地支纪年
     * - 支持24节气
     * - 支持四季
     * - 支持皇帝年号（可配置表）
     * - 年号随时间变化（历史视角）
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
     * 简化的中国古代历法日期类
     */
    class ChineseCalendarDate
    {
    private:
        int year;          ///< 年份（公历年份）
        int month;         ///< 月份 (1-12)
        int day;           ///< 日期 (1-30)
        
        // 年号上下文（用于历史视角转换）
        int reference_year; ///< 参考年份（当前时间点），用于年号转换
        
    public:
        ChineseCalendarDate();
        ChineseCalendarDate(int y, int m, int d);
        
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
     * 格式化中国古代历法日期和时间
     * 
     * 示例：
     * - "永乐元年腊月初八午时三刻"
     * - "甲子年冬月十五申时初刻"
     */
    AnsiString FormatChineseDateTime(const ChineseCalendarDate &date, const TimeOfDay &time, 
                                     bool use_era_name = true, int reference_year = 0);
    
    /**
     * 仅格式化中国古代历法日期
     * 
     * 示例：
     * - "永乐元年腊月初八"
     * - "甲子年冬月十五"
     */
    AnsiString FormatChineseDate(const ChineseCalendarDate &date, bool use_era_name = true, int reference_year = 0);
    
} // namespace hgl

#endif // HGL_TIME_CHINESE_CALENDAR_INCLUDE
