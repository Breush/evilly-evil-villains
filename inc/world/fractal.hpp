#pragma once

#include "tools/param.hpp"
#include "tools/tools.hpp"

class Block;

class FractalWorld
{
public:
    // Constructor and destructor
    FractalWorld(Block*& m_blocks);
    virtual ~FractalWorld() {}

    // Main function
    void create();
    void erase();

    // Params
    PARAMGS(uint32, m_seed, seed, setSeed)
    PARAMGS(uint32, m_fractions, fractions, setFractions)
    PARAMG(double, m_ySalt, ySalt)
    PARAMG(double, m_ySaltDec, ySaltDec)
    PARAMG(uint32, m_dirtDepth, dirtDepth)

protected:
    void heightMaker(const sf::Vector2u32& start, const sf::Vector2u32& stop, double salt);
    void rockarize();

private:
    Block*& m_blocks;
    uint32 m_rfFraction;
};

