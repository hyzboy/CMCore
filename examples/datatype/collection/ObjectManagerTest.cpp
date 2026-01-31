#include<hgl/type/ObjectManager.h>
#include<iostream>
#include<string>
#include<cassert>

using namespace hgl;
using namespace std;

// ============================================================================
// 测试用的简单对象类
// ============================================================================

class TestObject
{
public:
    int id;
    string name;
    
    TestObject(int i, const string& n) : id(i), name(n)
    {
        cout << "  [Create] TestObject(" << id << ", \"" << name << "\")" << endl;
    }
    
    ~TestObject()
    {
        cout << "  [Delete] TestObject(" << id << ", \"" << name << "\")" << endl;
    }
};

// ============================================================================
// 测试 1: 基础操作
// ============================================================================

void TestBasicOperations()
{
    cout << "\n" << string(80, '=') << endl;
    cout << "Test 1: Basic Operations" << endl;
    cout << string(80, '=') << endl;

    ManagedObjectRegistry<int, TestObject> manager;

    cout << "\n✓ Testing Add..." << endl;
    {
        auto obj1 = new TestObject(1, "Alice");
        auto obj2 = new TestObject(2, "Bob");
        auto obj3 = new TestObject(3, "Charlie");

        assert(manager.Add(100, obj1) == true);
        assert(manager.Add(200, obj2) == true);
        assert(manager.Add(300, obj3) == true);
        
        // 重复添加应该失败
        auto obj_dup = new TestObject(99, "Duplicate");
        assert(manager.Add(100, obj_dup) == false);
        delete obj_dup; // 手动删除未添加的对象

        cout << "  Added 3 objects. Count: " << manager.GetCount() << endl;
        assert(manager.GetCount() == 3);
    }

    cout << "\n✓ Testing Find..." << endl;
    {
        TestObject* obj = manager.Find(100);
        assert(obj != nullptr);
        assert(obj->id == 1);
        assert(obj->name == "Alice");
        cout << "  Found: " << obj->id << " -> " << obj->name << endl;

        TestObject* not_found = manager.Find(999);
        assert(not_found == nullptr);
        cout << "  Not found (999) correctly returns nullptr" << endl;
    }

    cout << "\n✓ Testing Get (with ref_count increment)..." << endl;
    {
        TestObject* obj = manager.Get(200);
        assert(obj != nullptr);
        assert(obj->id == 2);
        cout << "  Got: " << obj->id << " -> " << obj->name << endl;
        
        // 多次 Get 应该增加引用计数
        TestObject* obj2 = manager.Get(200);
        assert(obj2 == obj);
        cout << "  Called Get twice on same object" << endl;
        
        // 使用新的 GetRefCount 方法
        int ref_count = manager.GetRefCount(200);
        cout << "  Ref count by key: " << ref_count << endl;
        assert(ref_count == 3); // 初始1 + Get两次
    }

    cout << "\n✓ Testing GetKeyByValue..." << endl;
    {
        TestObject* obj = manager.Find(100);
        int found_key = -1;
        uint ref_count = 0;
        
        bool result = manager.GetKeyByValue(obj, &found_key, &ref_count, false);
        assert(result == true);
        assert(found_key == 100);
        cout << "  Found key: " << found_key << ", ref_count: " << ref_count << endl;
    }

    cout << "\n✓ Cleaning up..." << endl;
    manager.Clear();
    assert(manager.GetCount() == 0);
    cout << "  All objects deleted" << endl;
}

// ============================================================================
// 测试 2: 引用计数
// ============================================================================

void TestReferenceCount()
{
    cout << "\n" << string(80, '=') << endl;
    cout << "Test 2: Reference Counting" << endl;
    cout << string(80, '=') << endl;

    ManagedObjectRegistry<int, TestObject> manager;

    cout << "\n✓ Adding object..." << endl;
    auto obj = new TestObject(10, "RefTest");
    manager.Add(1, obj);

    cout << "\n✓ Testing Get (increases ref_count)..." << endl;
    {
        int initial_key;
        uint initial_count;
        manager.GetKeyByValue(obj, &initial_key, &initial_count, false);
        cout << "  Initial ref_count: " << initial_count << endl;

        manager.Get(1);
        
        uint after_get;
        manager.GetKeyByValue(obj, nullptr, &after_get, false);
        cout << "  After Get(): " << after_get << endl;
        assert(after_get == initial_count + 1);
    }

    cout << "\n✓ Testing Release..." << endl;
    {
        uint before_release;
        manager.GetKeyByValue(obj, nullptr, &before_release, false);
        cout << "  Before Release: " << before_release << endl;

        int remaining = manager.Release(1, false);
        cout << "  After Release: " << remaining << endl;
        assert(remaining == before_release - 1);
        
        // 使用新的 GetRefCount 方法验证
        int ref_count = manager.GetRefCount(1);
        cout << "  Ref count by key after Release: " << ref_count << endl;
        assert(ref_count == remaining);
    }

    cout << "\n✓ Testing Release by pointer..." << endl;
    {
        uint before_release;
        manager.GetKeyByValue(obj, nullptr, &before_release, false);
        cout << "  Before Release: " << before_release << endl;

        int remaining = manager.Release(obj, false);
        cout << "  After Release by pointer: " << remaining << endl;
        assert(remaining == before_release - 1);
        
        // 使用新的 GetRefCount 方法按对象指针查询
        int ref_count = manager.GetRefCount(obj);
        cout << "  Ref count by pointer after Release: " << ref_count << endl;
        assert(ref_count == remaining);
    }

    cout << "\n✓ Cleaning up..." << endl;
    manager.Clear();
}

// ============================================================================
// 测试 3: 零引用计数删除
// ============================================================================

void TestZeroClear()
{
    cout << "\n" << string(80, '=') << endl;
    cout << "Test 3: Zero Reference Count Deletion" << endl;
    cout << string(80, '=') << endl;

    ManagedObjectRegistry<int, TestObject> manager;

    cout << "\n✓ Adding 3 objects..." << endl;
    manager.Add(1, new TestObject(1, "Obj1"));
    manager.Add(2, new TestObject(2, "Obj2"));
    manager.Add(3, new TestObject(3, "Obj3"));
    cout << "  Count: " << manager.GetCount() << endl;

    cout << "\n✓ Getting objects (increase ref_count)..." << endl;
    manager.Get(1);
    manager.Get(2);
    // Object 3 的 ref_count 保持为 1

    cout << "\n✓ Releasing with zero_clear=true..." << endl;
    {
        int count = manager.Release(1, true);
        cout << "  Released obj1, remaining ref_count: " << count << endl;
        cout << "  [DEBUG] After first Release(1,true): GetCount() = " << manager.GetCount() << endl;
        assert(count == 1); // 还有一个引用
        assert(manager.GetCount() == 3); // 对象还在

        count = manager.Release(1, true);
        cout << "  Released obj1 again, remaining ref_count: " << count << endl;
        cout << "  [DEBUG] After second Release(1,true): GetCount() = " << manager.GetCount() << endl;
        assert(count == 0); // 引用计数为0，对象被删除
        assert(manager.GetCount() == 2); // 对象数量减少
    }

    cout << "\n✓ Testing ClearFree (removes zero ref_count objects)..." << endl;
    {
        // Object 2 ref_count = 2, Object 3 ref_count = 1
        manager.Release(2, false); // 减到 1
        manager.Release(3, false); // 减到 0

        cout << "  Before ClearFree: " << manager.GetCount() << endl;
        manager.ClearFree();
        cout << "  After ClearFree: " << manager.GetCount() << endl;
        assert(manager.GetCount() == 1); // 只剩 Object 2 (ref_count=1)
    }

    cout << "\n✓ Cleaning up..." << endl;
    manager.Clear();
}

// ============================================================================
// 测试 4: AutoIdObjectManager
// ============================================================================

void TestAutoIdManager()
{
    cout << "\n" << string(80, '=') << endl;
    cout << "Test 4: AutoIdObjectManager" << endl;
    cout << string(80, '=') << endl;

    Uint32IdManager<TestObject> manager;

    cout << "\n✓ Testing auto ID allocation..." << endl;
    {
        auto obj1 = new TestObject(101, "Auto1");
        auto obj2 = new TestObject(102, "Auto2");
        auto obj3 = new TestObject(103, "Auto3");

        uint32 id1 = manager.Add(obj1);
        uint32 id2 = manager.Add(obj2);
        uint32 id3 = manager.Add(obj3);

        cout << "  Allocated IDs: " << id1 << ", " << id2 << ", " << id3 << endl;
        assert(id1 == 0);
        assert(id2 == 1);
        assert(id3 == 2);
        assert(manager.GetCount() == 3);
    }

    cout << "\n✓ Testing duplicate object detection..." << endl;
    {
        TestObject* existing_obj = manager.Find(0);
        uint32 id = manager.Add(existing_obj); // 添加已存在的对象
        
        cout << "  Adding existing object returned ID: " << id << endl;
        assert(id == 0); // 返回原来的 ID
        assert(manager.GetCount() == 3); // 数量不变
        
        // 检查引用计数是否增加
        uint ref_count;
        manager.GetKeyByValue(existing_obj, nullptr, &ref_count, false);
        cout << "  Ref count after re-add: " << ref_count << endl;
        assert(ref_count == 2); // ref_count 应该增加了
    }

    cout << "\n✓ Testing Find by auto-generated ID..." << endl;
    {
        TestObject* obj = manager.Find(1);
        assert(obj != nullptr);
        assert(obj->id == 102);
        assert(obj->name == "Auto2");
        cout << "  Found: ID=" << obj->id << ", Name=" << obj->name << endl;
    }

    cout << "\n✓ Testing null object handling..." << endl;
    {
        uint32 id = manager.Add(nullptr);
        assert(id == uint32(-1));
        cout << "  Adding nullptr correctly returned -1" << endl;
    }

    cout << "\n✓ Cleaning up..." << endl;
    manager.Clear();
}

// ============================================================================
// 测试 5: 边界情况
// ============================================================================

void TestEdgeCases()
{
    cout << "\n" << string(80, '=') << endl;
    cout << "Test 5: Edge Cases" << endl;
    cout << string(80, '=') << endl;

    ManagedObjectRegistry<int, TestObject> manager;

    cout << "\n✓ Testing empty manager..." << endl;
    {
        assert(manager.GetCount() == 0);
        assert(manager.Find(1) == nullptr);
        assert(manager.Get(1) == nullptr);
        
        int result = manager.Release(1, false);
        assert(result == -1);
        cout << "  Empty manager operations handled correctly" << endl;
    }

    cout << "\n✓ Testing null object add..." << endl;
    {
        bool result = manager.Add(1, nullptr);
        assert(result == false);
        cout << "  Adding nullptr correctly rejected" << endl;
    }

    cout << "\n✓ Testing duplicate key add..." << endl;
    {
        auto obj1 = new TestObject(1, "First");
        auto obj2 = new TestObject(2, "Second");
        
        assert(manager.Add(100, obj1) == true);
        assert(manager.Add(100, obj2) == false); // 重复键
        
        delete obj2; // 手动删除未添加的对象
        cout << "  Duplicate key correctly rejected" << endl;
    }

    cout << "\n✓ Testing GetKeyByValue with nullptr..." << endl;
    {
        TestObject* fake_obj = reinterpret_cast<TestObject*>(0x12345678);
        int key;
        bool result = manager.GetKeyByValue(fake_obj, &key, nullptr, false);
        assert(result == false);
        cout << "  Non-existent object correctly returned false" << endl;
    }

    cout << "\n✓ Testing multiple Clear calls..." << endl;
    {
        manager.Clear();
        manager.Clear(); // 应该安全
        assert(manager.GetCount() == 0);
        cout << "  Multiple Clear() calls handled safely" << endl;
    }
}

// ============================================================================
// 测试 6: 实际使用场景
// ============================================================================

class Asset
{
public:
    string filename;
    int size;
    
    Asset(const string& fn, int sz) : filename(fn), size(sz)
    {
        cout << "  [Load] Asset: " << filename << " (" << size << " bytes)" << endl;
    }
    
    ~Asset()
    {
        cout << "  [Unload] Asset: " << filename << endl;
    }
};

void TestRealWorldScenario()
{
    cout << "\n" << string(80, '=') << endl;
    cout << "Test 6: Real-World Scenario (Asset Manager)" << endl;
    cout << string(80, '=') << endl;

    ManagedObjectRegistry<string, Asset> asset_manager;

    cout << "\n✓ Loading assets..." << endl;
    {
        asset_manager.Add("texture.png", new Asset("texture.png", 2048));
        asset_manager.Add("model.obj", new Asset("model.obj", 65536));
        asset_manager.Add("sound.wav", new Asset("sound.wav", 16384));
        cout << "  Loaded " << asset_manager.GetCount() << " assets" << endl;
    }

    cout << "\n✓ Using assets (acquiring references)..." << endl;
    {
        // 场景1 使用 texture 和 model
        Asset* tex1 = asset_manager.Get("texture.png");
        Asset* mdl1 = asset_manager.Get("model.obj");
        
        // 场景2 也使用 texture
        Asset* tex2 = asset_manager.Get("texture.png");
        
        cout << "  Scene 1 acquired texture and model" << endl;
        cout << "  Scene 2 acquired texture" << endl;
        
        uint tex_refs = 0;
        asset_manager.GetKeyByValue(tex1, nullptr, &tex_refs, false);
        cout << "  Texture ref_count: " << tex_refs << endl;
        assert(tex_refs == 3); // 初始1 + Get两次
    }

    cout << "\n✓ Releasing unused assets..." << endl;
    {
        // 场景1 结束，释放资源
        asset_manager.Release("texture.png", false);   // ref_count: 3 -> 2
        asset_manager.Release("model.obj", false);     // ref_count: 2 -> 1
        
        // 场景2 结束，释放资源
        asset_manager.Release("texture.png", false);   // ref_count: 2 -> 1
        
        cout << "  Released assets from scenes" << endl;
        cout << "  Assets still loaded: " << asset_manager.GetCount() << endl;
        // 此时都没被删除，因为都还有ref_count > 0
        assert(asset_manager.GetCount() == 3);
    }

    cout << "\n✓ Final Release with zero_clear..." << endl;
    {
        // 再释放一次，使ref_count到0
        asset_manager.Release("texture.png", true);   // ref_count: 1 -> 0，删除！
        asset_manager.Release("model.obj", true);     // ref_count: 1 -> 0，删除！
        
        cout << "  Assets after final release: " << asset_manager.GetCount() << endl;
        assert(asset_manager.GetCount() == 1); // 只剩 sound.wav
    }

    cout << "\n✓ Cleaning up unused assets..." << endl;
    {
        asset_manager.ClearFree(); // 删除 ref_count=0 的资源
        cout << "  Assets after cleanup: " << asset_manager.GetCount() << endl;
    }

    cout << "\n✓ Final cleanup..." << endl;
    asset_manager.Clear();
}

// ============================================================================
// 主函数
// ============================================================================

int main()
{
    cout << "\n" << string(80, '═') << endl;
    cout << "   ObjectManager Comprehensive Test Suite" << endl;
    cout << string(80, '═') << endl;

    try
    {
        TestBasicOperations();
        TestReferenceCount();
        TestZeroClear();
        TestAutoIdManager();
        TestEdgeCases();
        TestRealWorldScenario();

        cout << "\n" << string(80, '═') << endl;
        cout << "✓ All tests passed successfully!" << endl;
        cout << string(80, '═') << endl;

        cout << "\n【测试覆盖】\n" << endl;
        cout << "✓ 基础操作: Add, Find, Get, GetCount" << endl;
        cout << "✓ 引用计数: Get 增加, Release 减少" << endl;
        cout << "✓ 内存管理: Clear, ClearFree, zero_clear" << endl;
        cout << "✓ 查找: GetKeyByValue, Find by key" << endl;
        cout << "✓ 自动ID: AutoIdObjectManager, 重复对象检测" << endl;
        cout << "✓ 边界情况: 空管理器, nullptr, 重复键" << endl;
        cout << "✓ 实际场景: 资源管理器模拟" << endl;
    }
    catch (const exception& e)
    {
        cerr << "\n✗ Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
