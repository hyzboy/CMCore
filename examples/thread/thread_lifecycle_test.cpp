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
