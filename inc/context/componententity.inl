#pragma once

#include "context/componenter.hpp"

#include "tools/platform-fixes.hpp"

namespace context
{
    //----------------------//
    //----- Components -----//

    template <class Component_t>
    inline bool ComponentEntity::hasComponent() const
    {
        auto found = m_components.find(Component_t::id());
        return (found != std::end(m_components));
    }

    template <class Component_t>
    inline Component_t* ComponentEntity::getComponent()
    {
        auto found = m_components.find(Component_t::id());
        return (found != std::end(m_components))? reinterpret_cast<Component_t*>(found->second) : nullptr;
    }

    template <class Component_t, class... Args>
    inline Component_t* ComponentEntity::addComponent(Args&&... args)
    {
        auto& component = componenter.newComponent<Component_t>(std::forward<Args>(args)...);
        m_components[Component_t::id()] = &component;
        return &component;
    }

    template <class Component_t>
    inline void ComponentEntity::removeComponent()
    {
        auto found = m_components.find(Component_t::id());
        if (found == std::end(m_components)) return;
        auto pComponent = reinterpret_cast<Component_t*>(found->second);
        componenter.deleteComponent<Component_t>(pComponent);
        m_components.erase(found);
    }
}
