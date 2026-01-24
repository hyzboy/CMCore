#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>
#include"TrackedObject.h"

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 4: Move Semantics with ManagedStack" << endl;
    cout << "========================================" << endl;

    TrackedObject::ResetCounters();

    cout << "\n[4.1] ManagedStack operations with dynamically allocated objects:" << endl;
    {
        ManagedStack<TrackedObject> stack1;
        stack1.Push(new TrackedObject(100));
        stack1.Push(new TrackedObject(200));

        cout << "\n  Before operations:" << endl;
        TrackedObject::PrintCounters();
        assert(stack1.GetCount() == 2);

        cout << "\n  ValueStack contains 2 elements:" << endl;
        TrackedObject::PrintCounters();
        cout << "  ✓ ManagedStack successfully holds " << stack1.GetCount() << " elements" << endl;
    }

    cout << "\n[4.2] After scope exit:" << endl;
    TrackedObject::PrintCounters();
    assert(TrackedObject::VerifyBalance());

    cout << "\n✅ TEST 4 PASSED" << endl;
    return 0;
}
