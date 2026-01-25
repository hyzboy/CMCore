#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>
#include"TrackedObject.h"

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 6: Stack with Pointers (Manual Management)" << endl;
    cout << "========================================" << endl;

    TrackedObject::ResetCounters();

    cout << "\n[6.1] Push objects:" << endl;
    {
        Stack<TrackedObject*> obj_stack;

        for (int i = 0; i < 5; i++)
        {
            TrackedObject* obj = new TrackedObject(i * 100);
            obj_stack.Push(obj);
        }

        cout << "\n[6.2] Pop some objects manually:" << endl;
        for (int i = 0; i < 2; i++)
        {
            TrackedObject* obj;
            assert(obj_stack.Pop(obj));
            assert(obj != nullptr);
            cout << "  Popped value: " << obj->GetValue() << endl;
            delete obj;
        }

        cout << "\n[6.3] Remaining objects (manual cleanup):" << endl;
        cout << "  Stack count: " << obj_stack.GetCount() << endl;

        // 手动删除剩余对象
        while (obj_stack.GetCount() > 0)
        {
            TrackedObject* obj;
            obj_stack.Pop(obj);
            delete obj;
        }
    }

    cout << "\n[6.4] After manual cleanup:" << endl;
    TrackedObject::PrintCounters();
    assert(TrackedObject::VerifyBalance());

    cout << "\n✅ TEST 6 PASSED" << endl;
    return 0;
}
