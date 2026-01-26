/**
 * ValueBuffer 浅拷贝导致双重释放 (Double-Free) 的演示
 * 
 * 核心问题：
 * - ValueBuffer 使用默认拷贝构造（浅拷贝）
 * - 两个对象共享同一块内存指针
 * - 第一个对象析构时释放内存
 * - 第二个对象析构时再次释放已释放的内存 -> CRASH!
 */

#include <iostream>
#include <cstring>

using namespace std;

// ==================== 简化的 ValueBuffer 实现 ====================
// 注意：没有自定义拷贝构造和赋值运算符，使用默认的浅拷贝！

template<typename T>
class SimpleBuggyValueBuffer
{
private:
    T *items;
    int count;
    int alloc_count;

public:
    SimpleBuggyValueBuffer() 
        : items(nullptr), count(0), alloc_count(0)
    {
        cout << "[Constructor] " << this << " created (items=" << (void*)items << ")" << endl;
    }

    // 默认拷贝构造：浅拷贝！
    // SimpleBuggyValueBuffer(const SimpleBuggyValueBuffer &other) = default;

    ~SimpleBuggyValueBuffer()
    {
        cout << "[Destructor] " << this << " destroying, attempting to delete " << (void*)items << endl;
        if(items)
        {
            cout << "  -> Freeing memory at " << (void*)items << endl;
            delete[] items;  // 这里可能释放已经释放的内存！
            items = nullptr;
        }
    }

    void Allocate(int size)
    {
        if(items) delete[] items;
        items = new T[size];
        count = size;
        alloc_count = size;
        cout << "[Allocate] " << this << " allocated " << size << " items at " 
             << (void*)items << endl;
    }

    void Resize(int size)
    {
        if(size <= 0)
        {
            cout << "[Resize] " << this << " resizing to " << size << endl;
            if(items) delete[] items;
            items = nullptr;
            count = 0;
            return;
        }

        if(size > alloc_count)
        {
            cout << "[Resize] " << this << " expanding from " << alloc_count << " to " << size << endl;
            T *new_items = new T[size];
            if(items)
            {
                memcpy(new_items, items, count * sizeof(T));
                delete[] items;
            }
            items = new_items;
            alloc_count = size;
        }

        count = size;
        cout << "[Resize] Done: count=" << count << " at " << (void*)items << endl;
    }

    int GetCount() const { return count; }
    int GetAllocCount() const { return alloc_count; }
    T *GetData() const { return items; }
};

// ==================== 演示 1: 直接浅拷贝 ====================

void Demo1_DirectShallowCopy()
{
    cout << "\n========================================" << endl;
    cout << "Demo 1: Direct Shallow Copy" << endl;
    cout << "========================================" << endl;

    {
        cout << "\nCreating buf1..." << endl;
        SimpleBuggyValueBuffer<int> buf1;
        buf1.Allocate(100);

        cout << "\nCreating buf2 as copy of buf1..." << endl;
        cout << "  buf2 = buf1;  // 默认拷贝构造 - 浅拷贝！" << endl;
        SimpleBuggyValueBuffer<int> buf2 = buf1;  // 浅拷贝：buf2.items = buf1.items

        cout << "\nBoth buf1 and buf2 point to the SAME memory!" << endl;
        cout << "  buf1.items = " << (void*)buf1.GetData() << endl;
        cout << "  buf2.items = " << (void*)buf2.GetData() << endl;

        cout << "\nExiting scope - both destructors will be called..." << endl;
    }  // buf2 析构 -> delete[]; buf1 析构 -> delete[] 已释放的内存 -> CRASH!

    cout << "ERROR: If you see this, the program didn't crash!" << endl;
    cout << "        (Debug heap detection may have prevented it)" << endl;
}

// ==================== 演示 2: 返回值导致的浅拷贝 ====================

template<typename T>
class SimpleQueue
{
private:
    SimpleBuggyValueBuffer<T> internal_buffer;

public:
    // 按值返回 ValueBuffer - 这会导致浅拷贝！
    SimpleBuggyValueBuffer<T> GetSnapshot() const
    {
        cout << "  [GetSnapshot] Creating temporary ValueBuffer..." << endl;
        SimpleBuggyValueBuffer<T> result;
        result.Allocate(50);
        cout << "  [GetSnapshot] Returning by value (will be copied)..." << endl;
        return result;  // 返回值：result 会被复制
    }
};

void Demo2_ReturnByValue()
{
    cout << "\n========================================" << endl;
    cout << "Demo 2: Return By Value (浅拷贝)" << endl;
    cout << "========================================" << endl;

    {
        SimpleQueue<int> queue;

        cout << "\nCalling GetSnapshot()..." << endl;
        SimpleBuggyValueBuffer<int> snapshot = queue.GetSnapshot();

        cout << "\nSnapshot obtained. Both the temporary and snapshot" << endl;
        cout << "point to the same memory!" << endl;

        cout << "\nExiting scope - destructors will be called..." << endl;
    }  // snapshot 析构; 之前的临时对象也会析构 -> 双重释放!
}

// ==================== 演示 3: const 引用绑定临时对象 ====================

void Demo3_ConstRefBindTemporary()
{
    cout << "\n========================================" << endl;
    cout << "Demo 3: const Reference Binding Temporary" << endl;
    cout << "========================================" << endl;

    cout << "\nThis is what happened in the original bug:" << endl;
    cout << "const ValueBuffer<int> &active_view = adm.GetActiveView();" << endl;

    {
        SimpleQueue<int> queue;

        cout << "\nBinding const reference to temporary..." << endl;
        const SimpleBuggyValueBuffer<int> &snapshot = queue.GetSnapshot();

        cout << "\nReference is alive, but points to temporary object" << endl;
        cout << "Snapshot data count: " << snapshot.GetCount() << endl;

        cout << "\nExiting scope - temporary will be destroyed first..." << endl;
    }  // 临时对象析构; 但 snapshot 引用的内存已被释放!
       // 如果之后访问 snapshot，会是 use-after-free

    cout << "\nDone (if we get here)" << endl;
}

// ==================== 演示 4: 正确的做法 ====================

template<typename T>
class CorrectValueBuffer
{
private:
    T *items;
    int count;
    int alloc_count;

public:
    CorrectValueBuffer() 
        : items(nullptr), count(0), alloc_count(0)
    {
        cout << "[Correct::Constructor] " << this << " created" << endl;
    }

    // 显式删除拷贝操作（禁止浅拷贝）
    CorrectValueBuffer(const CorrectValueBuffer &) = delete;
    CorrectValueBuffer &operator=(const CorrectValueBuffer &) = delete;

    // 允许移动
    CorrectValueBuffer(CorrectValueBuffer &&other) noexcept
        : items(other.items), count(other.count), alloc_count(other.alloc_count)
    {
        cout << "[Correct::Move Constructor] " << this << " (from " << &other << ")" << endl;
        other.items = nullptr;
        other.count = 0;
        other.alloc_count = 0;
    }

    ~CorrectValueBuffer()
    {
        cout << "[Correct::Destructor] " << this << " deleting " << (void*)items << endl;
        if(items) delete[] items;
    }

    void Allocate(int size)
    {
        if(items) delete[] items;
        items = new T[size];
        count = size;
        alloc_count = size;
        cout << "[Correct::Allocate] " << this << " allocated at " << (void*)items << endl;
    }

    int GetCount() const { return count; }
    T *GetData() const { return items; }
};

template<typename T>
class CorrectQueue
{
private:
    CorrectValueBuffer<T> internal_buffer;

public:
    // 返回 const 引用 - 避免拷贝
    const CorrectValueBuffer<T> &GetSnapshot() const
    {
        cout << "  [CorrectGetSnapshot] Returning const reference..." << endl;
        return internal_buffer;
    }
};

void Demo4_CorrectWay()
{
    cout << "\n========================================" << endl;
    cout << "Demo 4: The CORRECT Way (返回 const 引用)" << endl;
    cout << "========================================" << endl;

    {
        CorrectQueue<int> queue;

        cout << "\nCalling GetSnapshot() that returns const ref..." << endl;
        const CorrectValueBuffer<int> &snapshot = queue.GetSnapshot();

        cout << "\nSnapshot obtained (no copy involved!)" << endl;
        cout << "Snapshot data count: " << snapshot.GetCount() << endl;

        cout << "\nExiting scope - only one destructor called..." << endl;
    }

    cout << "Success! No crash, no double-free." << endl;
}

// ==================== Main ====================

int main(int argc, char *argv[])
{
    cout << "============================================" << endl;
    cout << "ValueBuffer Shallow Copy & Double-Free Demo" << endl;
    cout << "============================================" << endl;

    if(argc > 1)
    {
        int demo = atoi(argv[1]);
        switch(demo)
        {
            case 1: Demo1_DirectShallowCopy(); break;
            case 2: Demo2_ReturnByValue(); break;
            case 3: Demo3_ConstRefBindTemporary(); break;
            case 4: Demo4_CorrectWay(); break;
            default:
                cout << "Usage: " << argv[0] << " [1-4]" << endl;
                cout << "  1 = Direct shallow copy" << endl;
                cout << "  2 = Return by value" << endl;
                cout << "  3 = const ref binding temporary" << endl;
                cout << "  4 = Correct way (const ref return)" << endl;
                return 0;
        }
    }
    else
    {
        cout << "\nRunning Demo 4 (the correct way)..." << endl;
        Demo4_CorrectWay();

        cout << "\n" << endl;
        cout << "To see the bugs, run with arguments:" << endl;
        cout << "  ValueBufferShallowCopyDemo 1  - Direct shallow copy" << endl;
        cout << "  ValueBufferShallowCopyDemo 2  - Return by value" << endl;
        cout << "  ValueBufferShallowCopyDemo 3  - const ref binding temporary" << endl;
    }

    return 0;
}
