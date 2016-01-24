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
        for (auto& component : m_components) {
            auto pComponent = dynamic_cast<Component_t*>(component);
            if (pComponent != nullptr)
                return true;
        }
        return false;
    }

    template <class Component_t>
    inline Component_t* ComponentEntity::getComponent()
    {
        for (auto& component : m_components) {
            auto pComponent = dynamic_cast<Component_t*>(component);
            if (pComponent != nullptr)
                return pComponent;
        }
        return nullptr;
    }

    template <class Component_t, class... Args>
    inline Component_t* ComponentEntity::addComponent(Args&&... args)
    {
        auto& component = componenter.newComponent<Component_t>(std::forward<Args>(args)...);
        m_components.emplace_back(&component);
        return &component;
    }

    template <class Component_t>
    inline void ComponentEntity::removeComponent()
    {
        auto pComponent = getComponent<Component_t>();
        if (pComponent == nullptr) return;
        componenter.deleteComponent<Component_t>(pComponent);
        std::erase_if(m_components, [pComponent] (Component* component) { return component == pComponent; });
    }
}
