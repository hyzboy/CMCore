#include<hgl/type/FlatOrderedSet.h>
#include<iostream>

using namespace hgl;

int main()
{
    std::cout << "Testing empty FlatOrderedSet GetLast..." << std::endl;

    FlatOrderedSet<int> set;
    int val = 999;

    std::cout << "Set count: " << set.GetCount() << std::endl;
    std::cout << "Set is empty: " << set.IsEmpty() << std::endl;

    std::cout << "Calling GetLast..." << std::endl;
    bool result = set.GetLast(val);
    std::cout << "GetLast returned: " << result << std::endl;
    std::cout << "val: " << val << std::endl;

    std::cout << "Done!" << std::endl;
    return 0;
}
