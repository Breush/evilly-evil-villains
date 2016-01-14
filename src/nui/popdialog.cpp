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
    m_background.setFillColor({0u, 0u, 0u, 100u});

    // Box
    attachChild(m_frame);
    m_frame.setContent(m_mainStacker);
    m_frame.centerOrigin();
    m_frame.setRelativePosition({0.5f, 0.5f});

    // Box content
    m_mainStacker.stackBack(m_message);
    m_mainStacker.stackBack(m_buttonsStacker, nui::Align::OPPOSITE);

    m_message.setPrestyle(scene::Label::Prestyle::NUI);
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

void PopDialog::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_message.setCharacterSize(cNUI.fontSize);
}

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
    m_message.setText(L"");
    m_buttonsStacker.unstackAll();
    m_buttons.clear();
}

void PopDialog::setTitle(const std::wstring& title)
{
    m_frame.setTitle(title);
}

void PopDialog::setMessage(const std::wstring& message)
{
    m_message.setText(message);
}

void PopDialog::addButton(std::wstring text, Callback callback)
{
    auto button = std::make_unique<nui::PushButton>();

    button->set(text, callback);
    m_buttonsStacker.stackBack(*button);

    m_buttons.emplace_back(std::move(button));
}

void PopDialog::setExitCallback(Callback callback)
{
    m_exitCallback = callback;
}

//------------------//
//----- Events -----//

bool PopDialog::handleMouseButtonPressed(const sf::Mouse::Button, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    returnif (m_exitCallback == nullptr) true;

    // If inside the message box, the background get this event, and is not here
    m_exitCallback();
    return true;
}
