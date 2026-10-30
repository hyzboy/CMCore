#include<hgl/type/DataType.h>
#include<hgl/type/Queue.h>
#include<iostream>
#include<vector>
#include<cassert>

using namespace hgl;
using namespace std;

static void AssertEquals(const vector<int> &actual, initializer_list<int> expected, const char *label)
{
    if(actual.size() != expected.size())
    {
        cout << "  [FAIL] " << label << ": size " << actual.size() << " != " << expected.size() << endl;
        assert(false);
    }

    size_t idx = 0;
    for(int v : expected)
    {
        if(actual[idx] != v)
        {
            cout << "  [FAIL] " << label << ": index " << idx << " is " << actual[idx] << ", expected " << v << endl;
            assert(false);
        }
        ++idx;
    }

    cout << "  [PASS] " << label << endl;
}

static vector<int> Iterate(const Queue<int> &q)
{
    vector<int> out;
    for(const int &v : q)
        out.push_back(v);
    return out;
}

int os_main(int, os_char **)
{
    cout << "\n========================================" << endl;
    cout << "QueueTest05_WriteArrayAndPushConst" << endl;
    cout << "========================================" << endl;

    // ------------------------------------------------------------------
    // 1) Push(const T*) / Push(const T&) 接受常量与临时量
    // ------------------------------------------------------------------
    {
        const int src[3] = {1, 2, 3};

        Queue<int> q;
        assert(q.Push(src, 3));
        assert(q.Push(4));              // 右值（临时量）
        assert(q.Push(5));

        AssertEquals(Iterate(q), {1, 2, 3, 4, 5}, "Push(const T*) + Push(const T&)");
    }

    // ------------------------------------------------------------------
    // 2) GetWriteArray 与旧名 GetArray 等价
    // ------------------------------------------------------------------
    {
        Queue<int> q;
        q.Push(1);

        assert(&q.GetWriteArray() == &q.GetArray());
        assert(q.GetWriteArray().size() == 1);

        const Queue<int> &cq = q;
        assert(&cq.GetWriteArray() == &cq.GetArray());
        assert(cq.GetWriteArray().size() == 1);

        // 只读快照与写段读取结果一致（此时读段为空）
        const vector<int> snapshot = q.GetUnreadSnapshot();
        AssertEquals(snapshot, {1}, "GetUnreadSnapshot matches write array");
    }

    // ------------------------------------------------------------------
    // 3) 迭代器五种场景
    // ------------------------------------------------------------------
    // 3.1 只有写段有数据
    {
        Queue<int> q;
        q.Push(1);
        q.Push(2);
        AssertEquals(Iterate(q), {1, 2}, "iterator: write segment only");
    }

    // 3.2 只有读段有数据
    {
        Queue<int> q;
        q.Push(1);
        q.Push(2);
        q.Push(3);

        int v;
        assert(q.Pop(v));               // 触发段切换：read=原写段，write=已清空的旧读段
        assert(v == 1);

        AssertEquals(Iterate(q), {2, 3}, "iterator: read segment only");
    }

    // 3.3 读写两段都有数据
    {
        Queue<int> q;
        q.Push(1);
        q.Push(2);

        int v;
        assert(q.Pop(v));               // 段切换
        q.Push(3);
        q.Push(4);

        AssertEquals(Iterate(q), {2, 3, 4}, "iterator: both segments");
    }

    // 3.4 两段都为空
    {
        Queue<int> q;
        assert(Iterate(q).empty());
        assert(q.GetCount() == 0);
    }

    // 3.5 交错 Push/Pop 之后
    {
        Queue<int> q;

        int v;
        for(int i = 0; i < 4; ++i)
            q.Push(i);

        for(int i = 0; i < 4; ++i)
        {
            assert(q.Push(100 + i));
            assert(q.Pop(v));
            assert(v == i);
        }

        AssertEquals(Iterate(q), {100, 101, 102, 103}, "iterator: interleaved push/pop");
        assert(q.GetCount() == 4);
    }

    // 3.6 读段读尽后迭代应从写段开始
    {
        Queue<int> q;
        q.Push(1);

        int v;
        assert(q.Pop(v));
        assert(v == 1);
        assert(q.IsEmpty());

        q.Push(9);
        AssertEquals(Iterate(q), {9}, "iterator: restarts from write after drain");
    }

    cout << "\nAll QueueTest05 assertions passed." << endl;
    return 0;
}
