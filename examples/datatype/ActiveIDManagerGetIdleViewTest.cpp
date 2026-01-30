/**
 * ActiveIDManagerGetIdleViewTest.cpp
 * 专门测试 ActiveIDManager::GetIdleView() 的崩溃问题
 *
 * 复现场景：
 * 1. Reserve
 * 2. CreateIdle
 * 3. 多次调用 GetIdleView()
 * 4. 观察 std::vector<> 临时对象的析构行为
 */

#include<hgl/type/ActiveIDManager.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

// 简单的调试输出函数
void DebugOutputIdleView(const char *hint, ActiveIDManager &aim)
{
    cout << "========================================" << endl;
    cout << hint << endl;
    cout << "========================================" << endl;

    cout << "  ActiveCount: " << aim.GetActiveCount() << endl;
    cout << "  IdleCount: " << aim.GetIdleCount() << endl;
    cout << "  TotalCount: " << aim.GetTotalCount() << endl;

    cout << "\n  Calling GetIdleView()..." << endl;

    // ✅ 使用 auto 接收返回值（std::vector<int>）
    auto idle_view = aim.GetIdleView();

    cout << "  idle_view Count: " << idle_view.size() << endl;
    cout << "  idle_view Data pointer: " << (void*)idle_view.data() << endl;

    if (!idle_view.empty()) {
        cout << "  Idle IDs: [";
        for (size_t i = 0; i < idle_view.size(); i++) {
            if (i > 0) cout << ", ";
            cout << idle_view[i];
        }
        cout << "]" << endl;
    } else {
        cout << "  Idle IDs: []" << endl;
    }

    cout << "  Exiting DebugOutputIdleView (idle_view will be destroyed)..." << endl;
    cout << "========================================" << endl << endl;
}

int main()
{
    cout << "========================================" << endl;
    cout << "ActiveIDManager GetIdleView() Test" << endl;
    cout << "========================================" << endl << endl;

    // Test 1: Empty idle list
    cout << "=== Test 1: Empty Idle List ===" << endl;
    {
        cout << "  Creating ActiveIDManager..." << endl;
        ActiveIDManager aim;

        cout << "  Calling Reserve(20)..." << endl;
        aim.Reserve(20);

        cout << "  Calling GetIdleView() on empty idle list..." << endl;
        DebugOutputIdleView("Empty idle list state", aim);

        cout << "  Test 1 scope ending..." << endl;
    }
    cout << "  Test 1 completed successfully." << endl << endl;

    // Test 2: With idle IDs (复现崩溃场景)
    cout << "=== Test 2: With Idle IDs (Crash Scenario) ===" << endl;
    {
        cout << "  Creating ActiveIDManager..." << endl;
        ActiveIDManager aim;

        cout << "  Calling Reserve(20)..." << endl;
        aim.Reserve(20);

        cout << "  Calling CreateIdle(5)..." << endl;
        int idle_ids[5];
        aim.CreateIdle(idle_ids, 5);

        cout << "  Created IDs: ";
        for (int i = 0; i < 5; ++i) cout << idle_ids[i] << " ";
        cout << endl << endl;

        cout << "  [FIRST CALL] Calling GetIdleView()..." << endl;
        DebugOutputIdleView("After CreateIdle(5) - First call", aim);

        cout << "  [SECOND CALL] Calling GetIdleView() again..." << endl;
        DebugOutputIdleView("After CreateIdle(5) - Second call", aim);

        cout << "  Test 2 scope ending (ActiveIDManager will be destroyed)..." << endl;
    }
    cout << "  Test 2 completed successfully." << endl << endl;

    // Test 3: Multiple GetIdleView() in nested scopes
    cout << "=== Test 3: Nested GetIdleView() Calls ===" << endl;
    {
        ActiveIDManager aim;
        aim.Reserve(20);

        int idle_ids[5];
        aim.CreateIdle(idle_ids, 5);

        cout << "  Outer scope - Calling GetIdleView()..." << endl;
        {
            auto view1 = aim.GetIdleView();
            cout << "    view1 count: " << view1.size() << endl;

            {
                auto view2 = aim.GetIdleView();
                cout << "    view2 count: " << view2.size() << endl;

                {
                    auto view3 = aim.GetIdleView();
                    cout << "    view3 count: " << view3.size() << endl;

                    cout << "    Destroying view3..." << endl;
                }
                cout << "    Destroying view2..." << endl;
            }
            cout << "    Destroying view1..." << endl;
        }
        cout << "  All nested views destroyed successfully." << endl;

        cout << "  Test 3 scope ending..." << endl;
    }
    cout << "  Test 3 completed successfully." << endl << endl;

    // Test 4: GetIdleView() after Get() operations
    cout << "=== Test 4: GetIdleView() After Get() ===" << endl;
    {
        ActiveIDManager aim;
        aim.Reserve(20);

        int idle_ids[5];
        aim.CreateIdle(idle_ids, 5);

        cout << "  Initial idle count: " << aim.GetIdleCount() << endl;
        DebugOutputIdleView("Before Get()", aim);

        int activated_ids[2];
        aim.Get(activated_ids, 2);

        cout << "  After Get(2), idle count: " << aim.GetIdleCount() << endl;
        DebugOutputIdleView("After Get(2)", aim);

        cout << "  Test 4 scope ending..." << endl;
    }
    cout << "  Test 4 completed successfully." << endl << endl;

    // Test 5: Stress test - many GetIdleView() calls
    cout << "=== Test 5: Stress Test - 100 GetIdleView() Calls ===" << endl;
    {
        ActiveIDManager aim;
        aim.Reserve(50);

        int idle_ids[10];
        aim.CreateIdle(idle_ids, 10);

        cout << "  Calling GetIdleView() 100 times..." << endl;
        for (int i = 0; i < 100; i++) {
            auto view = aim.GetIdleView();
            if (i % 20 == 0) {
                cout << "    Iteration " << i << ": count = " << view.size() << endl;
            }
        }

        cout << "  Test 5 scope ending..." << endl;
    }
    cout << "  Test 5 completed successfully." << endl << endl;

    cout << "========================================" << endl;
    cout << "All tests completed!" << endl;
    cout << "If crash occurs, check which test failed." << endl;
    cout << "========================================" << endl;

    return 0;
}
