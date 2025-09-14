#pragma once

#include<unordered_map>

namespace hgl
{
    template<typename ID,typename T> class ActiveObjectManager
    {
        struct ObjectItem
        {
            ID id;
            T *object;

            int ref_count;
        };

    public:

        virtual void OnCreate(ObjectItem *item)
        {
            //指这个对象被create出来
        }

        virtual void OnActive(ObjectItem *item)
        {
            //指这个对象从idle切回active，而非是create出来
        }

        virtual void OnIdle(ObjectItem *item)
        {
            //指这个对象从active切到idle
        }

        virtual void OnClear(ObjectItem *item)
        {
            //指这个对象被clear掉
        }

    protected:

        std::unordered_map<ID,ObjectItem *> active_object_map;
        std::unordered_map<ID,ObjectItem *> idle_object_map;

        ActiveObjectManager() {}

        template<typename... ARGS>
        ObjectItem *CreateObject(const ID &id,ARGS ...args)
        {
            ObjectItem *item = new ObjectItem;

            item->id = id;
            item->object = new T(args...);
            item->ref_count = 0;

            OnCreate(item);

            return item;
        }

        void Clear(std::unordered_map<ID,ObjectItem *> &m)
        {
            for(auto &pair : m)
            {
                OnClear(pair.second);

                delete pair.second->object;
                delete pair.second;
            }
            m.clear();
        }

        void MoveToIdle(ObjectItem *item)
        {
            active_object_map.erase(item->id);
            idle_object_map[item->id] = item;

            OnIdle(item);
        }

        void MoveToActive(ObjectItem *item)
        {
            idle_object_map.erase(item->id);
            active_object_map[item->id] = item;

            OnActive(item);
        }

        void Release(ObjectItem *item)
        {
            --item->ref_count;

            if(item->ref_count <= 0)
                MoveToIdle(item);
        }

    public:

        virtual ~ActiveObjectManager()
        {
            Clear(active_object_map);
            Clear(idle_object_map);
        }

        int     GetActiveCount  ()              const   { return static_cast<int>(active_object_map.size()); }
        int     GetIdleCount    ()              const   { return static_cast<int>(idle_object_map.size()); }

        int     GetTotalCount   ()              const   { return GetActiveCount() + GetIdleCount(); }

        bool    IsActive        (const ID & id) const   { return active_object_map.contains(id); }
        bool    IsIdle          (const ID & id) const   { return idle_object_map.contains(id); }

        void    ClearActive     ()                      { Clear(active_object_map); }
        void    ClearIdle       ()                      { Clear(idle_object_map); }

    public:

        class ObjectRef
        {
            ActiveObjectManager<ID,T> *manager;
            ObjectItem *item;

        private:

            friend class ActiveObjectManager<ID,T>;

            ObjectRef(ActiveObjectManager<ID,T> *m,ObjectItem *it): manager(m),item(it)
            {
                if(item)
                    ++item->ref_count;
            }

        public:

            // Copy constructor
            ObjectRef(const ObjectRef &other): manager(other.manager),item(other.item)
            {
                if(item)
                    ++item->ref_count;
            }

            // Copy assignment
            ObjectRef &operator=(const ObjectRef &other)
            {
                if(this == &other)
                    return *this;

                // release current
                if(item && manager)
                    manager->Release(item);

                manager = other.manager;
                item = other.item;

                if(item)
                    ++item->ref_count;

                return *this;
            }

            // Move constructor
            ObjectRef(ObjectRef &&other) noexcept: manager(other.manager),item(other.item)
            {
                other.manager = nullptr;
                other.item = nullptr;
            }

            // Move assignment
            ObjectRef &operator=(ObjectRef &&other) noexcept
            {
                if(this == &other)
                    return *this;

                if(item && manager)
                    manager->Release(item);

                manager = other.manager;
                item = other.item;

                other.manager = nullptr;
                other.item = nullptr;

                return *this;
            }

            ~ObjectRef()
            {
                if(!item || !manager)
                    return;

                manager->Release(item);
            }

            bool IsValid() const
            {
                return item != nullptr;
            }

            void Release()
            {
                if(!item || !manager)
                    return;

                manager->Release(item);
                item = nullptr;
            }

            T *operator ->()
            {
                if(!item || item->ref_count <= 0)
                    return nullptr;

                return item->object;
            }

            T *operator ->() const
            {
                if(!item || item->ref_count <= 0)
                    return nullptr;

                return item->object;
            }
        };//class ObjectRef

        template<typename... ARGS>
        ObjectRef GetOrCreate(const ID &id,ARGS...args)
        {
            auto it = active_object_map.find(id);

            if(it != active_object_map.end())
                return ObjectRef(this,it->second);

            it = idle_object_map.find(id);

            if(it != idle_object_map.end())
            {
                MoveToActive(it->second);
                return ObjectRef(this,it->second);
            }

            ObjectItem *item = CreateObject(id,args...);

            active_object_map[id] = item;

            return ObjectRef(this,item);
        }

        ObjectRef Get(const ID &id)
        {
            auto it = active_object_map.find(id);

            if(it != active_object_map.end())
                return ObjectRef(this,it->second);

            it = idle_object_map.find(id);

            if(it != idle_object_map.end())
            {
                MoveToActive(it->second);
                return ObjectRef(this,it->second);
            }

            return ObjectRef(this,nullptr);
        }
    };//class ActiveObjectManager
}//namespace hgl
