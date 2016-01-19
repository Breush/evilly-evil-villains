#include "hub/selector.hpp"

#include "core/gettext.hpp"
#include "core/application.hpp"
#include "states/hub/market.hpp"
#include "states/hub/inn.hpp"
#include "tools/vector.hpp"
#include "tools/string.hpp"
#include "tools/event.hpp"
#include "tools/tools.hpp"

using namespace hub;

Selector::Selector(states::State& state, dungeon::Data& data)
    : m_state(state)
    , m_data(data)
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

bool Selector::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    returnif (button == sf::Mouse::Middle) false;
    returnif (m_selectedBox == "none") false;

    if (m_selectedBox == "sign") {
        m_state.stackPop();
    }
    else if (m_selectedBox == "inn") {
        m_state.stackDynamicPush<states::HubInn>(m_data);
    }
    else if (m_selectedBox == "bank") {
    }
    else if (m_selectedBox == "market") {
        m_state.stackDynamicPush<states::HubMarket>(m_data);
    }
    else if (m_selectedBox == "marketing_office") {
    }

    return true;
}

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
        auto boxBounds = m_sprite.findBox(boxName + "_hitbox") + m_sprite.localPosition() - m_sprite.getOrigin();
        returnif (!boxBounds.contains(relPos)) false;
        returnif (m_selectedBox == boxName) true;
        m_sprite.select(boxName + "_selected");
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
    m_sprite.select("none");
    m_selectedBox = "none";
}
