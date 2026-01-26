/**
 * QueueTest04_UnreadSnapshot.cpp
 * 测试 Queue::GetUnreadSnapshot() 功能
 * 
 * 重点验证：
 * 1. 空队列返回空 snapshot
 * 2. 只有写段数据的 snapshot
 * 3. 读段+写段合并的 snapshot
 * 4. snapshot 是独立副本（不影响原队列）
 * 5. snapshot 的析构是否正常（无 double free）
 */

#include<hgl/type/Queue.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

void TestEmptySnapshot()
{
    cout << "\n=== Test 1: Empty Queue Snapshot ===" << endl;
    
    Queue<int> queue;
    
    {
        ValueBuffer<int> snapshot = queue.GetUnreadSnapshot();
        
        cout << "  Queue count: " << queue.GetCount() << endl;
        cout << "  Snapshot count: " << snapshot.GetCount() << endl;
        
        assert(queue.GetCount() == 0);
        assert(snapshot.GetCount() == 0);
        
        cout << "  ✓ Empty snapshot correct" << endl;
    }
    
    cout << "  ✓ Snapshot destroyed without crash" << endl;
}

void TestWriteOnlySnapshot()
{
    cout << "\n=== Test 2: Write-Only Queue Snapshot ===" << endl;
    
    Queue<int> queue;
    
    // 只向写段添加数据
    for (int i = 1; i <= 5; i++) {
        queue.Push(i * 10);
    }
    
    cout << "  Queue count: " << queue.GetCount() << endl;
    
    {
        ValueBuffer<int> snapshot = queue.GetUnreadSnapshot();
        
        cout << "  Snapshot count: " << snapshot.GetCount() << endl;
        assert(snapshot.GetCount() == 5);
        
        cout << "  Snapshot data: ";
        for (int i = 0; i < snapshot.GetCount(); i++) {
            int value;
            snapshot.ReadAt(value, i);
            cout << value << " ";
            assert(value == (i + 1) * 10);
        }
        cout << endl;
        
        cout << "  ✓ Write-only snapshot correct" << endl;
    }
    
    // 验证原队列未受影响
    assert(queue.GetCount() == 5);
    cout << "  ✓ Original queue unchanged" << endl;
    cout << "  ✓ Snapshot destroyed without crash" << endl;
}

void TestReadWriteSnapshot()
{
    cout << "\n=== Test 3: Read + Write Queue Snapshot ===" << endl;
    
    Queue<int> queue;
    
    // 添加初始数据到写段
    cout << "  Phase 1: Push 1-5 to write segment" << endl;
    for (int i = 1; i <= 5; i++) {
        queue.Push(i * 10);
    }
    
    // Pop 2个数据，触发段切换
    cout << "  Phase 2: Pop 2 items (trigger swap)" << endl;
    int popped;
    queue.Pop(popped);
    cout << "    Popped: " << popped << endl;
    assert(popped == 10);
    
    queue.Pop(popped);
    cout << "    Popped: " << popped << endl;
    assert(popped == 20);
    
    // 再添加3个数据到新的写段
    cout << "  Phase 3: Push 6-8 to new write segment" << endl;
    for (int i = 6; i <= 8; i++) {
        queue.Push(i * 10);
    }
    
    cout << "  Queue count: " << queue.GetCount() << " (expected: 6)" << endl;
    assert(queue.GetCount() == 6);
    
    // 获取 snapshot
    {
        ValueBuffer<int> snapshot = queue.GetUnreadSnapshot();
        
        cout << "  Snapshot count: " << snapshot.GetCount() << endl;
        assert(snapshot.GetCount() == 6);
        
        cout << "  Snapshot data: ";
        int expected[] = {30, 40, 50, 60, 70, 80};
        for (int i = 0; i < snapshot.GetCount(); i++) {
            int value;
            snapshot.ReadAt(value, i);
            cout << value << " ";
            assert(value == expected[i]);
        }
        cout << endl;
        
        cout << "  ✓ Read+Write snapshot correct" << endl;
    }
    
    cout << "  ✓ Snapshot destroyed without crash" << endl;
}

void TestSnapshotIndependence()
{
    cout << "\n=== Test 4: Snapshot Independence ===" << endl;
    
    Queue<int> queue;
    
    for (int i = 1; i <= 5; i++) {
        queue.Push(i * 10);
    }
    
    ValueBuffer<int> snapshot = queue.GetUnreadSnapshot();
    
    cout << "  Original snapshot count: " << snapshot.GetCount() << endl;
    assert(snapshot.GetCount() == 5);
    
    // 修改 snapshot 数据
    cout << "  Modifying snapshot data..." << endl;
    for (int i = 0; i < snapshot.GetCount(); i++) {
        snapshot.WriteAt(999, i);
    }
    
    // 验证原队列未受影响
    cout << "  Verifying original queue unchanged..." << endl;
    int index = 0;
    for (auto it = queue.begin(); it != queue.end(); ++it) {
        int value = *it;
        cout << "    Queue[" << index << "] = " << value << " (expected: " << (index + 1) * 10 << ")" << endl;
        assert(value == (index + 1) * 10);
        index++;
    }
    
    cout << "  ✓ Snapshot is independent copy" << endl;
}

void TestMultipleSnapshots()
{
    cout << "\n=== Test 5: Multiple Snapshots Lifecycle ===" << endl;
    
    Queue<int> queue;
    
    for (int i = 1; i <= 3; i++) {
        queue.Push(i * 100);
    }
    
    cout << "  Creating 3 snapshots in nested scopes..." << endl;
    
    {
        ValueBuffer<int> snap1 = queue.GetUnreadSnapshot();
        cout << "    Snapshot 1 created (count: " << snap1.GetCount() << ")" << endl;
        assert(snap1.GetCount() == 3);
        
        {
            ValueBuffer<int> snap2 = queue.GetUnreadSnapshot();
            cout << "    Snapshot 2 created (count: " << snap2.GetCount() << ")" << endl;
            assert(snap2.GetCount() == 3);
            
            {
                ValueBuffer<int> snap3 = queue.GetUnreadSnapshot();
                cout << "    Snapshot 3 created (count: " << snap3.GetCount() << ")" << endl;
                assert(snap3.GetCount() == 3);
                
                cout << "    Destroying snapshot 3..." << endl;
            }
            
            cout << "    Destroying snapshot 2..." << endl;
        }
        
        cout << "    Destroying snapshot 1..." << endl;
    }
    
    cout << "  ✓ All snapshots destroyed without crash" << endl;
    cout << "  ✓ No double-free detected" << endl;
}

void TestComplexScenario()
{
    cout << "\n=== Test 6: Complex Push-Pop-Snapshot Scenario ===" << endl;
    
    Queue<int> queue;
    
    // Step 1: 添加10个数据
    cout << "  Step 1: Push 10 items" << endl;
    for (int i = 1; i <= 10; i++) {
        queue.Push(i);
    }
    
    // Step 2: Pop 5个
    cout << "  Step 2: Pop 5 items" << endl;
    int popped;
    for (int i = 0; i < 5; i++) {
        queue.Pop(popped);
    }
    assert(queue.GetCount() == 5);
    
    // Step 3: 获取 snapshot 1
    cout << "  Step 3: Take snapshot 1" << endl;
    ValueBuffer<int> snap1 = queue.GetUnreadSnapshot();
    cout << "    Snapshot 1 count: " << snap1.GetCount() << endl;
    assert(snap1.GetCount() == 5);
    
    // Step 4: 再添加3个数据
    cout << "  Step 4: Push 3 more items" << endl;
    for (int i = 11; i <= 13; i++) {
        queue.Push(i);
    }
    assert(queue.GetCount() == 8);
    
    // Step 5: 获取 snapshot 2
    cout << "  Step 5: Take snapshot 2" << endl;
    ValueBuffer<int> snap2 = queue.GetUnreadSnapshot();
    cout << "    Snapshot 2 count: " << snap2.GetCount() << endl;
    assert(snap2.GetCount() == 8);
    
    // Step 6: 验证两个 snapshot 的内容
    cout << "  Step 6: Verify snapshots" << endl;
    
    cout << "    Snapshot 1: ";
    for (int i = 0; i < snap1.GetCount(); i++) {
        int value;
        snap1.ReadAt(value, i);
        cout << value << " ";
        assert(value == i + 6);
    }
    cout << endl;
    
    cout << "    Snapshot 2: ";
    for (int i = 0; i < 5; i++) {
        int value;
        snap2.ReadAt(value, i);
        cout << value << " ";
        assert(value == i + 6);
    }
    for (int i = 5; i < 8; i++) {
        int value;
        snap2.ReadAt(value, i);
        cout << value << " ";
        assert(value == i + 6);
    }
    cout << endl;
    
    cout << "  ✓ Complex scenario passed" << endl;
}

int main()
{
    cout << "========================================" << endl;
    cout << "Queue GetUnreadSnapshot() Test Suite" << endl;
    cout << "========================================" << endl;
    
    try {
        TestEmptySnapshot();
        TestWriteOnlySnapshot();
        TestReadWriteSnapshot();
        TestSnapshotIndependence();
        TestMultipleSnapshots();
        TestComplexScenario();
        
        cout << "\n========================================" << endl;
        cout << "All tests passed! ✓" << endl;
        cout << "========================================" << endl;
        return 0;
    }
    catch (const exception& e) {
        cerr << "\n❌ Test failed with exception: " << e.what() << endl;
        return 1;
    }
    catch (...) {
        cerr << "\n❌ Test failed with unknown exception" << endl;
        return 1;
    }
}
