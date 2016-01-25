#include "dungeon/inter.hpp"

#include "core/gettext.hpp"
#include "context/context.hpp"
#include "context/worlds.hpp"
#include "context/villains.hpp"
#include "dungeon/managers/heroesmanager.hpp"
#include "tools/debug.hpp"
#include "tools/event.hpp"
#include "tools/tools.hpp"
#include "tools/platform-fixes.hpp" // make_unique

#include <SFML/Graphics/RenderWindow.hpp>
#include <sstream>

using namespace dungeon;

Inter::Inter(nui::ContextMenu& contextMenu)
    : m_commandable(this)
    , m_contextMenu(contextMenu)
{
    // Grid
    m_grid.setVisible(false);
    addPart(&m_grid);

    // Spinbox
    attachChild(m_treasureEditSpinBox);
    m_treasureEditSpinBox.setDepth(-1.f);
    m_treasureEditSpinBox.setVisible(false);
    m_treasureEditSpinBox.setStep(10u);
    m_treasureEditSpinBox.setOnValidateCallback([this]() { m_treasureEditSpinBox.markForVisible(false); });

    // Prediction
    attachChild(m_predictionSprite);
    m_predictionSprite.setDepth(-10.f);
    m_predictionSprite.setStarted(false);
    m_predictionSprite.setVisible(false);

    attachChild(m_predictionLink);
    m_predictionLink.setDepth(-15.f);

    // Outer walls
    addPart(&m_voidBackground);
    addPart(&m_outerWalls[0]);
    addPart(&m_outerWalls[1]);
}

void Inter::init()
{
    m_effecter.init();

    // Outer walls
    m_voidBackground.setTexture(&context::context.textures.get("core/dungeon/inter/void_room"));
    m_outerWalls[0].setTexture(&context::context.textures.get("core/dungeon/inter/outer_wall_west"));
    m_outerWalls[1].setTexture(&context::context.textures.get("core/dungeon/inter/outer_wall_east"));

    // Ref size
    m_refRoomSize = sf::v2f(context::context.textures.get("core/dungeon/inter/inner_wall").getSize());
}

//-------------------//
//----- Routine -----//

void Inter::updateRoutine(const sf::Time& dt)
{
    // Effecter update
    m_effecter.update(dt);

    // Moving rooms
    returnif (m_movingRooms.empty());

    auto timeElapsed = dt.asSeconds();

    // Animate the rooms
    for (auto& movingRoom : m_movingRooms) {
        auto& tile = m_tiles[movingRoom.coords];

        // Time update
        movingRoom.animationTime += timeElapsed;
        auto adaptedTimeElapsed = timeElapsed;
        if (movingRoom.animationTime >= movingRoom.animationDelay)
            adaptedTimeElapsed -= (movingRoom.animationTime - movingRoom.animationDelay);

        // Move all it contains
        const auto offset = movingRoom.velocity * adaptedTimeElapsed;
        for (auto& layer : tile.layers)             layer->localMove(offset);
        for (auto& facility : tile.facilities)      facility->localMove(offset);
        if (tile.trap != nullptr)                   tile.trap->localMove(offset);
        if (tile.totalDoshLabel != nullptr)         tile.totalDoshLabel->localMove(offset);
        if (tile.harvestableDoshLabel != nullptr)   tile.harvestableDoshLabel->localMove(offset);

        // Move moving elements (monsters/heroes)
        for (auto& monster : movingRoom.monsters)   monster->localMove(offset);
        for (auto& hero : movingRoom.heroes)        hero->localMove(offset);

        // The end
        if (movingRoom.animationTime >= movingRoom.animationDelay) {
            // Unlock them all
            tile.movingLocked = false;
            for (auto monster : movingRoom.monsters)    m_data->monstersManager().setLocked(monster, false);
            for (auto hero :    movingRoom.heroes)      m_data->heroesManager().setLocked(hero, false);

            if (movingRoom.onFinishCallback != nullptr) movingRoom.onFinishCallback();
        }
    }

    // Remove all the rooms which finished their animation
    std::erase_if(m_movingRooms, [] (const MovingRoom& movingRoom) { return movingRoom.animationTime >= movingRoom.animationDelay; });
}

void Inter::onSizeChanges()
{
    m_grid.setSize(size());
    refreshOuterWalls();

    returnif (m_data == nullptr);

    // Should only update position/size of elements inside
    // as no object changed inside
    refreshTiles();
}

//------------------//
//----- Events -----//

void Inter::handleGlobalEvent(const sf::Event& event)
{
    // Remove selection if something is clicked anywhere
    if (event.type == sf::Event::MouseButtonPressed)
        deselectTile();

#if DEBUG_GLOBAL > 0
    // Keyboard event
    if (event.type == sf::Event::KeyPressed) {
        // Control rows
        if (event.key.code == sf::Keyboard::Add)
            adaptFloorsCount(1);
        else if (event.key.code == sf::Keyboard::Subtract)
            adaptFloorsCount(-1);

        // Control columns
        else if (event.key.code == sf::Keyboard::Multiply)
            adaptRoomsByFloor(1);
        else if (event.key.code == sf::Keyboard::Divide)
            adaptRoomsByFloor(-1);

        // Dosh control
        else if (event.key.code == sf::Keyboard::Numpad9)
            m_data->doshWallet().add(500);
        else if (event.key.code == sf::Keyboard::Numpad3)
            m_data->doshWallet().sub(500);

        // Soul control
        else if (event.key.code == sf::Keyboard::Numpad8)
            m_data->soulWallet().add(50);
        else if (event.key.code == sf::Keyboard::Numpad2)
            m_data->soulWallet().sub(50);

        // Fame control
        else if (event.key.code == sf::Keyboard::Numpad7)
            m_data->fameWallet().add(50);
        else if (event.key.code == sf::Keyboard::Numpad1)
            m_data->fameWallet().sub(50);
    }

    refreshOuterWalls();
#endif
}

bool Inter::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& relPos, const sf::Vector2f& nuiPos)
{
    returnif (button == sf::Mouse::Middle) false;

    // Selected the tile below
    selectTile(relPos);

    // Harvest the money
    if (button == sf::Mouse::Left)
        harvestTileDosh(m_selectedTile->coords);

    // Remove spinbox interface if any
    m_treasureEditSpinBox.markForVisible(false);

    // Pop the context menu up
    if (button == sf::Mouse::Right)
        showTileContextMenu(m_selectedTile->coords, nuiPos);

    return true;
}

bool Inter::handleMouseMoved(const sf::Vector2f& relPos, const sf::Vector2f&)
{
    auto coords = roomCoordsFromPosition(relPos);
    setHoveredTile(coords);
    return true;
}

void Inter::handleMouseLeft()
{
    resetHoveredTile();
}

void Inter::receive(const context::Event& event)
{
    const auto& devent = *reinterpret_cast<const dungeon::Event*>(&event);
    RoomCoords coords(devent.room.x, devent.room.y);

    if (event.type == "room_destroyed") {
        refreshTile(coords);
        refreshNeighboursLayers(coords);
    }
    else if (event.type == "room_constructed") {
        refreshTile(coords);
        refreshNeighboursLayers(coords);
    }
    else if (event.type == "room_changed") {
        refreshTile(coords);
        refreshNeighboursLayers(coords);
    }
    else if (event.type == "room_hide_changed") {
        refreshTileLayers(coords);
    }
    else if (event.type == "facility_changed") {
        refreshTileFacilities(coords);
        refreshTileDoshLabel(coords);
    }
    else if (event.type == "trap_changed") {
        refreshTileTraps(coords);
    }
    else if (event.type == "harvestable_dosh_changed") {
        refreshTileDoshLabel(coords);
    }
    else if (event.type == "dungeon_structure_changed") {
        refreshFromData();
    }
}

//------------------------//
//----- Dungeon data -----//

void Inter::useData(Data& data)
{
    m_data = &data;
    m_data->heroesManager().useInter(*this);
    m_data->monstersManager().useInter(*this);
    m_data->dynamicsManager().useInter(*this);

    m_effecter.useInter(*this);

    setEmitter(m_data);
    refreshFromData();
}

void Inter::refreshFromData()
{
    returnif (m_data == nullptr);

    const auto& roomsByFloor = m_data->roomsByFloor();
    const auto& floorsCount = m_data->floorsCount();

    // Grid
    m_grid.setRowsColumns(floorsCount, roomsByFloor);

    // Room tiles
    clearTiles();

    for (uint8 floor = 0u; floor < floorsCount; ++floor)
    for (uint8 room = 0u; room < roomsByFloor; ++room) {
        Tile tile;
        tile.coords = {floor, room};
        m_tiles[tile.coords] = std::move(tile);
    }

    // Sets the new size
    refreshSize();
    refreshTiles();
}

//---------------------------//
//----- Tile management -----//

sf::Vector2f Inter::positionFromRoomCoords(const RoomCoords& coords) const
{
    return m_grid.cellPosition(m_data->floorsCount() - coords.x - 1u, coords.y);
}

RoomCoords Inter::roomCoordsFromPosition(const sf::Vector2f& position) const
{
    auto gridCoords = m_grid.coordsFromPosition(position);

    RoomCoords coords;
    coords.x = m_data->floorsCount() - gridCoords.x - 1u;
    coords.y = gridCoords.y;
    return coords;
}

sf::Vector2f Inter::positionFromRelCoords(const RoomRelCoords& relCoords) const
{
    const auto& factor = tileSize();

    sf::Vector2f pos;
    pos.x = relCoords.y * factor.x;
    pos.y = size().y - relCoords.x * factor.y;
    return pos;
}

sf::FloatRect Inter::rectFromRelRect(const RoomRelRect& relRect) const
{
    const auto& factor = tileSize();

    sf::FloatRect rect;
    rect.left = relRect.top * factor.x;
    rect.top = size().y - (relRect.left + relRect.height) * factor.y;
    rect.width = relRect.width * factor.x;
    rect.height = relRect.height * factor.y;
    return rect;
}

RoomRelCoords Inter::relCoordsFromPosition(const sf::Vector2f& position) const
{
    RoomRelCoords relCoords;
    relCoords.x = (size().y - position.y) / tileSize().y;
    relCoords.y = position.x / tileSize().x;
    return relCoords;
}

void Inter::addLayer(const RoomCoords& coords, const std::string& textureID, float depth)
{
    auto& tile = m_tiles[coords];

    auto layer = std::make_unique<scene::RectangleShape>();
    layer->setTexture(textureID);
    layer->setLocalPosition(positionFromRoomCoords(coords));
    layer->setSize(tileSize());
    layer->setDepth(depth);

    tile.layers.emplace_back(std::move(layer));
    attachChild(*tile.layers.back());
}

void Inter::clearLayers(const RoomCoords& coords)
{
    auto& tile = m_tiles.at(coords);
    tile.layers.clear();
}

void Inter::clearTiles()
{
    for (auto& tile : m_tiles)
        clearLayers(tile.second.coords);

    m_tiles.clear();
    m_selectedTile = nullptr;
    m_hoveredTile = nullptr;
}

//-------------------------//
//----- Selected tile -----//

void Inter::selectTile(const sf::Vector2f& pos)
{
    selectTile(roomCoordsFromPosition(pos));
}

void Inter::selectTile(const RoomCoords& coords)
{
    deselectTile();
    m_selectedTile = &m_tiles.at(coords);

    for (auto& layer : m_selectedTile->layers)
        layer->setShader("core/nui/select/select");

    // Animated sprites do not handle shaders right now...
    // so, nothing else to select
}

void Inter::deselectTile()
{
    returnif (m_selectedTile == nullptr);

    for (auto& layer : m_selectedTile->layers)
        layer->setShader("");

    m_selectedTile = nullptr;
}

//------------------------//
//----- Hovered tile -----//

void Inter::setHoveredTile(const RoomCoords& coords)
{
    returnif (coords.x >= m_data->floorsCount());
    returnif (coords.y >= m_data->roomsByFloor());

    auto hoveredTile = &m_tiles.at(coords);
    returnif (m_hoveredTile == hoveredTile);

    resetHoveredTile();
    m_hoveredTile = hoveredTile;

    returnif (m_hoveredTile == m_selectedTile);

    for (auto& layer : m_hoveredTile->layers)
        layer->setShader("core/nui/hover/hover");
}

void Inter::resetHoveredTile()
{
    returnif (m_hoveredTile == nullptr);

    if (m_hoveredTile != m_selectedTile)
        for (auto& layer : m_hoveredTile->layers)
            layer->setShader("");

    m_hoveredTile = nullptr;
}

//------------------------//
//----- Augmented UI -----//

void Inter::showTileContextMenu(const RoomCoords& coords, const sf::Vector2f& nuiPos)
{
    auto& room = m_data->room(coords);
    m_contextMenu.clearChoices();

    // Context title
    std::wstringstream roomName;
    roomName << _("Room") << L' ' << coords.x << L'/' << coords.y;
    m_contextMenu.setTitle(roomName.str());

    // Room does not exists yet
    if (room.state == RoomState::EMPTY) {
        // TODO Have a nice way to display money cost in context menu
        std::wstring text = _("Build room") + L" (-" + toWString(m_data->onConstructRoomCost) + L"d)";
        if (m_data->onConstructRoomCost <= m_data->villain().doshWallet.value())
            m_contextMenu.addChoice(text, [this, coords] () { constructRoom(coords); });
        else
            m_contextMenu.addChoice(text, nullptr);
    }

    // Room does exists
    else {
        // TODO Show really how much we get for destroying (adding all elements inside)
        m_contextMenu.addChoice(_("Destroy room"), [=]() { destroyRoom(coords); });

        // If treasure, modifiy pop-up
        // TODO Let the facility itself edit its treasure content
        for (const auto& facilityInfo : m_data->room(coords).facilities)
            if (facilityInfo.treasure != -1u)
                m_contextMenu.addChoice(_("Edit treasure dosh"), [this, coords] () { showEditTreasureDialog(coords); });
    }

    // Context positions
    m_contextMenu.setLocalPosition(nuiPos);
    m_contextMenu.markForVisible(true);
}

void Inter::showEditTreasureDialog(const RoomCoords& coords)
{
    // Find the reference in dungeon data
    // TODO We can currently only edit one type of treasure...
    // Do something elsewhere and better

    uint* pTreasureDosh = nullptr;
    for (auto& facilityInfo : m_data->room(coords).facilities)
        if (facilityInfo.treasure != -1u)
            pTreasureDosh = &facilityInfo.treasure;
    returnif (pTreasureDosh == nullptr);
    auto& treasureDosh = *pTreasureDosh;

    m_treasureEditSpinBox.entry().giveFocus();
    m_treasureEditSpinBox.setVisible(true);
    m_treasureEditSpinBox.set(treasureDosh);
    m_treasureEditSpinBox.setLocalPosition(positionFromRoomCoords(coords));
    m_treasureEditSpinBox.setMaxLimit(treasureDosh + m_data->villain().doshWallet.value());
    m_treasureEditSpinBox.setOnValueChangeCallback([this, coords, &treasureDosh] (uint32 oldValue, uint32 newValue) {
        // Sub or add (doshWallet will send an event)
        if (newValue >= oldValue) m_data->villain().doshWallet.sub(newValue - oldValue);
        else m_data->villain().doshWallet.add(oldValue - newValue);
        treasureDosh = newValue;

        // Global dosh changed
        m_data->addEvent("facility_changed", coords);
    });
}

//----------------------//
//----- Prediction -----//

void Inter::resetPrediction()
{
    m_predictionID = L"";
    m_predictionSprite.setVisible(false);
}

// TODO These two functions setPredictionXXX can definitely be factorized

void Inter::setPredictionMonster(const sf::Vector2f& relPos, const std::wstring& monsterID)
{
    if (m_predictionID != monsterID) {
        m_predictionID = monsterID;
        m_predictionSprite.load(toString(L"vanilla/monsters/" + monsterID + L"/anim"));
        m_predictionSprite.setVisible(true);
    }

    auto rPosition = relPos / tileSize();
    rPosition.x = std::floor(rPosition.x) + 0.5f;
    rPosition.y = std::floor(rPosition.y) + 0.5f;
    m_predictionSprite.setLocalPosition(rPosition * tileSize());

    if (m_data->addMonsterValid(roomCoordsFromPosition(relPos), monsterID))
        m_predictionSprite.setTiltColor({119u, 192u, 119u, 200u});
    else
        m_predictionSprite.setTiltColor({192u, 119u, 119u, 200u});
}

void Inter::setPredictionFacility(const sf::Vector2f& relPos, const std::wstring& facilityID)
{
    if (m_predictionID != facilityID) {
        m_predictionID = facilityID;
        m_predictionSprite.load(toString(L"vanilla/facilities/" + facilityID + L"/anim"));
        m_predictionSprite.setVisible(true);
    }

    auto rPosition = relPos / tileSize();
    rPosition.x = std::floor(rPosition.x) + 0.5f;
    rPosition.y = std::floor(rPosition.y) + 0.5f;
    m_predictionSprite.setLocalPosition(rPosition * tileSize());

    if (m_data->createRoomFacilityValid(roomCoordsFromPosition(relPos), facilityID))
        m_predictionSprite.setTiltColor({119u, 192u, 119u, 200u});
    else
        m_predictionSprite.setTiltColor({192u, 119u, 119u, 200u});
}

void Inter::resetPredictionLink()
{
    m_predictionLink.setVisible(false);
}

void Inter::setPredictionLink(const RoomCoords& coords, const RoomCoords& linkCoords)
{
    m_predictionLink.setVisible(true);
    auto limitStart = positionFromRoomCoords(coords) + tileSize() / 2.f;
    auto limitEnd   = positionFromRoomCoords(linkCoords) + tileSize() / 2.f;
    m_predictionLink.setLimits(limitStart, limitEnd);
}

//-------------------//
//----- Control -----//

void Inter::setRoomWidth(const float roomWidth)
{
    // Note: We want room to keep the same ratio as original image.
    const float scaleFactor = roomWidth / m_refRoomSize.x;
    m_roomScale = {scaleFactor, scaleFactor};
    m_predictionSprite.setLocalScale(m_roomScale);
    refreshSize();
}

//---------------------//
//----- Structure -----//

void Inter::constructRoom(const RoomCoords& coords, bool free)
{
    returnif (m_data->isRoomConstructed(coords));
    returnif (m_tiles[coords].movingLocked);

    if (!free) returnif (!villain().doshWallet.sub(m_data->onConstructRoomCost));

    m_data->constructRoom(coords);
    m_effecter.add("construct_room", coords);
}

void Inter::destroyRoom(const RoomCoords& coords, bool loss)
{
    returnif (!m_data->isRoomConstructed(coords));
    returnif (m_tiles[coords].movingLocked);

    if (!loss) {
        uint gainedDosh = m_data->onDestroyRoomGain;
        gainedDosh += gainRemoveRoomFacilities(coords);
        gainedDosh += gainRemoveRoomTrap(coords);
        villain().doshWallet.add(gainedDosh);
    }

    m_data->destroyRoom(coords);
}

bool Inter::pushRoom(const RoomCoords& coords, Direction direction, uint animationDelay)
{
    const auto& floorsCount = m_data->floorsCount();
    const auto& roomsByFloor = m_data->roomsByFloor();

    returnif (coords.x >= floorsCount)  false;
    returnif (coords.y >= roomsByFloor) false;

    // We look for the first void in the direction,
    // if none, it's impossible to move the rooms
    auto voidCoords = coords;
    while (m_data->isRoomConstructed(voidCoords)) {
        returnif (m_tiles[voidCoords].movingLocked) false;
        voidCoords = m_data->roomNeighbourCoords(voidCoords, direction);
    }

    returnif (voidCoords.x >= floorsCount)  false;
    returnif (voidCoords.y >= roomsByFloor) false;
    returnif (coords == voidCoords) true;

    // The velocity is the offset to go each second
    float animationDelayS = animationDelay / 1000.f;
    auto directionVector = sf::Vector2f(m_data->roomDirectionVector2i(direction));
    sf::Vector2f offset(directionVector.y * tileSize().x, - directionVector.x * tileSize().y);
    auto velocity = offset / animationDelayS;

    // All right, let's register the rooms to animate
    auto movingCoords = coords;
    while (movingCoords != voidCoords) {
        auto targetCoords = m_data->roomNeighbourCoords(movingCoords, direction);

        // Register it to move
        MovingRoom movingRoom;
        movingRoom.animationDelay = animationDelayS;
        movingRoom.coords = movingCoords;
        movingRoom.velocity = velocity;

        // Find moving elements
        m_data->monstersManager().listRoomMonsters(movingCoords, movingRoom.monsters);
        m_data->heroesManager().listRoomHeroes(movingCoords, movingRoom.heroes);

        // Lock them all
        m_tiles[movingCoords].movingLocked = true;
        for (auto monster : movingRoom.monsters)    m_data->monstersManager().setLocked(monster, true);
        for (auto hero : movingRoom.heroes)         m_data->heroesManager().setLocked(hero, true);

        m_movingRooms.emplace_back(std::move(movingRoom));
        movingCoords = targetCoords;
    }

    // Inform data on the change when moving is over
    m_movingRooms.back().onFinishCallback = [this, coords, direction] { m_data->pushRoom(coords, direction); };

    return true;
}

void Inter::adaptFloorsCount(int relativeValue)
{
    setFloorsCount(m_data->floorsCount() + relativeValue);
}

void Inter::adaptRoomsByFloor(int relativeValue)
{
    setRoomsByFloor(m_data->roomsByFloor() + relativeValue);
}

void Inter::setFloorsCount(uint value)
{
    m_data->setFloorsCount(value);
}

void Inter::setRoomsByFloor(uint value)
{
    m_data->setRoomsByFloor(value);
}

//----------------------//
//----- Facilities -----//

Facility* Inter::findRoomFacility(const RoomCoords& coords, const std::wstring& facilityID)
{
    for (auto& facility : m_tiles[coords].facilities)
        if (facility->edata().type() == facilityID)
            return facility.get();

    // Not found
    return nullptr;
}

bool Inter::hasRoomFacility(const RoomCoords& coords, const std::wstring& facilityID) const
{
    return m_data->hasFacility(coords, facilityID);
}

uint Inter::gainRemoveRoomFacility(const RoomCoords& coords, const std::wstring& facilityID) const
{
    const auto pFacilityInfo = m_data->getFacility(coords, facilityID);
    returnif (pFacilityInfo == nullptr) 0u;
    returnif (pFacilityInfo->isLink) 0u;

    uint gainedDosh = 0u;
    if (pFacilityInfo->treasure != -1u) gainedDosh += pFacilityInfo->treasure;
    gainedDosh += static_cast<uint>(0.80f * pFacilityInfo->common->baseCost.dosh);

    return gainedDosh;
}

uint Inter::gainRemoveRoomFacilities(const RoomCoords& coords) const
{
    returnif (!m_data->isRoomConstructed(coords)) 0u;
    const auto& facilitiesData = m_data->room(coords).facilities;

    uint gainedDosh = 0u;
    for (const auto& facilityData : facilitiesData)
        gainedDosh += gainRemoveRoomFacility(coords, facilityData.data.type());

    return gainedDosh;
}

bool Inter::createRoomFacility(const RoomCoords& coords, const std::wstring& facilityID, bool free)
{
    returnif (m_tiles[coords].movingLocked) false;

    auto cost = facilitiesDB().get(facilityID).baseCost.dosh;
    if (!free) returnif (!villain().doshWallet.required(cost)) false;

    bool created = m_data->createRoomFacility(coords, facilityID);
    returnif (!created) false;

    if (!free) villain().doshWallet.sub(cost);
    return true;
}

bool Inter::createRoomFacilityLinked(const RoomCoords& coords, const std::wstring& facilityID, const RoomCoords& linkCoords, const std::wstring& linkFacilityID, bool free)
{
    auto cost = facilitiesDB().get(facilityID).baseCost.dosh;
    if (!free) returnif (!villain().doshWallet.required(cost)) false;

    bool created = m_data->createRoomFacilityLinked(coords, facilityID, linkCoords, linkFacilityID);
    returnif (!created) false;

    if (!free) villain().doshWallet.sub(cost);
    return true;
}

void Inter::setRoomFacilityLink(const RoomCoords& coords, const std::wstring& facilityID, const RoomCoords& linkCoords)
{
    m_data->setRoomFacilityLink(coords, facilityID, linkCoords);
}

void Inter::setRoomFacilityTreasure(const RoomCoords& coords, const std::wstring& facilityID, uint32 amount)
{
    m_data->setRoomFacilityTreasure(coords, facilityID, amount);
}

void Inter::setRoomFacilityBarrier(const RoomCoords& coords, const std::wstring& facilityID, bool activated)
{
    m_data->setRoomFacilityBarrier(coords, facilityID, activated);
}

void Inter::removeRoomFacility(const RoomCoords& coords, const std::wstring& facilityID, bool loss)
{
    returnif (m_tiles[coords].movingLocked);
    if (!loss) villain().doshWallet.add(gainRemoveRoomFacility(coords, facilityID));
    m_data->removeRoomFacility(coords, facilityID);
}

void Inter::removeRoomFacilities(const RoomCoords& coords, bool loss)
{
    returnif (m_tiles[coords].movingLocked);
    if (!loss) villain().doshWallet.add(gainRemoveRoomFacilities(coords));
    m_data->removeRoomFacilities(coords);
}

//-----------------//
//----- Traps -----//

uint Inter::gainRemoveRoomTrap(const RoomCoords& coords) const
{
    returnif (!m_data->isRoomConstructed(coords)) 0u;
    const auto& trapData = m_data->room(coords).trap.data;
    returnif (!trapData.exists()) 0u;

    // TODO Gain something proportionate to the current resistance?
    // TODO Have a "common" attribute pointing to the DB Info, as in facilities
    const auto& trapInfo = trapsDB().get(trapData.type());
    return static_cast<uint>(trapInfo.repairCost.dosh * trapInfo.resistance.durability);
}

void Inter::setRoomTrap(const RoomCoords& coords, const std::wstring& trapID, bool free)
{
    // TODO Have a isTileModifiable(coords) inside Inter
    returnif (!m_data->isRoomConstructed(coords));
    returnif (m_tiles[coords].movingLocked);
    auto& trapData = m_data->room(coords).trap.data;
    returnif (trapData.exists() && trapData.type() == trapID);

    if (!free) {
        // FIXME We're only using the dosh (no soul, no fame) in the cost...
        const auto& trapInfo = trapsDB().get(trapID);
        const uint createCost = trapInfo.baseCost.dosh;
        const uint removeGain = gainRemoveRoomTrap(coords);
        returnif (!villain().doshWallet.addsub(removeGain, createCost));
    }

    // Note: Setting does remove the previous trap if any
    m_data->setRoomTrap(coords, trapID);
}

void Inter::removeRoomTrap(const RoomCoords& coords, bool loss)
{
    returnif (m_tiles[coords].movingLocked);
    if (!loss) villain().doshWallet.add(gainRemoveRoomTrap(coords));
    m_data->removeRoomTrap(coords);
}

//--------------------//
//----- Monsters -----//

void Inter::moveMonsterFromReserve(const sf::Vector2f& relPos, const std::wstring& monsterID)
{
    m_data->moveMonsterFromReserve(roomCoordsFromPosition(relPos), monsterID);
}

void Inter::addMonsterToReserve(const std::wstring& monsterID)
{
    // Increase countdown
    const auto& monsterData = monstersDB().get(monsterID);
    const auto& hireCountdown = (context::worlds.selected().gamemode == context::Gamemode::RICHMAN)? 0u : monsterData.hireCountdown;

    // Check for cost
    returnif (!m_data->villain().doshWallet.required(monsterData.baseCost.dosh));
    returnif (!m_data->soulWallet().required(monsterData.baseCost.soul));
    returnif (!m_data->fameWallet().required(monsterData.baseCost.fame));

    // Do pay
    // TODO Let some globalWallet manage all resources
    m_data->villain().doshWallet.sub(monsterData.baseCost.dosh);
    m_data->soulWallet().sub(monsterData.baseCost.soul);
    m_data->fameWallet().sub(monsterData.baseCost.fame);

    m_data->addMonsterToReserve(monsterID, hireCountdown);
}

//--------------------//
//----- Dynamics -----//

uint32 Inter::spawnDynamic(const sf::Vector2f& relPos, const std::wstring& dynamicID)
{
    return m_data->dynamicsManager().create(relCoordsFromPosition(relPos), dynamicID);
}

//----------------//
//----- Dosh -----//

void Inter::harvestTileDosh(const RoomCoords& coords)
{
    returnif (m_tiles[coords].movingLocked);
    auto& trap = m_tiles[coords].trap;
    returnif (trap == nullptr);

    auto harvestableDosh = trap->harvestableDosh();
    if (harvestableDosh > 0u)
        m_data->villain().doshWallet.add(trap->harvestDosh());

    refreshTileDoshLabel(coords);
}

void Inter::configureDoshLabel(std::unique_ptr<scene::Label>& doshLabel, const uint dosh, const sf::Color& color)
{
    // Nothing if no dosh
    if (dosh == 0u) {
        doshLabel = nullptr;
        return;
    }

    // Create label if does not exists
    if (doshLabel == nullptr) {
        doshLabel = std::make_unique<scene::Label>();
        doshLabel->setPrestyle(scene::Label::Prestyle::NUI);
        doshLabel->setFillColor(color);
        attachChild(*doshLabel);
    }

    // Re-format string
    std::wstringstream str;
    str << dosh << L'd';
    doshLabel->setText(str.str());
}

//-----------------------------------//
//----- Internal change updates -----//

void Inter::refreshSize()
{
    returnif (m_data == nullptr);

    const auto& roomsByFloor = m_data->roomsByFloor();
    const auto& floorsCount = m_data->floorsCount();
    const auto roomSize = m_roomScale * m_refRoomSize;
    setSize({roomSize.x * roomsByFloor, roomSize.y * floorsCount});
}

void Inter::refreshTiles()
{
    for (auto& tile : m_tiles)
        refreshTile(tile.second.coords);
}

void Inter::refreshTile(const RoomCoords& coords)
{
    returnif (coords.x >= m_data->floorsCount());
    returnif (coords.y >= m_data->roomsByFloor());

    refreshTileDoshLabel(coords);
    refreshTileLayers(coords);
    refreshTileTraps(coords);
    refreshTileFacilities(coords);
}

void Inter::refreshNeighboursLayers(const RoomCoords& coords)
{
    refreshTileLayers(m_data->roomNeighbourCoords(coords, WEST));
    refreshTileLayers(m_data->roomNeighbourCoords(coords, SOUTH));
    refreshTileLayers(m_data->roomNeighbourCoords(coords, EAST));
    refreshTileLayers(m_data->roomNeighbourCoords(coords, NORTH));
}

void Inter::refreshTileDoshLabel(const RoomCoords& coords)
{
    returnif (coords.x >= m_data->floorsCount());
    returnif (coords.y >= m_data->roomsByFloor());

    const auto& room = m_data->room(coords);
    const auto& localPosition = positionFromRoomCoords(coords);
    auto& tile = m_tiles[coords];

    // Remove label if no room
    if (room.state == RoomState::EMPTY) {
        tile.totalDoshLabel = nullptr;
        tile.harvestableDoshLabel = nullptr;
        return;
    }

    // Harvestable dosh
    uint harvestableDosh = 0u;
    if (tile.trap != nullptr)
        harvestableDosh += tile.trap->harvestableDosh();
    configureDoshLabel(tile.harvestableDoshLabel, harvestableDosh, sf::Color::Red);

    // Total dosh
    uint totalDosh = 0u;
    totalDosh += harvestableDosh;
    totalDosh += m_data->roomTreasureDosh(coords);
    configureDoshLabel(tile.totalDoshLabel, totalDosh, sf::Color::White);

    // Re-position
    if (tile.totalDoshLabel != nullptr)
        tile.totalDoshLabel->setLocalPosition(localPosition);

    if (tile.harvestableDoshLabel != nullptr) {
        tile.harvestableDoshLabel->setLocalPosition(localPosition + tileSize());
        tile.harvestableDoshLabel->setRelativeOrigin({1.f, 1.f});
    }
}

void Inter::refreshTileLayers(const RoomCoords& coords)
{
    // TODO Do not refresh is locked because of moving

    returnif (coords.x >= m_data->floorsCount());
    returnif (coords.y >= m_data->roomsByFloor());

    const auto& room = m_data->room(coords);
    const auto state = room.state;

    // Reset
    clearLayers(coords);

    // Room is not constructed
    if (state == RoomState::EMPTY)
    {
        if (m_data->isRoomConstructed(m_data->roomNeighbourCoords(coords, WEST)))
            addLayer(coords, "core/dungeon/inter/void_west_transition", 150.f);
        if (m_data->isRoomConstructed(m_data->roomNeighbourCoords(coords, SOUTH)))
            addLayer(coords, "core/dungeon/inter/void_south_transition", 150.f);
        if (m_data->isRoomConstructed(m_data->roomNeighbourCoords(coords, EAST)))
            addLayer(coords, "core/dungeon/inter/void_east_transition", 150.f);

        return;
    }

    // Room is somehow bugged
    else if (state == RoomState::UNKNOWN) {
        std::cerr << "/!\\ Found a room with unknown state at " << coords << "." << std::endl;
        std::cerr << "If that is a recurrent issue, please report this bug." << std::endl;
        addLayer(coords, "default");
        return;
    }

    // Add room textures if not hidden
    if (!(room.hide & RoomFlag::WALL))  addLayer(coords, "core/dungeon/inter/inner_wall", 100.f);
    if (!(room.hide & RoomFlag::FLOOR)) addLayer(coords, "core/dungeon/inter/floor", 75.f);

    if (!m_data->isRoomConstructed(m_data->roomNeighbourCoords(coords, EAST)))
        addLayer(coords, "core/dungeon/inter/right_wall", 75.f);
}

void Inter::refreshTileFacilities(const RoomCoords& coords)
{
    returnif (coords.x >= m_data->floorsCount());
    returnif (coords.y >= m_data->roomsByFloor());

    auto& roomFacilities = m_data->room(coords).facilities;
    auto& tileFacilities = m_tiles[coords].facilities;

    // Reset
    tileFacilities.clear();

    // Facilities
    for (auto& facilityInfo : roomFacilities) {
        auto facility = std::make_unique<Facility>(coords, facilityInfo.data, *this);
        tileFacilities.emplace_back(std::move(facility));
        attachChild(*tileFacilities.back());
    }

    // Now rebind the data
    // Note: this is delayed so that facilities requiring sibling facility to be present will work fine
    auto facilitiesCount = roomFacilities.size();
    for (uint i = 0u; i < facilitiesCount; ++i)
        tileFacilities[i]->bindFacilityInfo(roomFacilities[i]);
}

void Inter::refreshTileTraps(const RoomCoords& coords)
{
    returnif (coords.x >= m_data->floorsCount());
    returnif (coords.y >= m_data->roomsByFloor());

    auto& room = m_data->room(coords);
    auto& tile = m_tiles[coords];

    // Reset
    tile.trap = nullptr;

    // Room is not constructed with a trap
    returnif (room.state != RoomState::CONSTRUCTED);
    returnif (!room.trap.data.exists());

    // Trap
    tile.trap = std::make_unique<Trap>(coords, room.trap.data, *this);
    attachChild(*tile.trap);
}

void Inter::refreshOuterWalls()
{
    const auto& roomSize = context::context.textures.get("core/dungeon/inter/inner_wall").getSize();

    // Void rooms behind
    const auto& voidTextureSize = context::context.textures.get("core/dungeon/inter/void_room").getSize();
    sf::IntRect voidRect(0, 0, voidTextureSize.x * m_grid.columns(), voidTextureSize.y * m_grid.rows());
    m_voidBackground.setSize(size());
    m_voidBackground.setTextureRect(voidRect);

    // West wall
    const auto& westTextureSize = context::context.textures.get("core/dungeon/inter/outer_wall_west").getSize();
    sf::Vector2f westRealSize(westTextureSize.x * m_grid.cellSize().x / roomSize.x, size().y);
    sf::IntRect westWallsRect(0, 0, westTextureSize.x, westTextureSize.y * m_grid.rows());
    m_outerWalls[0].setSize(westRealSize);
    m_outerWalls[0].setPosition(-westRealSize.x, 0.f);
    m_outerWalls[0].setTextureRect(westWallsRect);

    // East wall
    const auto& eastTextureSize = context::context.textures.get("core/dungeon/inter/outer_wall_east").getSize();
    sf::Vector2f eastRealSize(eastTextureSize.x * m_grid.cellSize().x / roomSize.x, size().y);
    sf::IntRect eastWallsRect(0, 0, eastTextureSize.x, eastTextureSize.y * m_grid.rows());
    m_outerWalls[1].setSize(eastRealSize);
    m_outerWalls[1].setPosition(size().x, 0.f);
    m_outerWalls[1].setTextureRect(eastWallsRect);
}
