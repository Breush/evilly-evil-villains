#pragma once

#include "tools/int.hpp"

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/NonCopyable.hpp>

//! A state-stack-independent class to print debug information.
/*!
 *  The main goal is the print FPS.
 *  If game context information is available,
 *  we might also print it.
 */

class VisualDebug final : sf::NonCopyable
{
public:

    //! Default constructor.
    VisualDebug() = default;

    //! Once every resource is loaded, call this.
    void init();

    //-------------------//
    //----- Routine -----//

    //! Used to recompute time-dependent information, such as FPS.
    void update(const sf::Time& dt);

    //! Basic drawing to the main window.
    void draw();

    //! Reset the views to current screen status.
    void refreshDisplay();

    //----------------------//
    //----- Visibility -----//

    //! Switch debug information on and off.
    void switchVisible();

private:

    //! VisualDebug owns its own view
    sf::View m_view;

    //! The rendered text.
    sf::Text m_text;

    bool m_visible = false;         //!< Is text visible?
    uint m_renderedFrames = 0u;     //!< Number of calls to draw()
    uint m_renderedUpdates = 0u;    //!< Number of calls to update()
    float m_time = 0.f;             //!< Timer to refresh FPS information
};

