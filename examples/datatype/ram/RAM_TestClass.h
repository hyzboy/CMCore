#pragma once
#include"RuntimeAssetManager.h"
#include<hgl/type/String.h>
#include<hgl/type/OrderedValueSet.h>
#include<iostream>

using namespace hgl;

using InstanceID        =uint64;
using PhysicalDeviceID  =uint64;

struct Instance:public RuntimeAsset<InstanceID,Instance>
{
    OrderedValueSet<PhysicalDeviceID> physical_devices;

public:

    using RuntimeAsset::RuntimeAsset;

    ~Instance()
    {
        std::cout<<"~Instance("<<GetID()<<")"<<std::endl;
    }

    void AddPhysicalDevice(PhysicalDeviceID pd_id)
    {
        physical_devices.Add(pd_id);
    }

    const OrderedValueSet<PhysicalDeviceID> &GetPhysicalDevices()const{return physical_devices;}
};

struct PhysicalDevice:public RuntimeAsset<PhysicalDeviceID,PhysicalDevice>
{
    InstanceID inst_id;

    AnsiString device_name;

public:

    using RuntimeAsset::RuntimeAsset;

    ~PhysicalDevice()
    {
        std::cout<<"~PhysicalDevice("<<GetID()<<")"<<std::endl;
    }

    bool Init(const AnsiString &name,const InstanceID &iid)
    {
        device_name=name;
        inst_id=iid;

        return(true);
    }
};
