#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>
#include"TrackedObject.h"

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 14: Stack Pointer Management Detailed Tests" << endl;
    cout << "========================================" << endl;

    TrackedObject::ResetCounters();
    
    cout << "\n[14.1] Clear() without Free() - manual cleanup:" << endl;
    {
        Stack<TrackedObject*> obj_stack;
        
        for (int i = 0; i < 3; i++)
            obj_stack.Push(new TrackedObject(i * 100));
        
        int alloc_before = obj_stack.GetAllocCount();
        
        // 手动删除所有对象
        obj_stack.ForEachFromTop([](TrackedObject* obj) { delete obj; });
        
        obj_stack.Clear();
        assert(obj_stack.GetCount() == 0);
        assert(obj_stack.GetAllocCount() == alloc_before);  // 容量保留
        
        cout << "  ✓ Clear() cleared stack and capacity retained" << endl;
        TrackedObject::PrintCounters();
    }
    
    cout << "\n[14.2] Verify proper cleanup:" << endl;
    TrackedObject::PrintCounters();
    assert(TrackedObject::VerifyBalance());
    cout << "  ✓ All objects properly deleted" << endl;

    cout << "\n✅ TEST 14 PASSED" << endl;
    return 0;
}
