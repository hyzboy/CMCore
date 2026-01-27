#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>
#include"TrackedObject.h"

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 4: Stack with Pointer Management" << endl;
    cout << "========================================" << endl;

    TrackedObject::ResetCounters();

    cout << "\n[4.1] Stack<TrackedObject*> operations (caller manages lifetime):" << endl;
    {
        Stack<TrackedObject*> stack1;
        stack1.Push(new TrackedObject(100));
        stack1.Push(new TrackedObject(200));

        cout << "\n  Before operations:" << endl;
        TrackedObject::PrintCounters();
        assert(stack1.GetCount() == 2);

        cout << "\n  Stack contains 2 elements:" << endl;
        TrackedObject::PrintCounters();
        cout << "  ✓ Stack successfully holds " << stack1.GetCount() << " pointers" << endl;

        // Manual cleanup - caller manages lifetime
        TrackedObject* obj = nullptr;
        while(stack1.Pop(obj)) {
            delete obj;
        }
    }

    cout << "\n[4.2] After manual cleanup:" << endl;
    TrackedObject::PrintCounters();
    assert(TrackedObject::VerifyBalance());

    cout << "\n✅ TEST 4 PASSED" << endl;
    return 0;
}
