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
    return (e.type == sf::Event::MouseWheelMoved)
           || (e.type == sf::Event::MouseButtonPressed)
           || (e.type == sf::Event::MouseButtonReleased)
           || (e.type == sf::Event::MouseMoved);
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

inline sf::Vector2i mouseButtonToVector2i(const sf::Event::MouseButtonEvent& mouseButtonEvent)
{
    return sf::Vector2i(mouseButtonEvent.x, mouseButtonEvent.y);
}
