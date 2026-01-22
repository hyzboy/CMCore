#include<hgl/type/ObjectManager.h>
#include<iostream>

using namespace hgl;
using namespace std;

//#define SCL std::source_location::current()

class TestObj
{
    int v;

public:

    TestObj(int _v):v(_v)
    {
        std::cout<<"TestObj::TestObj("<<v<<")"<<std::endl;
    }

    ~TestObj()
    {
        std::cout<<"TestObj::~TestObj("<<v<<")"<<std::endl;
    }
};

int main()
{
    cout<<"U32ObjectManage Test"<<endl;

    Uint32IdManager<TestObj> mgr;

    // create object
    TestObj *p=new TestObj(42);

    // add once
    uint32 id1=mgr.Add(p);
    cout<<"Add returned id: "<<id1<<endl;

    // add same pointer again should return same id (and increase ref count)
    uint32 id2=mgr.Add(p);
    cout<<"Add same object returned id: "<<id2<<endl;

    if(id1!=id2)
        cout<<"ERROR: duplicate Add returned different id"<<endl;

    // count should be 1
    cout<<"Count: "<<mgr.GetCount()<<endl;

    // Get should increase reference count
    TestObj *g=mgr.Get(id1);
    if(g!=p) cout<<"ERROR: Get returned different pointer"<<endl;

    // Release by id (not clearing on zero)
    int rc=mgr.Release(id1,false);
    cout<<"Release by id returned: "<<rc<<" (expected >0)"<<endl;

    // Release by value
    rc=mgr.Release(p,false);
    cout<<"Release by value returned: "<<rc<<" (expected >=0)"<<endl;

    // Final release with zero_clear=true should remove the item
    rc=mgr.Release(id1,true);
    cout<<"Final release (zero_clear) returned: "<<rc<<" (expected 0)"<<endl;

    cout<<"Count after final release: "<<mgr.GetCount()<<endl;

    if(mgr.Find(id1)!=nullptr)
        cout<<"ERROR: Find still returns object after zero clear"<<endl;

    // Add nullptr should fail and return (uint32)-1
    TestObj *nullp=nullptr;
    uint32 null_id=mgr.Add(nullp);
    cout<<"Add nullptr returned: "<<null_id<<" (expected "<<(uint32)-1<<")"<<endl;

    // Clean up any remaining data
    mgr.Clear();

    delete p; // in case lifecycle manager didn't delete it

    cout<<"U32ObjectManage Test finished"<<endl;
    return 0;
}

