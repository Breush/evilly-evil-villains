#include "nui/tooltip.hpp"

#include "core/application.hpp"
#include "tools/vector.hpp"
#include "tools/string.hpp"

using namespace nui;

Tooltip::Tooltip()
{
    // Corners
    addPart(&m_topLeft);
    addPart(&m_topRight);
    addPart(&m_bottomLeft);
    addPart(&m_bottomRight);

    const auto& topLeftTexture = Application::context().textures.get("nui/tooltip/top_left");

    m_topLeft.setTexture(topLeftTexture);
    m_topRight.setTexture(topLeftTexture);
    m_bottomLeft.setTexture(topLeftTexture);
    m_bottomRight.setTexture(topLeftTexture);

    m_topRight.setScale(-1.f, 1.f);
    m_bottomLeft.setScale(1.f, -1.f);
    m_bottomRight.setScale(-1.f, -1.f);

    m_cornerSize = sf::v2f(topLeftTexture.getSize());

    // Borders
    addPart(&m_top);
    addPart(&m_left);
    addPart(&m_right);
    addPart(&m_bottom);

    const auto& topTexture = Application::context().textures.get("nui/tooltip/top");
    const auto& leftTexture = Application::context().textures.get("nui/tooltip/left");

    m_top.setTexture(&topTexture);
    m_left.setTexture(&leftTexture);
    m_right.setTexture(&leftTexture);
    m_bottom.setTexture(&topTexture);

    m_right.setScale(-1.f, 1.f);
    m_bottom.setScale(1.f, -1.f);

    // Fill
    addPart(&m_fill);

    const auto& fillTexture = Application::context().textures.get("nui/tooltip/fill");

    m_fill.setTexture(&fillTexture);

    // Text
    addPart(&m_text);
    m_text.setFont(Application::context().fonts.get("nui"));
    m_text.setFillColor({210, 210, 210}); // Light grey
}

//-------------------//
//----- Routine -----//

void Tooltip::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_hPadding = cNUI.hPadding;
    m_vPadding = cNUI.vPadding;

    m_text.setCharacterSize(0.8f * cNUI.fontSize);

    updateSize();
}

void Tooltip::updateSize()
{
    auto textSize = boundsSize(m_text);

    sf::Vector2f newSize;
    newSize.x = textSize.x + 2.f * m_hPadding;
    newSize.y = textSize.y + 2.f * m_vPadding;

    setSize(newSize);
    refresh();
}

//-------------------//
//----- Control -----//

void Tooltip::setText(const std::wstring& text)
{
    m_text.setString(text);
    updateSize();
}

//---------------//
//----- ICU -----//

void Tooltip::refresh()
{
    // Frame
    m_topLeft.setPosition(0.f, 0.f);
    m_topRight.setPosition(size().x, 0.f);
    m_bottomLeft.setPosition(0.f, size().y);
    m_bottomRight.setPosition(size().x, size().y);

    float fillWidth = size().x - 2.f * m_cornerSize.x;
    float fillHeight = size().y - 2.f * m_cornerSize.y;

    m_fill.setPosition(m_cornerSize.x, m_cornerSize.y);
    m_fill.setSize({fillWidth, fillHeight});

    m_top.setPosition(m_cornerSize.x, 0.f);
    m_left.setPosition(0.f, m_cornerSize.y);
    m_right.setPosition(size().x, m_cornerSize.y);
    m_bottom.setPosition(m_cornerSize.x, size().y);

    m_top.setSize({fillWidth, m_cornerSize.y});
    m_left.setSize({m_cornerSize.x, fillHeight});
    m_right.setSize({m_cornerSize.x, fillHeight});
    m_bottom.setSize({fillWidth, m_cornerSize.y});

    // Text
    m_text.setPosition({m_hPadding, m_vPadding});
}
