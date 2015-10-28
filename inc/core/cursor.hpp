#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include <string>

// Forward declarations

namespace config
{
    class WindowInfo;
}

//! A state-stack-independent class to print debug information.

class Cursor final : public sf::Drawable, private sf::NonCopyable
{
public:

    //! Default constructor.
    Cursor() = default;

    //! Once every resource is loaded, call this.
    void init();

    //----------------//
    //! @name Routine
    //! @{

    //! Basic drawing to the main window.
    void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

    //! Used to update sprite position to the mouse one.
    void update(const sf::Time& dt);

    //! Reset the views to current screen status.
    void refreshWindow(const config::WindowInfo& cWindow);

    //! Handle event.
    void handleEvent(const sf::Event& event);

    //! @}

    //----------------//
    //! @name Control
    //! @{

    //! Override for the texture.
    void setMode(const std::string& mode);

    //! @}

private:

    sf::View m_view;            //!< Cursor owns its own view.
    sf::RectangleShape m_shape; //!< The cursor indeed.
};

