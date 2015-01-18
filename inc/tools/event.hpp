#pragma once

#include <SFML/Window/Event.hpp>

bool isWindow(const sf::Event& e);
bool isMouse(const sf::Event& e);
bool isKeyboard(const sf::Event& e);
bool isJoystick(const sf::Event& e);

sf::Vector2i mouseButtonToVector2i(const sf::Event::MouseButtonEvent& mouseButtonEvent);

#include "event.inl"

