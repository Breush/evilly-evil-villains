#pragma once

#include "tools/int.hpp"

#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Time.hpp>

class VisualDebug
{
public:
    VisualDebug();
    virtual ~VisualDebug() = default;

    // Routine
    void init();
    void update(const sf::Time& dt);
    void draw();

    // Visibility
    void switchVisible();

private:
    bool m_visible;
    uint m_renderedFrames;
    uint m_renderedUpdates;
    float m_time;

    sf::Text m_text;
};
