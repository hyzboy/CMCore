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
    cout<<"\n=== Test 2: Auto Increment ID ==="<<endl;
    
    Uint32IdManager<TestObj> mgr;
    
    TestObj *obj1 = new TestObj(1);
    TestObj *obj2 = new TestObj(2);
    TestObj *obj3 = new TestObj(3);
    
    uint32 id1 = mgr.Add(obj1);
    uint32 id2 = mgr.Add(obj2);
    uint32 id3 = mgr.Add(obj3);
    
    cout<<"ID sequence: "<<id1<<", "<<id2<<", "<<id3<<endl;
    assert(id1 == 0);
    assert(id2 == 1);
    assert(id3 == 2);
    
    assert(mgr.GetCount() == 3);
    cout<<"All IDs correctly auto-incremented"<<endl;
    
    mgr.Clear();
    cout<<"Test 2 PASSED"<<endl;
}

void TestDuplicateAdd()
{
    cout<<"\n=== Test 3: Duplicate Add (Same Pointer) ==="<<endl;
    
    Uint32IdManager<TestObj> mgr;
    TestObj *obj = new TestObj(42);
    
    uint32 id1 = mgr.Add(obj);
    cout<<"First add returned ID: "<<id1<<endl;
    
    uint32 id2 = mgr.Add(obj);
    cout<<"Second add (same pointer) returned ID: "<<id2<<endl;
    
    assert(id1 == id2);
    cout<<"Duplicate add returned same ID"<<endl;
    
    // Count should still be 1 (not duplicated)
    assert(mgr.GetCount() == 1);
    cout<<"Count is still 1 (no duplicate entry)"<<endl;
    
    mgr.Clear();
    cout<<"Test 3 PASSED"<<endl;
}

void TestReferenceCount()
{
    cout<<"\n=== Test 4: Reference Counting ==="<<endl;
    
    Uint32IdManager<TestObj> mgr;
    TestObj *obj = new TestObj(99);
    
    uint32 id = mgr.Add(obj);
    cout<<"Added object, initial ref count should be 1"<<endl;
    
    // Get increases ref count
    TestObj *g1 = mgr.Get(id);
    assert(g1 == obj);
    cout<<"Get() returned correct pointer, ref count now 2"<<endl;
    
    // Add same pointer again increases ref count
    uint32 id2 = mgr.Add(obj);
    assert(id2 == id);
    cout<<"Add same pointer again, ref count now 3"<<endl;
    
    // Release without zero_clear
    int rc = mgr.Release(id, false);
    cout<<"Release returned ref count: "<<rc<<" (expected 2)"<<endl;
    assert(rc == 2);
    
    rc = mgr.Release(id, false);
    cout<<"Release returned ref count: "<<rc<<" (expected 1)"<<endl;
    assert(rc == 1);
    
    rc = mgr.Release(id, false);
    cout<<"Release returned ref count: "<<rc<<" (expected 0)"<<endl;
    assert(rc == 0);
    
    // Object should still exist (zero_clear was false)
    assert(mgr.GetCount() == 1);
    cout<<"Object still in manager (zero_clear=false)"<<endl;
    
    mgr.Clear();
    cout<<"Test 4 PASSED"<<endl;
}

void TestReleaseWithZeroClear()
{
    cout<<"\n=== Test 5: Release with zero_clear ==="<<endl;
    
    Uint32IdManager<TestObj> mgr;
    TestObj *obj = new TestObj(123);
    
    uint32 id = mgr.Add(obj);
    
    // Release with zero_clear=true when ref_count=1
    int rc = mgr.Release(id, true);
    cout<<"Release with zero_clear returned: "<<rc<<" (expected 0)"<<endl;
    assert(rc == 0);
    
    // Object should be removed
    assert(mgr.GetCount() == 0);
    cout<<"Object removed from manager"<<endl;
    
    // Find should return nullptr
    TestObj *found = mgr.Find(id);
    assert(found == nullptr);
    cout<<"Find returns nullptr after zero_clear"<<endl;
    
    cout<<"Test 5 PASSED"<<endl;
}

void TestReleaseByPointer()
{
    cout<<"\n=== Test 6: Release by Pointer ==="<<endl;
    
    Uint32IdManager<TestObj> mgr;
    TestObj *obj = new TestObj(456);
    
    uint32 id = mgr.Add(obj);
    mgr.Get(id); // Increase ref count to 2
    
    // Release by pointer
    int rc = mgr.Release(obj, false);
    cout<<"Release by pointer returned: "<<rc<<" (expected 1)"<<endl;
    assert(rc == 1);
    
    // Release again by pointer with zero_clear
    rc = mgr.Release(obj, true);
    cout<<"Release by pointer with zero_clear returned: "<<rc<<" (expected 0)"<<endl;
    assert(rc == 0);
    
    assert(mgr.GetCount() == 0);
    cout<<"Object removed"<<endl;
    
    cout<<"Test 6 PASSED"<<endl;
}

void TestGetKeyByValue()
{
    cout<<"\n=== Test 7: GetKeyByValue ==="<<endl;
    
    Uint32IdManager<TestObj> mgr;
    TestObj *obj1 = new TestObj(111);
    TestObj *obj2 = new TestObj(222);
    
    uint32 id1 = mgr.Add(obj1);
    uint32 id2 = mgr.Add(obj2);
    
    uint32 found_key;
    uint found_count;
    
    // GetKeyByValue without increasing ref count
    bool result = mgr.GetKeyByValue(obj1, &found_key, &found_count, false);
    assert(result == true);
    assert(found_key == id1);
    assert(found_count == 1);
    cout<<"GetKeyByValue found correct key: "<<found_key<<", ref count: "<<found_count<<endl;
    
    // GetKeyByValue with increasing ref count
    result = mgr.GetKeyByValue(obj2, &found_key, &found_count, true);
    assert(result == true);
    assert(found_key == id2);
    assert(found_count == 1); // Returns count before increment
    cout<<"GetKeyByValue with add_ref=true"<<endl;
    
    // Verify ref count was increased
    int rc = mgr.Release(id2, false);
    assert(rc == 1); // Should be 1 after one release (was 2)
    cout<<"Ref count was correctly increased"<<endl;
    
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
    cout<<"\n=== Test 9: Find vs Get ==="<<endl;
    
    Uint32IdManager<TestObj> mgr;
    TestObj *obj = new TestObj(789);
    
    uint32 id = mgr.Add(obj);
    
    // Find does NOT increase ref count
    TestObj *f = mgr.Find(id);
    assert(f == obj);
    
    // Get DOES increase ref count
    TestObj *g = mgr.Get(id);
    assert(g == obj);
    
    // Release once should leave ref count at 1
    int rc = mgr.Release(id, false);
    assert(rc == 1);
    cout<<"Find doesn't increase ref count, Get does"<<endl;
    
    mgr.Clear();
    cout<<"Test 9 PASSED"<<endl;
}

void TestClearFree()
{
    cout<<"\n=== Test 10: ClearFree ==="<<endl;
    
    Uint32IdManager<TestObj> mgr;
    
    TestObj *obj1 = new TestObj(1);
    TestObj *obj2 = new TestObj(2);
    TestObj *obj3 = new TestObj(3);
    
    uint32 id1 = mgr.Add(obj1);
    uint32 id2 = mgr.Add(obj2);
    uint32 id3 = mgr.Add(obj3);
    
    // Decrease ref count of obj1 and obj2 to 0
    mgr.Release(id1, false);
    mgr.Release(id2, false);
    
    cout<<"Before ClearFree, count: "<<mgr.GetCount()<<endl;
    assert(mgr.GetCount() == 3);
    
    // ClearFree should remove objects with ref_count <= 0
    mgr.ClearFree();
    
    cout<<"After ClearFree, count: "<<mgr.GetCount()<<endl;
    assert(mgr.GetCount() == 1);
    
    // obj3 should still be findable
    TestObj *found = mgr.Find(id3);
    assert(found == obj3);
    cout<<"ClearFree removed zero-ref objects, kept others"<<endl;
    
    mgr.Clear();
    cout<<"Test 10 PASSED"<<endl;
}

void TestMultipleObjects()
{
    cout<<"\n=== Test 11: Multiple Objects Management ==="<<endl;
    
    Uint32IdManager<TestObj> mgr;
    
    const int COUNT = 10;
    TestObj *objects[COUNT];
    uint32 ids[COUNT];
    
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
    cout<<"\n=== Test 13: Clear with References ==="<<endl;
    
    Uint32IdManager<TestObj> mgr;
    
    TestObj *obj = new TestObj(999);
    uint32 id = mgr.Add(obj);
    
    // Increase ref count
    mgr.Get(id);
    mgr.Get(id);
    
    cout<<"Object has ref count 3"<<endl;
    
    // Clear should delete all objects regardless of ref count
    mgr.Clear();
    
    assert(mgr.GetCount() == 0);
    cout<<"Clear removed all objects"<<endl;
    
    cout<<"Test 13 PASSED"<<endl;
}

void TestInstanceLifecycle()
{
    cout<<"\n=== Test 14: Instance Lifecycle ==="<<endl;
    
    TestObj::ResetInstanceCount();
    int initial_count = TestObj::GetInstanceCount();
    cout<<"Initial instance count: "<<initial_count<<endl;
    
    {
        Uint32IdManager<TestObj> mgr;
        
        mgr.Add(new TestObj(1));
        mgr.Add(new TestObj(2));
        mgr.Add(new TestObj(3));
        
        cout<<"After adding 3 objects, instance count: "<<TestObj::GetInstanceCount()<<endl;
        assert(TestObj::GetInstanceCount() == initial_count + 3);
        
        // mgr goes out of scope, should delete all objects
    }
    
    cout<<"After mgr destroyed, instance count: "<<TestObj::GetInstanceCount()<<endl;
    assert(TestObj::GetInstanceCount() == initial_count);
    cout<<"All instances properly destroyed"<<endl;
    
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

