#pragma once

#include "tools/param.hpp"

#include <SFML/System/Vector2.hpp>

class Camera
{
public:
    Camera(const sf::Vector2f& viewSize);
    ~Camera() {}

    // Validating
    void validate();

    // Positioning
    void move(float x, float y);
    void center(const float x, const float y);
    void center(const sf::Vector2f& pos);

    // Camera position is given in number of blocks
    PARAMG(sf::Vector2f, m_position, position);
    PARAMG(sf::Vector2f, m_viewBlockSize, viewBlockSize);
};
