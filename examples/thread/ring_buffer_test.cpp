#include<hgl/concurrency/RingBuffer.h>

int main()
{
    // CN: 先写入并消费部分数据，再跨越物理缓冲区末端写入，验证环绕读写顺序。
    // EN: Partially write and consume data, then write across the physical end to verify wraparound order.
    hgl::RingBuffer<unsigned char> buffer(8);
    const unsigned char initial[]={1,2,3,4,5,6};
    unsigned char output[6]={};

    if(buffer.SafeWrite(initial,6)!=6||buffer.SafeGetReadSize()!=6)
        return(1);

    if(buffer.SafeRead(output,5)!=5)
        return(2);

    for(int i=0;i<5;i++)
        if(output[i]!=unsigned char(i+1))
            return(3);

    const unsigned char wrapped[]={7,8,9,10,11};
    if(buffer.SafeWrite(wrapped,5)!=5||buffer.SafeGetReadSize()!=6)
        return(4);

    if(buffer.SafeRead(output,6)!=6)
        return(5);

    for(int i=0;i<6;i++)
        if(output[i]!=unsigned char(i+6))
            return(6);

    return(0);
}
