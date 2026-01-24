#pragma once

#include<hgl/type/OrderedValueSet.h>
#include<hgl/io/event/InputEventSource.h>

/*
* 现阶段事件分发流程：
*
* 操作系统事件    ->      Window  ->   RenderFramework    -> WorkManager
*
*/

namespace hgl::io
{
    struct EventHeader
    {
        InputEventSource    type;       ///<输入源类型
        uint8               index;      ///<输入源索引(比如同一设备有多个)
        uint16              id;         ///<事件id
    };

    constexpr size_t EventHeaderBytes=sizeof(EventHeader);

    /**
     * 事件处理结果
     */
    enum class EventProcResult
    {
        Continue,
        Break,
    };

    /**
    * 事件分发器
    */
    class EventDispatcher
    {
    protected:

        InputEventSource                source_type;

        EventDispatcher *               parent_input_event=nullptr;

        OrderedValueSet<EventDispatcher *>    child_dispatchers;                      ///<子事件分发器列表

        EventDispatcher *               exclusive_dispatcher=nullptr;           ///<独占事件分发器,如果有则只处理这个分发器的事件

    protected:

        void SetParent(EventDispatcher *ie){parent_input_event=ie;}

    public:

        const   InputEventSource GetInputEventSource()const{return source_type;}

        virtual EventDispatcher *GetEventDispatcher(){return this;}

                EventDispatcher *GetParent()const{return parent_input_event;}

        virtual EventProcResult OnEvent(const EventHeader &header,const uint64 data);

    public:

        EventDispatcher()
        {
            source_type=InputEventSource::Root;
            parent_input_event=nullptr;
            exclusive_dispatcher=nullptr;
        }

        EventDispatcher(InputEventSource ies)
        {
            source_type=ies;
            parent_input_event=nullptr;
            exclusive_dispatcher=nullptr;
        }

        virtual ~EventDispatcher()
        {
            if(child_dispatchers.GetCount()>0)
            {
                for(auto ed : child_dispatchers)
                    ed->SetParent(nullptr);

                //child_dispatchers.Clear();    本质是要清理，但是这都析构函数了，就无所谓了。
            }

            if(parent_input_event)
                parent_input_event->RemoveChildDispatcher(this);
        }

        virtual bool AddChildDispatcher(EventDispatcher *ie);
        virtual bool RemoveChildDispatcher(EventDispatcher *ie);

        virtual bool SetExclusiveDispatcher(EventDispatcher *ie);
        virtual bool RemoveExclusiveDispatcher(EventDispatcher *ie);

        virtual bool Update(){return true;}
    };//class EventDispatcher
}//namespace hgl::io
