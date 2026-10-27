#include<hgl/thread/Thread.h>
#include<hgl/time/Time.h>

using namespace hgl;

class CooperativeThread:public Thread
{
public:

    bool Execute()override
    {
        SleepSecond(0.001);
        return(true);
    }
};

class GatedThread:public Thread
{
public:

    atom_bool release{false};

    bool Execute()override
    {
        while(!release.load())
            SleepSecond(0.001);

        return(false);
    }
};

int main()
{
    // CN: 验证“仅请求停止后再 Join”与“组合 StopAndJoin”都能结束可协作线程，
    //     且成功 Join 后同一 Thread 对象可以再次启动。
    // EN: Verify that RequestStop plus Join and StopAndJoin both end a cooperative
    //     thread, and that the object can restart after a successful Join.
    CooperativeThread cooperative;

    if(!cooperative.Start())
        return(1);

    if(!cooperative.IsRunning())
        return(2);

    cooperative.RequestStop();

    if(!cooperative.Join(2.0)||cooperative.IsRunning())
        return(2);

    if(!cooperative.Start())
        return(3);

    if(!cooperative.StopAndJoin(2.0)||cooperative.IsRunning())
        return(4);

    // CN: Join 不会发停止请求；未释放门闩时应超时，释放后才能自然结束。
    // EN: Join does not request stop; it should time out while the gate is closed and
    //     complete naturally after the gate is released.
    GatedThread gated;

    if(!gated.Start())
        return(5);

    if(gated.Join(0.01))
        return(6);

    gated.release.store(true);

    if(!gated.Join(2.0)||gated.IsRunning())
        return(7);

    return(0);
}
