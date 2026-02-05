/**
 * @file StaticPerfectHashMapPrototype.cpp
 * @brief 静态完美哈希映射 - 最小原型测试
 */
#include <hgl/type/StaticPerfectHashMapBuilder.h>
#include <hgl/platform/Platform.h>
#include <iostream>
#include <chrono>
#include <iomanip>

using namespace hgl;
using namespace std;

struct SkillConfig
{
    uint32_t damage;
    uint32_t mana_cost;
    uint32_t cooldown;
    uint32_t range;
};

int os_main(int, os_char**)
{
    cout << "\n╔════════════════════════════════════════════════════════╗" << endl;
    cout << "║   静态完美哈希映射 - 最小原型演示                       ║" << endl;
    cout << "╚════════════════════════════════════════════════════════╝" << endl;

    vector<uint8_t> game_data_buffer;
    
    // 阶段1：编辑器构建
    {
        StaticPerfectHashMapBuilder<uint32_t, SkillConfig> builder;
        
        cout << "\n[编辑器阶段] 添加技能配置..." << endl;
        const int NUM_SKILLS = 1000;
        for (int i = 0; i < NUM_SKILLS; ++i)
        {
            uint32_t skill_id = 10000 + i;
            SkillConfig config;
            config.damage = 100 + i % 500;
            config.mana_cost = 50 + i % 200;
            config.cooldown = 5 + i % 20;
            config.range = 10 + i % 30;
            builder.Add(skill_id, config);
        }

        cout << "[编辑器阶段] 构建完美哈希..." << endl;
        auto build_start = chrono::high_resolution_clock::now();
        bool success = builder.Build();
        auto build_end = chrono::high_resolution_clock::now();
        
        if (!success)
        {
            cout << "✗ 构建失败！" << endl;
            return 1;
        }

        auto build_time = chrono::duration_cast<chrono::milliseconds>(build_end - build_start);
        auto stats = builder.GetStats();
        
        cout << "  ✓ 构建成功！用时: " << build_time.count() << "ms" << endl;
        cout << "  键数量: " << stats.num_keys << endl;
        cout << "  桶数量: " << stats.num_buckets << endl;
        cout << "  空间开销: " << fixed << setprecision(1) 
             << stats.space_overhead << "%" << endl;

        cout << "[编辑器阶段] 保存数据..." << endl;
        builder.SaveToMemory(game_data_buffer);
        cout << "  ✓ 保存了 " << setprecision(2) << game_data_buffer.size() / 1024.0 << " KB" << endl;
    }

    // 阶段2：游戏运行时
    {
        StaticPerfectHashMap<uint32_t, SkillConfig> skills;

        cout << "\n[游戏运行时] 加载技能配置..." << endl;
        auto load_start = chrono::high_resolution_clock::now();
        bool loaded = skills.LoadFromMemory(game_data_buffer.data(), game_data_buffer.size());
        auto load_end = chrono::high_resolution_clock::now();
        auto load_time = chrono::duration_cast<chrono::microseconds>(load_end - load_start);

        if (!loaded)
        {
            cout << "✗ 加载失败！" << endl;
            return 1;
        }

        cout << "  ✓ 加载成功！用时: " << load_time.count() << "μs" << endl;

        cout << "\n[游戏运行时] 性能测试..." << endl;
        auto lookup_start = chrono::high_resolution_clock::now();
        int found = 0;
        for (int i = 0; i < 1000; ++i)
        {
            uint32_t skill_id = 10000 + (i * 7) % 1000;
            if (skills.Get(skill_id))
                found++;
        }
        auto lookup_end = chrono::high_resolution_clock::now();
        auto lookup_time = chrono::duration_cast<chrono::nanoseconds>(lookup_end - lookup_start);

        cout << "  1000次查找，找到: " << found << endl;
        cout << "  平均查找时间: " << lookup_time.count() / 1000 << " ns" << endl;

        cout << "\n[游戏运行时] 验证零碰撞..." << endl;
        bool all_ok = true;
        for (uint32_t id = 10000; id < 11000; ++id)
        {
            if (!skills.Contains(id))
            {
                all_ok = false;
                break;
            }
        }
        cout << "  " << (all_ok ? "✓" : "✗") << " 所有1000个键都能正确查找" << endl;

        cout << "\n[游戏运行时] 查找示例：" << endl;
        const SkillConfig* cfg = skills.Get(10500);
        if (cfg)
        {
            cout << "  技能ID 10500: 伤害=" << cfg->damage
                 << ", 魔法=" << cfg->mana_cost << endl;
        }
    }

    cout << "\n╔════════════════════════════════════════════════════════╗" << endl;
    cout << "║   ✓ 原型演示完成！                                      ║" << endl;
    cout << "╚════════════════════════════════════════════════════════╝\n" << endl;

    return 0;
}
