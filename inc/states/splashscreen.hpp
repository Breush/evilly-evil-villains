#pragma once

#include "states/state.hpp"
#include "scml/interface.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <list>

class SplashScreenState : public State
{
public:
    SplashScreenState(StateStack& stack);
    virtual ~SplashScreenState();

    virtual void draw();
    virtual bool update(sf::Time dt);
    virtual bool handleEvent(const sf::Event& event);


private:
    sf::Sprite m_bgSprite;
    sf::Shader* m_bgShader;

    // TODO Animation handler
    SCML::Data m_data;
    SCML::FileSystem m_fs;
    std::list<SCML::Entity*> m_entities;
};
