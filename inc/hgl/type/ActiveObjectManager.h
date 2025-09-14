/**
* @file ActiveObjectManager.h
* @brief CN:活跃对象管理器模板类，支持对象的活跃/闲置状态切换和引用计数。\nEN:Active object manager template class, supports active/idle state switching and reference counting.
*/
#pragma once

#include <unordered_map>

namespace hgl
{
    /**
    * @brief CN:活跃对象管理器模板类。\nEN:Active object manager template class.
    * @tparam ID CN:对象ID类型。EN:Object ID type.
    * @tparam T CN:对象类型。EN:Object type.
    */
    template<typename ID, typename T>
    class ActiveObjectManager
    {
        /**
        * @brief CN:对象项结构体。\nEN:Object item struct.
        */
        struct ObjectItem
        {
            /**
            * @brief CN:对象ID。\nEN:Object ID.
            */
            ID id;

            /**
            * @brief CN:对象指针。\nEN:Object pointer.
            */
            T *object;

            /**
            * @brief CN:引用计数。\nEN:Reference count.
            */
            int ref_count;

        };

    public:

        /**
        * @brief CN:对象创建时回调。\nEN:Callback when object is created.
        */
        virtual void OnCreate(ObjectItem *item)
        {
            // 指这个对象被create出来
        }

        /**
        * @brief CN:对象从闲置切回活跃时回调。\nEN:Callback when object switches from idle to active.
        */
        virtual void OnActive(ObjectItem *item)
        {
            // 指这个对象从idle切回active，而非是create出来
        }

        /**
        * @brief CN:对象从活跃切到闲置时回调。\nEN:Callback when object switches from active to idle.
        */
        virtual void OnIdle(ObjectItem *item)
        {
            // 指这个对象从active切到idle
        }

        /**
        * @brief CN:对象被清理时回调。\nEN:Callback when object is cleared.
        */
        virtual void OnClear(ObjectItem *item)
        {
            // 指这个对象被clear掉
        }

    protected:

        /**
        * @brief CN:活跃对象映射表。\nEN:Active object map.
        */
        std::unordered_map<ID, ObjectItem *> active_object_map;

        /**
        * @brief CN:闲置对象映射表。\nEN:Idle object map.
        */
        std::unordered_map<ID, ObjectItem *> idle_object_map;

        ActiveObjectManager() {}

        /**
        * @brief CN:创建对象项。\nEN:Create object item.
        */
        template<typename... ARGS>
        ObjectItem *CreateObject(const ID &id, ARGS ...args)
        {
            ObjectItem *item = new ObjectItem;

            item->id = id;
            item->object = new T(args...);
            item->ref_count = 0;

            OnCreate(item);

            return item;
        }

        /**
        * @brief CN:清理对象映射表。\nEN:Clear object map.
        */
        void Clear(std::unordered_map<ID, ObjectItem *> &m)
        {
            for (auto &pair : m)
            {
                OnClear(pair.second);

                delete pair.second->object;
                delete pair.second;
            }

            m.clear();
        }

        /**
        * @brief CN:将对象项移至闲置表。\nEN:Move object item to idle map.
        */
        void MoveToIdle(ObjectItem *item)
        {
            active_object_map.erase(item->id);
            idle_object_map[item->id] = item;

            OnIdle(item);
        }

        /**
        * @brief CN:将对象项移至活跃表。\nEN:Move object item to active map.
        */
        void MoveToActive(ObjectItem *item)
        {
            idle_object_map.erase(item->id);
            active_object_map[item->id] = item;

            OnActive(item);
        }

        /**
        * @brief CN:释放对象项引用。\nEN:Release object item reference.
        */
        void Release(ObjectItem *item)
        {
            --item->ref_count;

            if (item->ref_count <= 0)
                MoveToIdle(item);
        }

    public:

        virtual ~ActiveObjectManager()
        {
            Clear(active_object_map);
            Clear(idle_object_map);
        }

        /**
        * @brief CN:获取活跃对象数量。\nEN:Get active object count.
        */
        int GetActiveCount() const
        {
            return static_cast<int>(active_object_map.size());
        }

        /**
        * @brief CN:获取闲置对象数量。\nEN:Get idle object count.
        */
        int GetIdleCount() const
        {
            return static_cast<int>(idle_object_map.size());
        }

        /**
        * @brief CN:获取总对象数量。\nEN:Get total object count.
        */
        int GetTotalCount() const
        {
            return GetActiveCount() + GetIdleCount();
        }

        /**
        * @brief CN:判断对象是否活跃。\nEN:Check if object is active.
        */
        bool IsActive(const ID &id) const
        {
            return active_object_map.contains(id);
        }

        /**
        * @brief CN:判断对象是否闲置。\nEN:Check if object is idle.
        */
        bool IsIdle(const ID &id) const
        {
            return idle_object_map.contains(id);
        }

        /**
        * @brief CN:清理所有活跃对象。\nEN:Clear all active objects.
        */
        void ClearActive()
        {
            Clear(active_object_map);
        }

        /**
        * @brief CN:清理所有闲置对象。\nEN:Clear all idle objects.
        */
        void ClearIdle()
        {
            Clear(idle_object_map);
        }

    public:

        /**
        * @brief CN:对象引用封装类。\nEN:Object reference wrapper class.
        */
        class ObjectRef
        {
            ActiveObjectManager<ID, T> *manager;

            ObjectItem *item;

        private:

            friend class ActiveObjectManager<ID, T>;

            ObjectRef(ActiveObjectManager<ID, T> *m, ObjectItem *it) : manager(m), item(it)
            {
                if (item)
                    ++item->ref_count;
            }

        public:

            /**
            * @brief CN:拷贝构造。\nEN:Copy constructor.
            */
            ObjectRef(const ObjectRef &other) : manager(other.manager), item(other.item)
            {
                if (item)
                    ++item->ref_count;
            }

            /**
            * @brief CN:拷贝赋值。\nEN:Copy assignment.
            */
            ObjectRef &operator=(const ObjectRef &other)
            {
                if (this == &other)
                    return *this;

                // release current
                if (item && manager)
                    manager->Release(item);

                manager = other.manager;
                item = other.item;

                if (item)
                    ++item->ref_count;

                return *this;
            }

            /**
            * @brief CN:移动构造。\nEN:Move constructor.
            */
            ObjectRef(ObjectRef &&other) noexcept : manager(other.manager), item(other.item)
            {
                other.manager = nullptr;
                other.item = nullptr;
            }

            /**
            * @brief CN:移动赋值。\nEN:Move assignment.
            */
            ObjectRef &operator=(ObjectRef &&other) noexcept
            {
                if (this == &other)
                    return *this;

                if (item && manager)
                    manager->Release(item);

                manager = other.manager;
                item = other.item;

                other.manager = nullptr;
                other.item = nullptr;

                return *this;
            }

            /**
            * @brief CN:析构。\nEN:Destructor.
            */
            ~ObjectRef()
            {
                if (!item || !manager)
                    return;

                manager->Release(item);
            }

            /**
            * @brief CN:判断引用是否有效。\nEN:Check if reference is valid.
            */
            bool IsValid() const
            {
                return item != nullptr;
            }

            /**
            * @brief CN:释放引用。\nEN:Release reference.
            */
            void Release()
            {
                if (!item || !manager)
                    return;

                manager->Release(item);
                item = nullptr;
            }

            /**
            * @brief CN:对象指针访问。\nEN:Object pointer access.
            */
            T *operator ->()
            {
                if (!item || item->ref_count <= 0)
                    return nullptr;

                return item->object;
            }

            T *operator ->() const
            {
                if (!item || item->ref_count <= 0)
                    return nullptr;

                return item->object;
            }

        };

        /**
        * @brief CN:获取或创建对象引用。\nEN:Get or create object reference.
        */
        template<typename... ARGS>
        ObjectRef GetOrCreate(const ID &id, ARGS...args)
        {
            auto it = active_object_map.find(id);

            if (it != active_object_map.end())
                return ObjectRef(this, it->second);

            it = idle_object_map.find(id);

            if (it != idle_object_map.end())
            {
                MoveToActive(it->second);
                return ObjectRef(this, it->second);
            }

            ObjectItem *item = CreateObject(id, args...);

            active_object_map[id] = item;

            return ObjectRef(this, item);
        }

        /**
        * @brief CN:获取对象引用。\nEN:Get object reference.
        */
        ObjectRef Get(const ID &id)
        {
            auto it = active_object_map.find(id);

            if (it != active_object_map.end())
                return ObjectRef(this, it->second);

            it = idle_object_map.find(id);

            if (it != idle_object_map.end())
            {
                MoveToActive(it->second);
                return ObjectRef(this, it->second);
            }

            return ObjectRef(this, nullptr);
        }
    };
} // namespace hgl
