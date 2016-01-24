#pragma once

#include "tools/platform-fixes.hpp"
#include "tools/memorypool.hpp"
#include "tools/debug.hpp"
#include "tools/int.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>

#include <vector>

namespace context
{
    //! Components manager.
    //! Holds all components pool, and can update them.
    //! Registration and deletion of Component types should
    //! be handled manually.

    class Componenter final : private sf::NonCopyable
    {
    public:

        //! Default constructor.
        Componenter() = default;

        //! Default destructor.
        ~Componenter() = default;

        //----------------//
        //! @name Routine
        //! @{

        //! Update all the components in one pool, in a cache-friendly way.
        template <class Component_t>
        void update(const sf::Time& dt)
        {
            uint index = poolIndex<Component_t>();
            massert(index != -1u, "This component type has not been registered. No pool available.");

            for (auto pComponent : m_data[index].components)
                reinterpret_cast<Component_t*>(pComponent)->update(dt);
        }

        //! @}

        //-------------------//
        //! @name Components
        //! @{

        //! Create the specified component.
        template <class Component_t, class... Args>
        Component_t& newComponent(Args&&... args)
        {
            uint index = poolIndex<Component_t>();
            massert(index != -1u, "This component type has not been registered. No pool available.");
            auto pool = reinterpret_cast<MemoryPool<Component_t, sizeof(Component_t) * 4096u>*>(m_data[index].pool);

            auto pComponent = pool->newElement(std::forward<Args>(args)...);
            m_data[index].components.emplace_back(pComponent);
            return *pComponent;
        }

        //! Delete the specified component.
        template <class Component_t>
        void deleteComponent(Component_t* pComponent)
        {
            uint index = poolIndex<Component_t>();
            massert(index != -1u, "This component type has not been registered. No pool available.");
            auto pool = reinterpret_cast<MemoryPool<Component_t, sizeof(Component_t) * 4096u>*>(m_data[index].pool);

            pool->deleteElement(pComponent);
            std::erase_if(m_data[index].components, [pComponent] (void* component) { return component == pComponent; });
        }

        //! @}

        //--------------------//
        //! @name Registering
        //! @{

        //! Add a new component to the pools list.
        //! Note: We have a fixed limit of 4096 components per pool.
        //! And this is not configurable yet.
        template <class Component_t>
        inline void registerComponentType()
        {
            poolIndex<Component_t>(m_data.size());
            m_data.emplace_back();

            auto& pool = m_data.back().pool;
            pool = new MemoryPool<Component_t, sizeof(Component_t) * 4096u>();
        }

        //! Removes a component from the pools list.
        //! Note: This delete the pool but does not reduce the vector of pools,
        //! therefore, it is advice to just register once at the start of unregister all at the end,
        //! without complexes changes while the application is running.
        template <class Component_t>
        inline void unregisterComponentType()
        {
            uint index = poolIndex<Component_t>();
            massert(index != -1u, "This component type has not been registered. No pool available.");
            delete reinterpret_cast<MemoryPool<Component_t, sizeof(Component_t) * 4096u>*>(m_data[index].pool);
            m_data[index].components.clear();
        }

        //! @}

    protected:

        //--------------//
        //! @name Tools
        //! @{

        //! Get or set the pool index in the vector.
        template<typename T>
        inline constexpr uint poolIndex(uint index = -1u)
        {
            static uint value = index;
            return value;
        }

        //! @}

        //! Both pool and components.
        struct Data
        {
            void* pool;                     //!< The pool. Note: void* are in fact MemoryPool<Component_t>* with different Component_t.
            std::vector<void*> components;  //!< The active components in the pool.
        };

    private:

        std::vector<Data> m_data; //!< All the data.
    };

    extern Componenter componenter;
}
