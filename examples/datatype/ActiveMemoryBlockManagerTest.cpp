#include <iostream>
#include <vector>
#include <cassert>
#include <hgl/type/ActiveMemoryBlockManager.h>
#include <hgl/type/MemoryBlock.h>
#include "UserInfo.h"

using namespace hgl;
using namespace std;

static void WriteUsername(ActiveMemoryBlockManager &ambm, const int *ids, int count, int user_start)
{
    for (int i = 0; i < count; ++i)
        ambm.WriteData(user_info_array[user_start + i].name, ids[i]);
}

static void ReadUsername(ActiveMemoryBlockManager &ambm, const int *ids, int count, vector<string> &out)
{
    out.resize(count);
    for (int i = 0; i < count; ++i)
    {
        char buf[64] = {};
        bool ok = ambm.GetData(buf, ids[i]);
        assert(ok);
        out[i] = buf;
    }
}

int main(int, char **)
{
    // 1) unit_size==0 防御：不允许写入/读取
    {
        cout << "=== Test 1: unit_size==0 defense ===" << endl;
        ActiveMemoryBlockManager bad(0);
        int dummy = 0;

        cout << "Calling SetUnitSize(0)..." << endl;
        bool result1 = bad.SetUnitSize(0);
        cout << "SetUnitSize(0) returned: " << result1 << " (expected: false)" << endl;
        assert(!result1);

        cout << "Calling SetUnitSize(8)..." << endl;
        bool result2 = bad.SetUnitSize(8);
        cout << "SetUnitSize(8) returned: " << result2 << " (expected: true)" << endl;
        assert(result2);          // 首次设置为8

        cout << "Calling SetUnitSize(4)..." << endl;
        bool result3 = bad.SetUnitSize(4);
        cout << "SetUnitSize(4) returned: " << result3 << " (expected: false)" << endl;
        assert(!result3);         // 已设置后拒绝不同值

        cout << "Calling WriteData(&dummy, 0)..." << endl;
        bool result4 = bad.WriteData(&dummy, 0);
        cout << "WriteData(&dummy, 0) returned: " << result4 << " (expected: false)" << endl;
        cout << "dummy address: " << &dummy << ", id: 0" << endl;
        assert(!result4);   // unit_size为0时返回false

        cout << "Test 1 passed." << endl << endl;
    }

    cout << "=== Test 2: Normal operations ===" << endl;
    ActiveMemoryBlockManager ambm(8);
    ambm.Reserve(16);

    assert(ambm.GetActiveCount() == 0);
    assert(ambm.GetIdleCount() == 0);
    assert(ambm.GetTotalCount() == 0);

    // 2) CreateIdle 并写入数据
    int idle_ids[5];
    int created = ambm.CreateIdle(idle_ids, 5);
    assert(created == 5);
    assert(ambm.GetIdleCount() == 5);
    WriteUsername(ambm, idle_ids, 5, 0);

    // 3) CreateActive 直接新建并写数据
    int active_ids[5];
    created = ambm.CreateActive(active_ids, 5);
    assert(created == 5);
    assert(ambm.GetActiveCount() == 5);
    WriteUsername(ambm, active_ids, 5, 5);

    // 验证当前数据
    {
        vector<string> names;
        ReadUsername(ambm, idle_ids, 5, names);
        for (int i = 0; i < 5; ++i) assert(names[i] == user_info_array[i].name);
        ReadUsername(ambm, active_ids, 5, names);
        for (int i = 0; i < 5; ++i) assert(names[i] == user_info_array[5 + i].name);
    }

    // 4) Get 从 idle 获取，不创建新 ID
    cout << "=== Test 4: Get from idle (FIFO/Queue behavior) ===" << endl;
    cout << "Before Get - Active: " << ambm.GetActiveCount() << ", Idle: " << ambm.GetIdleCount() << endl;
    cout << "idle_ids created: [";
    for (int i = 0; i < 5; ++i) cout << idle_ids[i] << (i < 4 ? ", " : "");
    cout << "]" << endl;
    cout << "  - idle_ids[0]=" << idle_ids[0] << " -> " << user_info_array[0].name << endl;
    cout << "  - idle_ids[1]=" << idle_ids[1] << " -> " << user_info_array[1].name << endl;
    cout << "  - idle_ids[2]=" << idle_ids[2] << " -> " << user_info_array[2].name << endl;
    cout << "  - idle_ids[3]=" << idle_ids[3] << " -> " << user_info_array[3].name << endl;
    cout << "  - idle_ids[4]=" << idle_ids[4] << " -> " << user_info_array[4].name << endl;

    int user_ids[3];
    bool got = ambm.Get(user_ids, 3);
    cout << "Get(user_ids, 3) returned: " << got << endl;
    cout << "user_ids retrieved: [";
    for (int i = 0; i < 3; ++i) cout << user_ids[i] << (i < 2 ? ", " : "");
    cout << "]" << endl;
    cout << "After Get - Active: " << ambm.GetActiveCount() << ", Idle: " << ambm.GetIdleCount() << endl;

    assert(got);
    assert(ambm.GetActiveCount() == 8); // 原有5活跃 + 3新取
    assert(ambm.GetIdleCount() == 2);

    // 验证取出的是从 idle 开头的数据（Queue的FIFO行为）
    // idle中有[0,1,2,3,4]，Get(3)会取出[0,1,2]
    {
        vector<string> names;
        ReadUsername(ambm, user_ids, 3, names);

        cout << "Data read from user_ids (Queue Get from front):" << endl;
        for (int i = 0; i < 3; ++i) {
            cout << "  user_ids[" << i << "]=" << user_ids[i]
                 << " -> \"" << names[i] << "\"" << endl;
        }

        cout << "Expected data (user_info_array[0-2], from idle list FRONT):" << endl;
        for (int i = 0; i < 3; ++i) {
            cout << "  user_info_array[" << i << "].name = \""
                 << user_info_array[i].name << "\"" << endl;
        }

        // 由于 idle_list 使用 Queue (FIFO)，Get() 从开头取出
        // 所以取出的应该是 user_info_array[0,1,2]
        for (int i = 0; i < 3; ++i) {
            cout << "Checking: names[" << i << "]=\"" << names[i]
                 << "\" == user_info_array[" << i << "]=\""
                 << user_info_array[i].name << "\" -> "
                 << (names[i] == user_info_array[i].name ? "PASS" : "FAIL") << endl;
            assert(names[i] == user_info_array[i].name);
        }
    }

    cout << "Test 4 passed." << endl << endl;

    // 5) GetOrCreate：先耗尽 idle，再创建新 ID
    int more_ids[6];
    got = ambm.GetOrCreate(more_ids, 6);
    assert(got);
    assert(ambm.GetIdleCount() == 0);
    assert(ambm.GetActiveCount() == 14); // 8 + 6

    // 对新获取的 ID 写入数据（其中一部分来自新创建）
    WriteUsername(ambm, more_ids, 6, 10);

    // 6) Release 部分 active 回 idle
    int released = ambm.Release(user_ids, 3);
    assert(released == 3);
    assert(ambm.GetActiveCount() == 11);
    assert(ambm.GetIdleCount() == 3);

    // 验证释放的 ID 现在可重新 Get，并数据保持
    // 注意：Release 把 ID 推回栈，Get 再次从栈顶取出
    int reuse_ids[3];
    got = ambm.Get(reuse_ids, 3);
    assert(got);
    assert(ambm.GetActiveCount() == 14);
    assert(ambm.GetIdleCount() == 0);
    {
        cout << "=== Test 6b: Reusing released IDs (Queue FIFO) ===" << endl;
        cout << "Released user_ids (were from idle[0,1,2]): [";
        for (int i = 0; i < 3; ++i) cout << user_ids[i] << (i < 2 ? ", " : "");
        cout << "]" << endl;
        cout << "Reuse Get returned reuse_ids: [";
        for (int i = 0; i < 3; ++i) cout << reuse_ids[i] << (i < 2 ? ", " : "");
        cout << "]" << endl;

        vector<string> names;
        ReadUsername(ambm, reuse_ids, 3, names);

        cout << "Data in reuse_ids:" << endl;
        for (int i = 0; i < 3; ++i) {
            cout << "  reuse_ids[" << i << "]=" << reuse_ids[i]
                 << " -> \"" << names[i] << "\"" << endl;
        }

        // user_ids[0,1,2] 被 Release，以相同顺序推入 idle queue
        // Get(3) 会以 FIFO 顺序取出，所以顺序保持
        // 数据应该还是对应 user_info_array[0,1,2]
        for (int i = 0; i < 3; ++i) {
            cout << "Verifying reuse_ids[" << i << "] data matches original..." << endl;
            assert(names[i] == user_info_array[i].name);
        }
        cout << "Reused IDs maintain their data correctly." << endl << endl;
    }

    // 7) ReleaseAllActive 全部转 idle
    released = ambm.ReleaseAllActive();
    assert(released == 14);
    assert(ambm.GetActiveCount() == 0);
    assert(ambm.GetIdleCount() == 14);

    // 8) 无效输入的健壮性
    {
        cout << "=== Test 8: Invalid input robustness ===" << endl;
        int ids[2] = { -1, 9999 };
        vector<string> names;

        // GetData/WriteData 对无效 ID 返回 false
        cout << "Testing WriteData(nullptr, 0)..." << endl;
        bool result1 = ambm.WriteData(nullptr, 0);
        cout << "  Result: " << result1 << " (expected: false)" << endl;
        assert(!result1);

        cout << "Testing WriteData(&ids, -1)..." << endl;
        bool result2 = ambm.WriteData(&ids, -1);
        cout << "  Result: " << result2 << " (expected: false)" << endl;
        assert(!result2);

        cout << "Testing GetData(nullptr, 0)..." << endl;
        bool result3 = ambm.GetData(nullptr, 0);
        cout << "  Result: " << result3 << " (expected: false)" << endl;
        assert(!result3);

        cout << "Testing GetData(ids, -1)..." << endl;
        bool result4 = ambm.GetData(ids, -1);
        cout << "  Result: " << result4 << " (expected: false)" << endl;
        assert(!result4);

        // Release 空输入返回0
        cout << "Testing Release(nullptr, 0)..." << endl;
        int released = ambm.Release(nullptr, 0);
        cout << "  Result: " << released << " (expected: 0)" << endl;
        assert(released == 0);

        cout << "Test 8 passed." << endl << endl;
    }

    cout << "All ActiveMemoryBlockManager tests passed." << endl;
    return 0;
}
