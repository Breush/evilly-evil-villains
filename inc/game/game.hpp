#pragma once

#include "tools/param.hpp"
#include "game/camera.hpp"
#include "states/state.hpp"

#include <SFML/System.hpp>

class Player;
class World;

class Game : private sf::NonCopyable
{
public:
    // Game context structure
    struct Context {
        Context(Camera& inCamera);

        Camera* camera;
        Player* player; // Added by unique instance
        World*  world;  // Added by unique instance
    };

    // Constructors
    Game(State::Context inContext);
    virtual ~Game();

    // Routine
    void init();
    void update(sf::Time dt);
    bool handleEvent(const sf::Event& event);

    // Params
    PARAMG(State::Context, m_context, context)
    PARAMG(sf::Vector2f, m_viewSize, viewSize)
    PARAMG(Camera, m_camera, camera)
    PARAMG(Context, m_gameContext, gameContext)
};
