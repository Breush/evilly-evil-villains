#pragma once

#include "world/def.hpp"
#include "world/block.hpp"
#include "world/identifiers.hpp"

#include "tools/tools.hpp"
#include "tools/param.hpp"

#include "game/game.hpp"
#include "states/state.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System.hpp>

class Block;

/** World
 *   Lines are stored continually in memory
 */
class World : public sf::Drawable, private sf::NonCopyable
{
public:
    // Constructors
    World(State::Context inContext, Game::Context& inGameContext);
    virtual ~World();

    // Routine
    void init();
    void update(sf::Time dt);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    // Coordinate tools
    static uint32 coo(uint32 x, uint32 y);
    static uint32 coo(const sf::Vector2u32& pos);
    static uint32 cooX(uint32 coord);
    static uint32 cooY(uint32 coord);

    static sf::Vector2u32 subX(sf::Vector2u32& pos, uint32 x);
    static sf::Vector2u32 subY(sf::Vector2u32& pos, uint32 y);
    static sf::Vector2u32 addX(sf::Vector2u32& pos, uint32 x);
    static sf::Vector2u32 addY(sf::Vector2u32& pos, uint32 y);

    static bool subableY(uint32 posY, uint32 y);
    static bool addableY(uint32 posY, uint32 y);

    // Environment
    static Block& block(sf::Vector2u32& pos);
    static bool touch(sf::Vector2u32& block, Blocks::ID neighbourg, bool diagonally);

    // Params
    PARAMG(State::Context, m_context, context)
    PARAMG(sf::Vector2u32, m_size, size)
    PARAMG(sf::Vector2u32, m_viewSize, viewSize)

protected:
    // Init
    // TODO void initFromFile(std::string file);
    // TODO void initFromScratch(uint16 seed);

    // Create
    void createFlat();
    void createFractal();

private:
    static Block* m_blocks;
    Game::Context& m_gameContext;
};

#include "world.inl"
