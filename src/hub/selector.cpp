#include "hub/selector.hpp"

#include "core/gettext.hpp"
#include "core/application.hpp"
#include "tools/vector.hpp"
#include "tools/string.hpp"
#include "tools/event.hpp"
#include "tools/tools.hpp"

using namespace hub;

Selector::Selector()
{
    // Label
    attachChild(m_selectedLabel);
    m_selectedLabel.setDepth(-50.f);
    m_selectedLabel.setPrestyle(scene::RichLabel::Prestyle::NUI);
    m_selectedLabel.setRelativePosition({0.5f, 0.95f});
    m_selectedLabel.setRelativeOrigin({0.f, 1.f});
}

void Selector::init()
{
    // Sprite
    attachChild(m_sprite);
    m_sprite.load("hub/hub");
    m_sprite.setRelativePosition({0.5f, 0.5f});
}

//------------------//
//----- Events -----//

bool Selector::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    refreshHitboxSelected(mousePos);
    return true;
}

//---------------//
//----- ICU -----//

void Selector::refreshHitboxSelected(const sf::Vector2f& relPos)
{
    auto findAndSelect = [this, &relPos] (const std::string& boxName)
    {
        auto boxBounds = m_sprite.findBox(boxName + "_hitbox") + m_sprite.getPosition();
        returnif (!boxBounds.contains(relPos)) false;
        returnif (m_selectedBox == boxName) true;
        m_sprite.select(toWString(boxName) + L"_selected");
        m_selectedLabel.setText(_(boxName.c_str()));
        m_selectedBox = boxName;
        return true;
    };

    returnif (findAndSelect("sign"));
    returnif (findAndSelect("inn"));
    returnif (findAndSelect("bank"));
    returnif (findAndSelect("market"));
    // TODO Disable as long as I don't know what to do with
    // returnif (findAndSelect("marketing_office"));

    // If none found, select default animation
    returnif (m_selectedBox == "none");
    m_selectedLabel.setText(L"");
    m_sprite.select(L"none");
    m_selectedBox = "none";
}
