#include "dcb/answerline.hpp"

#include "context/context.hpp"
#include "config/nuiguides.hpp"

using namespace dcb;

AnswerLine::AnswerLine()
{
    // Text
    addPart(&m_text);
    m_text.setFillColor(sf::Color::White);
    m_text.setFont(context::context.fonts.get("core/global/fonts/nui"));
}

//-------------------//
//----- Routine -----//

void AnswerLine::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_text.setCharacterSize(cNUI.fontSize);

    updateSize();
}

void AnswerLine::updateSize()
{
    m_text.fitWidth(m_width);
    auto textSize = boundsSize(m_text);
    setSize({m_width, textSize.y});
}

//------------------//
//----- Events -----//

bool AnswerLine::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f&, const sf::Vector2f&)
{
    returnif (button != sf::Mouse::Left) false;
    returnif (m_clickedCallback == nullptr) true;

    m_clickedCallback();
    return true;
}

bool AnswerLine::handleMouseMoved(const sf::Vector2f&, const sf::Vector2f&)
{
    setPartShader(&m_text, "core/nui/hover/hover");
    return true;
}

void AnswerLine::handleMouseLeft()
{
    resetPartsShader();
}

//-------------------//
//----- Control -----//

void AnswerLine::setString(const std::wstring& string)
{
    m_text.setString(string);
    updateSize();
}

void AnswerLine::fitWidth(float width)
{
    m_width = width;
    updateSize();
}
