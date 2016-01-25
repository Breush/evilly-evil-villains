#include "dungeon/effecter.hpp"

#include "dungeon/inter.hpp"

using namespace dungeon;

Effecter::Effecter()
{
}

void Effecter::init()
{
    addFactory("construct_room", "core/dungeon/effects/construct_room");
}

//-------------------//
//----- Routine -----//

void Effecter::update(const sf::Time& dt)
{
    std::erase_if(m_effects, [] (const std::unique_ptr<Effect>& effect) { return !effect->started(); });
}

//-------------------//
//----- Binding -----//

void Effecter::useInter(Inter& inter)
{
    m_inter = &inter;
}

//---------------------------//
//----- Effects control -----//

void Effecter::add(const std::string& key, const RoomCoords& coords)
{
    m_effects.emplace_back(std::move(m_factories[key]()));

    auto& effect = *m_effects.back();
    effect.setLocalPosition(m_inter->positionFromRoomCoords(coords) + 0.5f * m_inter->tileSize());
    effect.setLocalScale(m_inter->roomScale());
    effect.setDepth(-50.f);
    m_inter->attachChild(effect);
}

//---------------------//
//----- Factories -----//

void Effecter::addFactory(const std::string& key, const std::string& animationID)
{
    m_factories[key] = [animationID] () {
        auto effect = std::make_unique<Effect>();
        effect->load(animationID);
        return std::move(effect);
    };
}
