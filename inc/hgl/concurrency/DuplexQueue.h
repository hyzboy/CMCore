#pragma once

#include<hgl/concurrency/SpscQueue.h>

namespace hgl
{
    /**
     * @brief CN: 由两个有界 SPSC 队列组成的双向请求/响应通道。
     *        EN: Bidirectional request/response channel built from two bounded SPSC queues.
     * @details CN: request 方向由请求生产者调用 Send、由请求消费者调用 Recv；
     *              response 方向由响应生产者调用 SendResponse、由响应消费者调用
     *              RecvResponse。每个方向各自必须遵守“恰好一个生产者和一个消费者”
     *              的约束，两个方向彼此独立。队列满时发送返回 false，不会覆盖旧数据；
     *              Request/Response 类型必须满足底层 SpscQueue 的可平凡复制要求。
     *          EN: The request producer calls Send and its consumer calls Recv; the
     *              response producer calls SendResponse and its consumer calls
     *              RecvResponse. Each direction independently requires exactly one
     *              producer and one consumer. A full queue rejects sends without
     *              overwriting data. Both value types must be trivially copyable.
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

        bool Send(const Request &request){return request_queue.Push(request);}       ///< @brief CN: 非阻塞发送请求；队列满时返回 false。 EN: Send a request without blocking; return false when full.
        bool Recv(Request &request){return request_queue.Pop(request);}              ///< @brief CN: 非阻塞接收请求；队列空时返回 false。 EN: Receive a request without blocking; return false when empty.

        bool SendResponse(const Response &response){return response_queue.Push(response);} ///< @brief CN: 非阻塞发送响应；队列满时返回 false。 EN: Send a response without blocking; return false when full.
        bool RecvResponse(Response &response){return response_queue.Pop(response);}        ///< @brief CN: 非阻塞接收响应；队列空时返回 false。 EN: Receive a response without blocking; return false when empty.

        int GetRequestCount()const{return request_queue.GetCount();}
        int GetResponseCount()const{return response_queue.GetCount();}

        uint64 GetRequestDroppedCount()const{return request_queue.GetDroppedCount();}
        uint64 GetResponseDroppedCount()const{return response_queue.GetDroppedCount();}

        bool IsValid()const{return request_queue.IsValid()&&response_queue.IsValid();}
    };//class DuplexQueue
}//namespace hgl
