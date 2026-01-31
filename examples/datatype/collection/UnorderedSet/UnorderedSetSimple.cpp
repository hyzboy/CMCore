/**
 * UnorderedSetSimple.cpp
 * UnorderedSet 简单使用示例
 */

#include <hgl/type/UnorderedSet.h>
#include <iostream>

using namespace hgl;
using namespace std;

int main()
{
    cout << "=== UnorderedSet Simple Usage Example ===" << endl;

    // 基本使用：整数集合
    {
        cout << "\n[Example 1: Integer Set]" << endl;

        UnorderedSet<int> numbers;

        // 添加元素
        numbers.Add(42);
        numbers.Add(17);
        numbers.Add(99);
        numbers.Add(42);  // 重复，会被忽略

        cout << "Count: " << numbers.GetCount() << endl;

        // 检查元素
        if (numbers.Contains(42))
            cout << "42 is in the set" << endl;

        // 遍历
        cout << "Elements: ";
        numbers.Enum([](int value) { cout << value << " "; });
        cout << endl;

        // 删除
        numbers.Delete(17);
        cout << "After deleting 17, count: " << numbers.GetCount() << endl;
    }

    // 字符串集合
    {
        cout << "\n[Example 2: String Set]" << endl;

        UnorderedSet<std::string> fruits;

        fruits.Add("apple");
        fruits.Add("banana");
        fruits.Add("cherry");

        cout << "Count: " << fruits.GetCount() << endl;
        cout << "Fruits: ";
        fruits.Enum([](const std::string& s) { cout << s << " "; });
        cout << endl;

        if (fruits.Contains("banana"))
            cout << "We have banana!" << endl;
    }

    // 使用迭代器
    {
        cout << "\n[Example 3: Using Iterators]" << endl;

        UnorderedSet<int> nums;
        nums.Add(10);
        nums.Add(20);
        nums.Add(30);

        cout << "Using iterator: ";
        for (auto it = nums.begin(); it != nums.end(); ++it)
            cout << *it << " ";
        cout << endl;

        cout << "Using range-based for: ";
        for (int num : nums)
            cout << num << " ";
        cout << endl;
    }

    // 集合运算
    {
        cout << "\n[Example 4: Set Operations]" << endl;

        UnorderedSet<int> set1, set2;

        set1.Add(1); set1.Add(2); set1.Add(3);
        set2.Add(2); set2.Add(3); set2.Add(4);

        // 并集
        UnorderedSet<int> unionSet;
        unionSet.Add(1); unionSet.Add(2); unionSet.Add(3);
        int added = unionSet.Union(set2);
        cout << "Union: {1,2,3} ∪ {2,3,4} = {";
        bool first = true;
        unionSet.Enum([&](int v) { 
            if (!first) cout << ","; 
            cout << v; 
            first = false;
        });
        cout << "}, added " << added << endl;

        // 交集
        UnorderedSet<int> intersectSet;
        intersectSet.Add(1); intersectSet.Add(2); intersectSet.Add(3);
        int removed = intersectSet.Intersect(set2);
        cout << "Intersection: {1,2,3} ∩ {2,3,4} = {";
        first = true;
        intersectSet.Enum([&](int v) { 
            if (!first) cout << ","; 
            cout << v; 
            first = false;
        });
        cout << "}, removed " << removed << endl;
    }

    cout << "\n=== All examples completed ✅ ===" << endl;
    return 0;
}
