#include "game/camera.hpp"

#include "world/def.hpp"

Camera::Camera(const sf::Vector2f& viewSize)
{
    // Initialisation
    m_viewBlockSize.x = viewSize.x / BLOCK_PIXEL_SIZE_X;
    m_viewBlockSize.y = viewSize.y / BLOCK_PIXEL_SIZE_X;
}

//----------------------//
//----- Validating -----//

void Camera::validate()
{
    // Loop around x axis
    if (position().x < 0.f)
        m_position.x += WORLD_SIZE_X;
    else if (position().x >= WORLD_SIZE_X)
        m_position.x -= WORLD_SIZE_X;

    // Stop on y axis limits
    if (position().y < 0.f)
        position().y = 0.f;
    else if (position().y + viewBlockSize().y > WORLD_SIZE_Y)
        m_position.y = WORLD_SIZE_Y - viewBlockSize().y;
}

//-----------------------//
//----- Positioning -----//

void Camera::move(float x, float y)
{
    m_position.x += x;
    m_position.y += y;

    validate();
}

void Camera::center(const float x, const float y)
{
    m_position.x = x - viewBlockSize().x / 2.f;
    m_position.y = y - viewBlockSize().y / 2.f;

    validate();
}

void Camera::center(const sf::Vector2f& pos)
{
    center(pos.x, pos.y);
}
