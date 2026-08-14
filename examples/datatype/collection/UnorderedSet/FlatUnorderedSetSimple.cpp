/**
 * UnOrderedValueSetTest00_Simple.cpp
 * 简单快速测试 - 验证 FlatUnorderedSet 基本功能
 */

#include<hgl/type/FlatUnorderedSet.h>
#include<iostream>

using namespace hgl;

int main()
{
    std::cout << "=== FlatUnorderedSet Simple Test ===" << std::endl;

    FlatUnorderedSet<int> set;

    // 测试添加
    std::cout << "\n1. Test Add:" << std::endl;
    bool r1 = set.Add(10);
    bool r2 = set.Add(20);
    bool r3 = set.Add(30);
    bool r4 = set.Add(10);  // 重复添加

    std::cout << "   Add 10: " << (r1 ? "OK" : "FAILED") << std::endl;
    std::cout << "   Add 20: " << (r2 ? "OK" : "FAILED") << std::endl;
    std::cout << "   Add 30: " << (r3 ? "OK" : "FAILED") << std::endl;
    std::cout << "   Duplicate Add 10: " << (r4 ? "OK" : "FAILED (expected)") << std::endl;
    std::cout << "   Set size: " << set.GetCount() << std::endl;

    // 测试查找
    std::cout << "\n2. Test Find:" << std::endl;
    std::cout << "   Contains(10): " << (set.Contains(10) ? "Yes" : "No") << std::endl;
    std::cout << "   Contains(20): " << (set.Contains(20) ? "Yes" : "No") << std::endl;
    std::cout << "   Contains(99): " << (set.Contains(99) ? "Yes" : "No (expected)") << std::endl;
    std::cout << "   Find(20) index: " << set.Find(20) << std::endl;
    std::cout << "   Find(99) index: " << set.Find(99) << " (should be -1)" << std::endl;

    // 测试枚举
    std::cout << "\n3. Enumerate all elements:" << std::endl;
    std::cout << "   ";
    set.Enum([](const int& value) {
        std::cout << value << " ";
    });
    std::cout << std::endl;

    // 测试删除
    std::cout << "\n4. Test Delete:" << std::endl;
    bool d1 = set.Delete(20);
    std::cout << "   Delete 20: " << (d1 ? "OK" : "FAILED") << std::endl;
    std::cout << "   Size after delete: " << set.GetCount() << std::endl;
    std::cout << "   Contains(20): " << (set.Contains(20) ? "Yes" : "No (expected)") << std::endl;

    // 测试清空
    std::cout << "\n5. Test Clear:" << std::endl;
    set.Clear();
    std::cout << "   Size after clear: " << set.GetCount() << std::endl;
    std::cout << "   IsEmpty: " << (set.IsEmpty() ? "Yes" : "No") << std::endl;

    std::cout << "\n=== Test Complete ===" << std::endl;
    return 0;
}
