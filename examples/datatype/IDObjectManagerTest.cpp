#include<hgl/type/ObjectManager.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

class TestObj
{
    int v;
    static int instance_count;

public:

    TestObj(int _v):v(_v)
    {
        ++instance_count;
        std::cout<<"TestObj::TestObj("<<v<<") - Total instances: "<<instance_count<<std::endl;
    }

    ~TestObj()
    {
        --instance_count;
        std::cout<<"TestObj::~TestObj("<<v<<") - Total instances: "<<instance_count<<std::endl;
    }

    int GetValue() const { return v; }

    static int GetInstanceCount() { return instance_count; }
    static void ResetInstanceCount() { instance_count = 0; }
};

int TestObj::instance_count = 0;

void TestBasicAddAndFind()
{
    cout<<"\n=== Test 1: Basic Add and Find ==="<<endl;

    Uint32IdManager<TestObj> mgr;
    TestObj *obj1 = new TestObj(100);

    uint32 id = mgr.Add(obj1);
    cout<<"Added object with ID: "<<id<<" (expected 0)"<<endl;
    assert(id == 0);

    TestObj *found = mgr.Find(id);
    assert(found == obj1);
    cout<<"Find returned correct object"<<endl;

    assert(mgr.GetCount() == 1);
    cout<<"Count is correct: "<<mgr.GetCount()<<endl;

    mgr.Clear();
    cout<<"Test 1 PASSED"<<endl;
}

void TestAutoIncrementID()
{
    cout<<"\n=== Test 2: Auto Increment ID (300 objects) ==="<<endl;

    Uint32IdManager<TestObj> mgr;
    const int COUNT = 300;
    uint32 *ids = new uint32[COUNT];

    for(int i = 0; i < COUNT; i++)
    {
        ids[i] = mgr.Add(new TestObj(i));
    }

    cout<<"ID sequence verified for "<<COUNT<<" objects"<<endl;
    for(int i = 0; i < COUNT; i++)
    {
        assert(ids[i] == (uint32)i);
    }

    assert(mgr.GetCount() == COUNT);
    cout<<"All "<<COUNT<<" IDs correctly auto-incremented"<<endl;

    delete[] ids;
    mgr.Clear();
    cout<<"Test 2 PASSED"<<endl;
}

void TestDuplicateAdd()
{
    cout<<"\n=== Test 3: Duplicate Add (100 objects added twice) ==="<<endl;

    Uint32IdManager<TestObj> mgr;
    const int COUNT = 100;

    uint32 *ids = new uint32[COUNT];
    TestObj **objects = new TestObj*[COUNT];

    for(int i = 0; i < COUNT; i++)
    {
        objects[i] = new TestObj(i * 42);
        ids[i] = mgr.Add(objects[i]);
    }

    // Add same objects again
    for(int i = 0; i < COUNT; i++)
    {
        uint32 id2 = mgr.Add(objects[i]);
        assert(id2 == ids[i]);
    }

    cout<<"Added "<<COUNT<<" objects twice, got same IDs"<<endl;

    // Count should still be COUNT (not duplicated)
    assert(mgr.GetCount() == COUNT);
    cout<<"Count is still "<<COUNT<<" (no duplicate entries)"<<endl;

    delete[] ids;
    delete[] objects;
    mgr.Clear();
    cout<<"Test 3 PASSED"<<endl;
}

void TestReferenceCount()
{
    cout<<"\n=== Test 4: Reference Counting (100 objects) ==="<<endl;

    Uint32IdManager<TestObj> mgr;
    const int COUNT = 100;
    uint32 *ids = new uint32[COUNT];

    for(int i = 0; i < COUNT; i++)
    {
        ids[i] = mgr.Add(new TestObj(99 + i));
    }

    cout<<"Testing reference counting on "<<COUNT<<" objects"<<endl;

    // Get increases ref count for all
    for(int i = 0; i < COUNT; i++)
    {
        mgr.Get(ids[i]);
    }

    // Release all
    for(int i = 0; i < COUNT; i++)
    {
        int rc = mgr.Release(ids[i], false);
        assert(rc == 1);
    }

    // Objects should still exist (zero_clear was false)
    assert(mgr.GetCount() == COUNT);
    cout<<"Reference counting verified on "<<COUNT<<" objects"<<endl;

    delete[] ids;
    mgr.Clear();
    cout<<"Test 4 PASSED"<<endl;
}

void TestReleaseWithZeroClear()
{
    cout<<"\n=== Test 5: Release with zero_clear (100 objects) ==="<<endl;

    Uint32IdManager<TestObj> mgr;
    const int COUNT = 100;
    uint32 *ids = new uint32[COUNT];

    for(int i = 0; i < COUNT; i++)
    {
        ids[i] = mgr.Add(new TestObj(123 + i));
    }

    // Release all with zero_clear=true
    for(int i = 0; i < COUNT; i++)
    {
        int rc = mgr.Release(ids[i], true);
        assert(rc == 0);
    }

    // Objects should be removed
    assert(mgr.GetCount() == 0);
    cout<<"All "<<COUNT<<" objects removed from manager"<<endl;

    // Find should return nullptr
    for(int i = 0; i < COUNT; i++)
    {
        TestObj *found = mgr.Find(ids[i]);
        assert(found == nullptr);
    }
    cout<<"Find returns nullptr for all after zero_clear"<<endl;

    delete[] ids;
    cout<<"Test 5 PASSED"<<endl;
}

void TestReleaseByPointer()
{
    cout<<"\n=== Test 6: Release by Pointer (100 objects) ==="<<endl;

    Uint32IdManager<TestObj> mgr;
    const int COUNT = 100;
    TestObj **objects = new TestObj*[COUNT];
    uint32 *ids = new uint32[COUNT];

    for(int i = 0; i < COUNT; i++)
    {
        objects[i] = new TestObj(456 + i);
        ids[i] = mgr.Add(objects[i]);
        mgr.Get(ids[i]); // Increase ref count to 2
    }

    // Release all by pointer
    for(int i = 0; i < COUNT; i++)
    {
        int rc = mgr.Release(objects[i], false);
        assert(rc == 1);
    }

    // Release again by pointer with zero_clear
    for(int i = 0; i < COUNT; i++)
    {
        int rc = mgr.Release(objects[i], true);
        assert(rc == 0);
    }

    assert(mgr.GetCount() == 0);
    cout<<"All "<<COUNT<<" objects removed"<<endl;

    delete[] objects;
    delete[] ids;
    cout<<"Test 6 PASSED"<<endl;
}

void TestGetKeyByValue()
{
    cout<<"\n=== Test 7: GetKeyByValue (200 objects) ==="<<endl;

    Uint32IdManager<TestObj> mgr;
    const int COUNT = 200;
    TestObj **objects = new TestObj*[COUNT];
    uint32 *ids = new uint32[COUNT];

    for(int i = 0; i < COUNT; i++)
    {
        objects[i] = new TestObj(111 + i);
        ids[i] = mgr.Add(objects[i]);
    }

    uint32 found_key;
    uint found_count;

    // GetKeyByValue without increasing ref count
    for(int i = 0; i < COUNT; i++)
    {
        bool result = mgr.GetKeyByValue(objects[i], &found_key, &found_count, false);
        assert(result == true);
        assert(found_key == ids[i]);
        assert(found_count == 1);
    }
    cout<<"GetKeyByValue found all "<<COUNT<<" objects with correct keys"<<endl;

    // GetKeyByValue with increasing ref count on half
    for(int i = 0; i < COUNT / 2; i++)
    {
        bool result = mgr.GetKeyByValue(objects[i], &found_key, &found_count, true);
        assert(result == true);
        assert(found_key == ids[i]);
    }
    cout<<"GetKeyByValue with add_ref=true verified"<<endl;

    // Verify ref count was increased for first half
    for(int i = 0; i < COUNT / 2; i++)
    {
        int rc = mgr.Release(ids[i], false);
        assert(rc == 1);
    }
    cout<<"Ref count was correctly increased"<<endl;

    delete[] objects;
    delete[] ids;
    mgr.Clear();
    cout<<"Test 7 PASSED"<<endl;
}

void TestNullHandling()
{
    cout<<"\n=== Test 8: Null Handling ==="<<endl;

    Uint32IdManager<TestObj> mgr;

    TestObj *nullp = nullptr;
    uint32 null_id = mgr.Add(nullp);

    cout<<"Add nullptr returned: "<<null_id<<" (expected "<<(uint32)-1<<")"<<endl;
    assert(null_id == (uint32)-1);

    assert(mgr.GetCount() == 0);
    cout<<"Count is 0 after adding nullptr"<<endl;

    cout<<"Test 8 PASSED"<<endl;
}

void TestFindVsGet()
{
    cout<<"\n=== Test 9: Find vs Get (100 objects) ==="<<endl;

    Uint32IdManager<TestObj> mgr;
    const int COUNT = 100;
    uint32 *ids = new uint32[COUNT];
    TestObj **objects = new TestObj*[COUNT];

    for(int i = 0; i < COUNT; i++)
    {
        objects[i] = new TestObj(789 + i);
        ids[i] = mgr.Add(objects[i]);
    }

    // Find does NOT increase ref count
    for(int i = 0; i < COUNT; i++)
    {
        TestObj *f = mgr.Find(ids[i]);
        assert(f == objects[i]);
    }

    // Get DOES increase ref count
    for(int i = 0; i < COUNT; i++)
    {
        TestObj *g = mgr.Get(ids[i]);
        assert(g == objects[i]);
    }

    // Release once should leave ref count at 1
    for(int i = 0; i < COUNT; i++)
    {
        int rc = mgr.Release(ids[i], false);
        assert(rc == 1);
    }
    cout<<"Find doesn't increase ref count, Get does - verified on "<<COUNT<<" objects"<<endl;

    delete[] ids;
    delete[] objects;
    mgr.Clear();
    cout<<"Test 9 PASSED"<<endl;
}

void TestClearFree()
{
    cout<<"\n=== Test 10: ClearFree (1000 objects) ==="<<endl;

    Uint32IdManager<TestObj> mgr;

    const int COUNT = 1000;
    uint32 *ids = new uint32[COUNT];

    // Add multiple objects
    for(int i = 0; i < COUNT; i++)
    {
        ids[i] = mgr.Add(new TestObj(i));
    }

    // Decrease ref count of first 500 objects to 0
    for(int i = 0; i < COUNT / 2; i++)
    {
        mgr.Release(ids[i], false);
    }

    cout<<"Before ClearFree, count: "<<mgr.GetCount()<<endl;
    assert(mgr.GetCount() == COUNT);

    // ClearFree should remove objects with ref_count <= 0
    mgr.ClearFree();

    cout<<"After ClearFree, count: "<<mgr.GetCount()<<endl;
    assert(mgr.GetCount() == COUNT / 2);
    cout<<"ClearFree removed zero-ref objects, kept others"<<endl;

    delete[] ids;
    mgr.Clear();
    cout<<"Test 10 PASSED"<<endl;
}

void TestMultipleObjects()
{
    cout<<"\n=== Test 11: Multiple Objects Management (1000 objects) ==="<<endl;

    Uint32IdManager<TestObj> mgr;

    const int COUNT = 1000;
    TestObj **objects = new TestObj*[COUNT];
    uint32 *ids = new uint32[COUNT];

    // Add multiple objects
    for(int i = 0; i < COUNT; ++i)
    {
        objects[i] = new TestObj(i * 100);
        ids[i] = mgr.Add(objects[i]);
        assert(ids[i] == (uint32)i);
    }

    cout<<"Added "<<COUNT<<" objects"<<endl;
    assert(mgr.GetCount() == COUNT);

    // Verify all objects are findable
    for(int i = 0; i < COUNT; ++i)
    {
        TestObj *found = mgr.Find(ids[i]);
        assert(found == objects[i]);
        assert(found->GetValue() == i * 100);
    }
    cout<<"All objects findable with correct values"<<endl;

    // Release half of them with zero_clear
    for(int i = 0; i < COUNT / 2; ++i)
    {
        mgr.Release(ids[i], true);
    }

    assert(mgr.GetCount() == COUNT / 2);
    cout<<"Released half, count now: "<<mgr.GetCount()<<endl;

    delete[] objects;
    delete[] ids;

    mgr.Clear();
    cout<<"Test 11 PASSED"<<endl;
}

void TestInvalidOperations()
{
    cout<<"\n=== Test 12: Invalid Operations ==="<<endl;

    Uint32IdManager<TestObj> mgr;

    // Release non-existent ID
    int rc = mgr.Release(999, false);
    cout<<"Release non-existent ID returned: "<<rc<<" (expected -1)"<<endl;
    assert(rc == -1);

    // Find non-existent ID
    TestObj *found = mgr.Find(999);
    assert(found == nullptr);
    cout<<"Find non-existent ID returned nullptr"<<endl;

    // Get non-existent ID
    TestObj *g = mgr.Get(999);
    assert(g == nullptr);
    cout<<"Get non-existent ID returned nullptr"<<endl;

    // Release non-existent pointer
    TestObj dummy(0);
    rc = mgr.Release(&dummy, false);
    assert(rc == -1);
    cout<<"Release non-existent pointer returned -1"<<endl;

    cout<<"Test 12 PASSED"<<endl;
}

void TestClearWhileHavingReferences()
{
    cout<<"\n=== Test 13: Clear with References (100 objects) ==="<<endl;

    Uint32IdManager<TestObj> mgr;

    const int COUNT = 100;
    uint32 *ids = new uint32[COUNT];

    for(int i = 0; i < COUNT; i++)
    {
        ids[i] = mgr.Add(new TestObj(999 + i));
        // Increase ref count to 3
        mgr.Get(ids[i]);
        mgr.Get(ids[i]);
    }

    cout<<"All "<<COUNT<<" objects have ref count 3"<<endl;

    // Clear should delete all objects regardless of ref count
    mgr.Clear();

    assert(mgr.GetCount() == 0);
    cout<<"Clear removed all "<<COUNT<<" objects"<<endl;

    delete[] ids;
    cout<<"Test 13 PASSED"<<endl;
}

void TestInstanceLifecycle()
{
    cout<<"\n=== Test 14: Instance Lifecycle (300 objects) ==="<<endl;

    TestObj::ResetInstanceCount();
    int initial_count = TestObj::GetInstanceCount();
    cout<<"Initial instance count: "<<initial_count<<endl;

    const int COUNT = 300;

    {
        Uint32IdManager<TestObj> mgr;

        for(int i = 0; i < COUNT; i++)
        {
            mgr.Add(new TestObj(i * 10));
        }

        cout<<"After adding "<<COUNT<<" objects, instance count: "<<TestObj::GetInstanceCount()<<endl;
        assert(TestObj::GetInstanceCount() == initial_count + COUNT);

        // mgr goes out of scope, should delete all objects
    }

    cout<<"After mgr destroyed, instance count: "<<TestObj::GetInstanceCount()<<endl;
    assert(TestObj::GetInstanceCount() == initial_count);
    cout<<"All "<<COUNT<<" instances properly destroyed"<<endl;

    cout<<"Test 14 PASSED"<<endl;
}

int main()
{
    cout<<"=========================================="<<endl;
    cout<<"  AutoIdObjectManager Comprehensive Test"<<endl;
    cout<<"=========================================="<<endl;

    TestBasicAddAndFind();
    TestAutoIncrementID();
    TestDuplicateAdd();
    TestReferenceCount();
    TestReleaseWithZeroClear();
    TestReleaseByPointer();
    TestGetKeyByValue();
    TestNullHandling();
    TestFindVsGet();
    TestClearFree();
    TestMultipleObjects();
    TestInvalidOperations();
    TestClearWhileHavingReferences();
    TestInstanceLifecycle();

    cout<<"\n=========================================="<<endl;
    cout<<"  All Tests PASSED!"<<endl;
    cout<<"=========================================="<<endl;

    return 0;
}

