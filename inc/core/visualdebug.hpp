#pragma once

#include "tools/int.hpp"

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/NonCopyable.hpp>

// Forward declarations

namespace config
{
    class NUIGuides;
    class WindowInfo;
}

//! A state-stack-independent class to print debug information.
/*!
 *  The main goal is the print FPS.
 *  If game context information is available,
 *  we might also print it.
 */

class VisualDebug final : public sf::Drawable, private sf::NonCopyable
{
public:

    //! Default constructor.
    VisualDebug() = default;

    //! Once every resource is loaded, call this.
    void init();

    //----------------//
    //! @name Routine
    //! @{

    //! Used to recompute time-dependent information, such as FPS.
    void update(const sf::Time& dt);

    //! Used to precisely inform how much time the whole logic update took.
    void setLogicTickTime(const sf::Time& t);

    //! Basic drawing to the main window.
    void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

    //! Reset the views to current screen status.
    void refreshWindow(const config::WindowInfo& cWindow);

    //! Refresh states' NUI appearance.
    void refreshNUI(const config::NUIGuides& cNUI);

    //! @}

    //--------------------//
    //! @name Interaction
    //! @{

    //! Switch debug information on and off.
    void switchVisible();

    //! Refresh the game time factor displayed.
    void setDisplayedTimeFactor(float timeFactor) { m_timeFactor = timeFactor; }

    //! @}

protected:

    //--------------------------------//
    //! @name Internal change updates
    //! @{

    void updateBackgroundSize();

    //! @}

private:

    //! VisualDebug owns its own view.
    sf::View m_view;

    //! The rendered text.
    sf::Text m_text;

    //! The background.
    sf::RectangleShape m_background;

    bool m_visible = false;         //!< Is text visible?

    // FPS
    uint m_renderedFrames = 0u;     //!< Number of calls to draw().
    uint m_renderedUpdates = 0u;    //!< Number of calls to update().
    float m_time = 0.f;             //!< Timer to refresh FPS information.

    // Time factor
    float m_timeFactor = 1.f;       //!< The estimated time factor.
    int64 m_logicTickTimeSum = 0;   //!< Cummulated time for update().
};

