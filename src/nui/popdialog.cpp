#include "nui/popdialog.hpp"

#include "core/application.hpp"
#include "tools/vector.hpp"
#include "tools/platform-fixes.hpp" // make_unique
#include "tools/tools.hpp"

using namespace nui;

PopDialog::PopDialog()
{
    setDepth(-1.f);
    setVisible(false);

    // Backgrounds
    addPart(&m_background);
    attachChild(m_boxBackground);
    m_background.setFillColor({0u, 0u, 0u, 100u});
    m_boxBackground.setFillColor({50u, 50u, 50u, 200u});

    // Texts
    attachChild(m_mainStacker);
    m_mainStacker.centerOrigin();
    m_mainStacker.setRelativePosition({0.5f, 0.5f});
    m_mainStacker.stackBack(m_title);
    m_mainStacker.stackBack(m_message);
    m_title.setPrestyle(sfe::Label::Prestyle::NUI_TITLE);
    m_message.setPrestyle(sfe::Label::Prestyle::NUI);

    // Buttons
    m_mainStacker.stackBack(m_buttonsStacker, nui::Align::OPPOSITE);
}

PopDialog::~PopDialog()
{
    reset();
}

//-------------------//
//----- Routine -----//

void PopDialog::onSizeChanges()
{
    m_background.setSize(size());
}

void PopDialog::onChildSizeChanges(scene::Entity&)
{
    refreshBoxBackground();
}

void PopDialog::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_message.setCharacterSize(cNUI.fontSize);

    refreshBoxBackground();
}

//------------------//
//----- Events -----//

//-------------------//
//----- Control -----//

void PopDialog::open()
{
    markForVisible(true);
}

void PopDialog::close()
{
    markForVisible(false);
}

void PopDialog::reset()
{
    m_title.setText(L"");
    m_message.setText(L"");
    m_buttonsStacker.unstackAll();
    m_buttons.clear();
}

void PopDialog::setTitle(const std::wstring& title)
{
    m_title.setText(title);
}

void PopDialog::setMessage(const std::wstring& message)
{
    m_message.setText(message);
}

void PopDialog::addButton(std::wstring text, Callback callback)
{
    auto button = std::make_unique<nui::Button>();

    button->setAction(text, callback);
    m_buttonsStacker.stackBack(*button);

    m_buttons.emplace_back(std::move(button));
}

void PopDialog::setExitCallback(Callback callback)
{
    m_exitCallback = callback;
}

//------------------//
//----- Events -----//

void PopDialog::handleMouseButtonPressed(const sf::Mouse::Button, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    returnif (m_exitCallback == nullptr);

    // If inside the message box, the background get this event, and is not here
    m_exitCallback();
}

//-----------------------------------//
//----- Internal change updates -----//

void PopDialog::refreshBoxBackground()
{
    m_boxBackground.setSize(m_mainStacker.size());
    m_boxBackground.setLocalPosition(m_mainStacker.localPosition());
    m_boxBackground.setOrigin(m_mainStacker.getOrigin());
}
