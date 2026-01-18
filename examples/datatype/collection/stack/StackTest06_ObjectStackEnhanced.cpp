#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>
#include"TrackedObject.h"

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 6: ObjectStack (Enhanced)" << endl;
    cout << "========================================" << endl;

    TrackedObject::ResetCounters();

    cout << "\n[6.1] Push objects:" << endl;
    {
        ObjectStack<TrackedObject> obj_stack;

        for (int i = 0; i < 5; i++)
        {
            TrackedObject* obj = new TrackedObject(i * 100);
            obj_stack.Push(obj);
        }

        cout << "\n[6.2] Pop some objects manually:" << endl;
        for (int i = 0; i < 2; i++)
        {
            TrackedObject* obj = obj_stack.Pop();
            assert(obj != nullptr);
            cout << "  Popped value: " << obj->GetValue() << endl;
            delete obj;
        }

        cout << "\n[6.3] Remaining objects (auto-cleanup on destruction):" << endl;
        cout << "  Stack count: " << obj_stack.GetCount() << endl;

        // ObjectStack析构时会自动删除剩余的3个对象
    }

    cout << "\n[6.4] After ObjectStack destruction:" << endl;
    TrackedObject::PrintCounters();
    assert(TrackedObject::VerifyBalance());

    cout << "\n✅ TEST 6 PASSED" << endl;
    return 0;
}
