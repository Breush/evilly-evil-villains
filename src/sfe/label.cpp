#include "sfe/label.hpp"

#include "core/application.hpp"

using namespace sfe;

Label::Label()
{
    addPart(&m_text);
}

void Label::updateSize()
{
    const auto& bounds = m_text.getLocalBounds();
    setSize({bounds.left + bounds.width, bounds.top + bounds.height});
}

void Label::setFont(Fonts::ID fontID)
{
    m_text.setFont(Application::context().fonts.get(fontID));
    updateSize();
}

void Label::setCharacterSize(uint characterSize)
{
    m_text.setCharacterSize(characterSize);
    updateSize();
}

void Label::setText(const std::wstring& text)
{
    m_text.setString(text);
    updateSize();
}
