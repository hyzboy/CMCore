/**
 * DataArrayNew 调试测试
 * 逐步调试插入操作
 */

#include <hgl/type/DataArray.h>
#include <iostream>
#include <string>

using namespace hgl;

int main()
{
    std::cout << "Starting Debug Test..." << std::endl;

    DataArray<std::string> arr;

    std::cout << "Step 1: Append 3 strings" << std::endl;
    arr.Append("Hello");
    arr.Append("World");
    arr.Append("!");
    std::cout << "  Count: " << arr.GetCount() << ", Alloc: " << arr.GetAllocCount() << std::endl;

    std::cout << "Step 2: Resize to 5" << std::endl;
    arr.Resize(5);
    arr[3] = "C++";
    arr[4] = "Test";
    std::cout << "  Count: " << arr.GetCount() << ", Alloc: " << arr.GetAllocCount() << std::endl;

    std::cout << "Step 3: Try Insert(2, {\"A\", \"B\"}, 2)" << std::endl;
    std::cout << "  Before Insert: count=" << arr.GetCount() << ", alloc=" << arr.GetAllocCount() << std::endl;

    std::string data[] = {"A", "B"};
    std::cout << "  Calling Insert..." << std::endl;

    // 这里应该会有问题
    bool result = arr.Insert(2, data, 2);

    std::cout << "  Insert returned: " << result << std::endl;
    std::cout << "  After Insert: count=" << arr.GetCount() << ", alloc=" << arr.GetAllocCount() << std::endl;

    std::cout << "Step 4: Print elements" << std::endl;
    for(int i = 0; i < arr.GetCount(); i++)
        std::cout << "  [" << i << "] = " << arr[i] << std::endl;

    std::cout << "Done!" << std::endl;
    return 0;
}
