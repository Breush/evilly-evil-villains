inline bool isWindow(const sf::Event& e)
{
    return (e.type == sf::Event::Closed)
           || (e.type == sf::Event::Resized)
           || (e.type == sf::Event::LostFocus)
           || (e.type == sf::Event::GainedFocus)
           || (e.type == sf::Event::MouseEntered)
           || (e.type == sf::Event::MouseLeft);
}

inline bool isMouse(const sf::Event& e)
{
    return (e.type == sf::Event::MouseMoved)
           || (e.type == sf::Event::MouseButtonPressed)
           || (e.type == sf::Event::MouseButtonReleased)
           || (e.type == sf::Event::MouseWheelMoved);
}

inline bool isKeyboard(const sf::Event& e)
{
    return (e.type == sf::Event::TextEntered)
           || (e.type == sf::Event::KeyPressed)
           || (e.type == sf::Event::KeyReleased);
}

inline bool isJoystick(const sf::Event& e)
{
    return (e.type == sf::Event::JoystickButtonPressed)
           || (e.type == sf::Event::JoystickButtonReleased)
           || (e.type == sf::Event::JoystickMoved)
           || (e.type == sf::Event::JoystickConnected)
           || (e.type == sf::Event::JoystickDisconnected);
}

inline sf::Vector2i mousePosition(const sf::Event& event)
{
    if (event.type == sf::Event::MouseMoved)
        return sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
    else if (event.type == sf::Event::MouseButtonPressed
             || event.type == sf::Event::MouseButtonReleased)
        return sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
    else if (event.type == sf::Event::MouseWheelMoved)
        return sf::Vector2i(event.mouseWheel.x, event.mouseWheel.y);

    return sf::Vector2i();
}
