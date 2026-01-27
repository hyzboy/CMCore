#include<hgl/platform/CpuInfo.h>
#include<hgl/platform/BigLittleDetector.h>
#include<iostream>
#include<iomanip>

using namespace hgl;
using namespace std;

void PrintCpuInfo(const CpuInfo& ci)
{
    cout << "=== CPU Information ===" << endl;
    cout << "CPU Count: " << ci.cpu_count << endl;
    cout << "Core Count: " << ci.core_count << endl;
    cout << "Logical Core Count: " << ci.logical_core_count << endl;
}

int main()
{
    CpuInfo ci{};

    if(GetCpuInfo(&ci))
    {
        PrintCpuInfo(ci);
    }
    else
    {
        cout << "Failed to get CPU information!" << endl;
        return 1;
    }

    return 0;
}
