#pragma once

#include <SFML/Window/Event.hpp>

bool isWindow(const sf::Event& e);
bool isMouse(const sf::Event& e);
bool isKeyboard(const sf::Event& e);
bool isJoystick(const sf::Event& e);

sf::Vector2i mousePosition(const sf::Event& event);

#include "event.inl"

