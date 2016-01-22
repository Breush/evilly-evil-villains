#pragma once

#include "tools/platform-fixes.hpp"

namespace scene
{
    //----------------------//
    //----- Components -----//

    template <class Component_t>
    inline bool Entity::hasComponent() const
    {
        for (auto& component : m_components) {
            auto pComponent = dynamic_cast<Component_t*>(component.get());
            if (pComponent != nullptr)
                return true;
        }
        return false;
    }

    template <class Component_t>
    inline Component_t* Entity::getComponent()
    {
        for (auto& component : m_components) {
            auto pComponent = dynamic_cast<Component_t*>(component.get());
            if (pComponent != nullptr)
                return pComponent;
        }
        return nullptr;
    }

    template <class Component_t>
    inline Component_t& Entity::addComponent()
    {
        auto component = std::make_unique<Component_t>(*this);
        auto pComponent = component.get();
        m_components.emplace_back(std::move(component));
        return *pComponent;
    }
}
