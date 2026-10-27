#pragma once

#include<hgl/thread/SpscQueue.h>

namespace hgl
{
    /**
    * Two bounded SPSC queues forming a bidirectional request/response channel.
    * Each direction has its own single producer and single consumer contract.
    */
    template<typename Request,typename Response>
    class DuplexQueue
    {
        SpscQueue<Request> request_queue;
        SpscQueue<Response> response_queue;

    public:

        explicit DuplexQueue(uint32 capacity=1024)
            :request_queue(capacity),response_queue(capacity)
        {
        }

        bool Send(const Request &request){return request_queue.Push(request);}
        bool Recv(Request &request){return request_queue.Pop(request);}

        bool SendResponse(const Response &response){return response_queue.Push(response);}
        bool RecvResponse(Response &response){return response_queue.Pop(response);}

        int GetRequestCount()const{return request_queue.GetCount();}
        int GetResponseCount()const{return response_queue.GetCount();}

        uint64 GetRequestDroppedCount()const{return request_queue.GetDroppedCount();}
        uint64 GetResponseDroppedCount()const{return response_queue.GetDroppedCount();}

        bool IsValid()const{return request_queue.IsValid()&&response_queue.IsValid();}
    };//class DuplexQueue
}//namespace hgl
