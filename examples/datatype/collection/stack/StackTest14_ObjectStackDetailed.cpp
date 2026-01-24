#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>
#include"TrackedObject.h"

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 14: ManagedStack Detailed Tests" << endl;
    cout << "========================================" << endl;

    TrackedObject::ResetCounters();
    
    cout << "\n[14.1] Clear() without Free():" << endl;
    {
        ManagedStack<TrackedObject> obj_stack;
        
        for (int i = 0; i < 3; i++)
            obj_stack.Push(new TrackedObject(i * 100));
        
        int alloc_before = obj_stack.GetAllocCount();
        
        obj_stack.Clear();
        assert(obj_stack.GetCount() == 0);
        assert(obj_stack.GetAllocCount() == alloc_before);  // 容量保留
        
        cout << "  ✓ Clear() deleted objects but retained capacity" << endl;
        TrackedObject::PrintCounters();
    }
    
    cout << "\n[14.2] Verify no double-deletion:" << endl;
    TrackedObject::PrintCounters();
    assert(TrackedObject::VerifyBalance());
    cout << "  ✓ No double-deletion detected (fix verified!)" << endl;

    cout << "\n✅ TEST 14 PASSED" << endl;
    return 0;
}
