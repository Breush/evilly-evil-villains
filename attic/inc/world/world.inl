//----------------------------//
//----- Coordinate tools -----//

inline uint32 World::coo(uint32 x, uint32 y)
{
    return (y << WORLD_SIZE_LOG_X) | x;
}

inline uint32 World::coo(const sf::Vector2u32& pos)
{
    return (pos.y << WORLD_SIZE_LOG_X) | pos.x;
}

inline uint32 World::cooX(uint32 coord)
{
    return (coord & WORLD_SIZE_X_MONE);
}

inline uint32 World::cooY(uint32 coord)
{
    return (coord >> WORLD_SIZE_LOG_X);
}

//-----------------------------//
//----- Arithmetics tools -----//

inline bool World::subableY(uint32 posY, uint32 y)
{
    return posY >= y;
}

inline bool World::addableY(uint32 posY, uint32 y)
{
    return (posY + y) < WORLD_SIZE_Y;
}

inline sf::Vector2u32 World::subX(sf::Vector2u32& pos, uint32 x)
{
    x = WORLD_SIZE_X - x;
    return addX(pos, x);
}

inline sf::Vector2u32 World::subY(sf::Vector2u32& pos, uint32 y)
{
    return sf::Vector2u32(pos.x, pos.y - y);
}

inline sf::Vector2u32 World::addX(sf::Vector2u32& pos, uint32 x)
{
    return sf::Vector2u32((pos.x + x) % WORLD_SIZE_X, pos.y);
}

inline sf::Vector2u32 World::addY(sf::Vector2u32& pos, uint32 y)
{
    return sf::Vector2u32(pos.x, pos.y + y);
}

//-----------------------//
//----- Environment -----//

inline Block& World::block(sf::Vector2u32& pos)
{
    return m_blocks[coo(pos)];
}
