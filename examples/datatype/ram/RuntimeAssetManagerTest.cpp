#include<iostream>
#include<iomanip>
#include<random>
#include"RAM_TestClass.h"

using namespace hgl;

void CreateTestObject()
{
    srand(time(nullptr));

    uint32_t ic=rand()%10+1;
    uint32_t pc=rand()%10+1;

    std::cout<<"Instance Count: "<<ic<<std::endl;
    std::cout<<"PhysicalDevice Count: "<<pc<<std::endl;

    Instance **ii=new Instance *[ic];
    PhysicalDevice **pd=new PhysicalDevice *[pc];

    for(uint32_t i=0;i<ic;i++)
        ii[i]=Instance::CreateInstance(i);

    for(uint32_t i=0;i<pc;i++)
    {
        uint32_t iid=rand()%ic;

        PhysicalDevice *pd=PhysicalDevice::CreateInstance(i,"PD"+AnsiString::numberOf(i),iid);

        ii[iid]->AddPhysicalDevice(pd->GetID());
    }

    delete[] pd;
    delete[] ii;
}

void OutputTestObject()
{
    const auto &ram=Instance::GetRAM();

    for(int i=0;i<ram.GetCount();i++)
    {
        Instance *inst=Instance::GetInstance(i);
        const auto &pd_set=inst->GetPhysicalDevices();

        std::cout<<"Instance "<<inst->GetID()<<" have "<<pd_set.GetCount()<<" physical device."<<std::endl;

        for(const auto &pd_id:pd_set)
        {
            PhysicalDevice *pd=PhysicalDevice::GetInstance(pd_id);

            std::cout<<"    PhysicalDevice "<<pd->GetID()<<" name: "<<pd->device_name.c_str()<<std::endl;
        }
    }
}

int main(int argc, char **argv)
{
    CreateTestObject();
    OutputTestObject();

    return 0;
}
