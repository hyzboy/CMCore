/**
* DataArray测试
* 
*   DataArray是List模板的数据储存类，它也提供一些通用的简单处理。
*/

#include<hgl/type/DataArray.h>
#include<iostream>
#include<iomanip>

using namespace hgl;

DataArray<int> da_list;

void restart()
{
    da_list.Resize(10);       //设定里面有10个值

    for(int i=0;i<10;i++)
       da_list[i]=i;            //设置值
}

void output(const char *str)
{
    const int count=da_list.GetCount();

    std::cout<<std::setw(16)<<str<<"["<<std::setw(2)<<count<<"] ";

    const int *p=da_list.data();

    for(int i=0;i<count;i++)
    {
        if(i>0)
            std::cout<<","<<*p;
        else
            std::cout<<*p;

        ++p;
    }

    std::cout<<std::endl;

    restart();
}

int main(int,char **)
{
    restart();
    output("origin     ");

    da_list.Delete(0,3);       //删除从0号位置开始的3个值
    output("delete(0,3)");

    da_list.Delete(0,5);       //删除从0号位置开始的5个值
    output("delete(0,5)");

    da_list.Delete(3,2);       //删除从3号位置开始的2个值
    output("delete(3,2)");

    da_list.Delete(5,3);       //删除从5号位置开始的3个值
    output("delete(5,3)");

    da_list.Delete(5,5);       //删除从5号位置开始的5个值
    output("delete(5,5)");

    da_list.DeleteShift(0,3);   //删除从0号位置开始的3个值
    output("delete move(0,3)");

    da_list.Move(10,0,2);       //从0号位置开始的2个值移动到10号位置
    output("move(10,0,2)");

    da_list.Move(5,0,2);        //从0号位置开始的2个值移动到5号位置
    output("move(5,0,2)");

    da_list.Move(0,7,3);        //从7号位置开始的3个值移动到0号位置
    output("move(0,7,3)");

    da_list.Move(2,7,3);        //从7号位置开始的3个值移动到2号位置
    output("move(2,7,3)");

    da_list.Move(0,5,3);        //从5号位置开始的3个值移动到0号位置
    output("move(0,5,3)");

    da_list.Move(10,5,3);       //从5号位置开始的3个值移动到10号位置
    output("move(10,5,3)");

    da_list.Move(2,5,3);        //从5号位置开始的3个值移动到2号位置
    output("move(2,5,3)");

    da_list.Move(7,2,3);        //从2号位置开始的3个值移动到7号位置
    output("move(7,2,3)");

    int ins_begin=99;
    da_list.Insert(0,&ins_begin,1); //在开头插入
    output("insert(0,99)");

    int ins_mid[]={111,222};
    da_list.Insert(5,ins_mid,2);    //在中间插入两个
    output("insert(5,{111,222})");

    int ins_end=888;
    da_list.Insert(da_list.GetCount(),&ins_end,1); //末尾插入
    output("insert(end,888)");

    return(0);
}
