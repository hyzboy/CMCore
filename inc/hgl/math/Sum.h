/**
 * Sum.h - 数组求和函数
 * 
 * 包含：
 * - sum 模板函数（计算数组元素总和）
 */

#pragma once

namespace hgl
{
    /**
     * 求一批数的和
     * @param result 输出：求和结果
     * @param data 输入：数据数组
     * @param count 数组元素个数
     * 
     * 示例：
     * int arr[] = {1, 2, 3, 4, 5};
     * int total;
     * sum(&total, arr, 5);  // total = 15
     */
    template<typename R,typename T>
    void sum(R *result,const T *data,const int count)
    {
        if(!result || !data || count <= 0)return;

        *result=0;

        for(int i=0;i<count;i++)
        {
            *result+=*data;
            ++data;
        }
    }
}//namespace hgl
