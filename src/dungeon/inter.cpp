#include "dungeon/inter.hpp"

#include "core/gettext.hpp"
#include "core/application.hpp"
#include "context/villains.hpp"
#include "context/logger.hpp"
#include "dungeon/heroesmanager.hpp"
#include "tools/debug.hpp"
#include "tools/event.hpp"
#include "tools/tools.hpp"
#include "tools/platform-fixes.hpp" // make_unique

#include <SFML/Graphics/RenderWindow.hpp>
#include <sstream>

using namespace dungeon;

Inter::Inter(nui::ContextMenu& contextMenu, const HeroesManager& heroesManager)
    : m_heroesManager(heroesManager)
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
    m_predictionSprite.setDepth(-1.f);
    m_predictionSprite.setStarted(false);

    // Outer walls
    addPart(&m_outerWalls[0]);
    addPart(&m_outerWalls[1]);
}

void Inter::init()
{
    // Outer walls
    m_outerWalls[0].setTexture(&Application::context().textures.get("dungeon/inter/outer_wall_west"));
    m_outerWalls[1].setTexture(&Application::context().textures.get("dungeon/inter/outer_wall_east"));

    // Ref size
    m_refRoomSize = sf::v2f(Application::context().textures.get("dungeon/inter/inner_wall").getSize());
}

//-------------------//
//----- Routine -----//

void Inter::updateRoutine(const sf::Time&)
{
    // Do delayed refresh
    for (const auto& tileRefresh : m_tileRefreshPending)
        tileRefresh();
    m_tileRefreshPending.clear();
}

void Inter::onSizeChanges()
{
    m_grid.setSize(size());
    refreshOuterWalls();

    returnif (m_data == nullptr);

    // Should only update position/size of elements inside
    // as no object changed inside
    refreshTiles();
    refreshMonsters();
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
            m_data->villain().doshWallet.add(500);
        else if (event.key.code == sf::Keyboard::Numpad3)
            m_data->villain().doshWallet.sub(500);
    }

    refreshOuterWalls();
#endif
}

bool Inter::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    returnif (button == sf::Mouse::Middle) false;

    // Selected the tile below
    selectTile(mousePos);

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

bool Inter::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    auto coords = tileFromLocalPosition(mousePos);
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
    sf::Vector2u coords;

    if (event.type == "room_destroyed") {
        coords = {devent.room.x, devent.room.y};
        m_tileRefreshPending.emplace_back([=]() { return refreshTile(coords); });
        m_tileRefreshPending.emplace_back([=]() { return refreshNeighboursLayers(coords); });
        m_tileRefreshPending.emplace_back([=]() { return refreshMonsters(); });
    }
    else if (event.type == "room_constructed") {
        coords = {devent.room.x, devent.room.y};
        m_tileRefreshPending.emplace_back([=]() { return refreshTile(coords); });
        m_tileRefreshPending.emplace_back([=]() { return refreshNeighboursLayers(coords); });
        m_tileRefreshPending.emplace_back([=]() { return refreshMonsters(); });
    }
    else if (event.type == "facility_changed") {
        coords = {devent.facility.room.x, devent.facility.room.y};
        m_tileRefreshPending.emplace_back([=]() { return refreshTileFacilities(coords); });
        m_tileRefreshPending.emplace_back([=]() { return refreshTileDoshLabel(coords); });
    }
    else if (event.type == "trap_changed") {
        coords = {devent.room.x, devent.room.y};
        m_tileRefreshPending.emplace_back([=]() { return refreshTileTraps(coords); });
    }
    else if (event.type == "monster_added") {
        m_tileRefreshPending.emplace_back([=]() { return refreshMonsters(); });
    }
    else if (event.type == "monster_removed") {
        m_tileRefreshPending.emplace_back([=]() { return refreshMonsters(); });
    }
    else if (event.type == "harvestable_dosh_changed") {
        coords = {devent.room.x, devent.room.y};
        m_tileRefreshPending.emplace_back([=]() { return refreshTileDoshLabel(coords); });
    }
}

//-----------------------//
//----- Interpreter -----//

context::Command Inter::interpret(const std::vector<std::wstring>& tokens)
{
    std::wstring logMessage = L"> [dungeon] Unable to interpret command";
    auto nTokens = tokens.size();

    if (nTokens >= 4u) {
        // Construct room
        if (tokens[0u] == L"construct" && tokens[1u] == L"room") {
            logMessage = L"> [dungeon] Constructing room " + tokens[2u] + L"/" + tokens[3u];
            bool hard = (nTokens >= 5u) && (tokens[4u] == L"hard");
            m_data->constructRoom({to<uint>(tokens[2u]), to<uint>(tokens[3u])}, hard);
        }
    }

    // Generate log
    context::Command command;
    return context::setCommandLog(command, logMessage);
}

void Inter::autoComplete(std::vector<std::wstring>& possibilities,
                         const std::vector<std::wstring>& tokens, const std::wstring& lastToken)
{
    // TODO How to automatize that?
    // Have a tree of tokens?

    auto nTokens = tokens.size();

    if (nTokens == 0u) {
        if (std::wstring(L"construct").find(lastToken) == 0u)
            possibilities.emplace_back(L"construct");
    }
    else if (nTokens == 1u && tokens[0u] == L"construct") {
        if (std::wstring(L"room").find(lastToken) == 0u)
            possibilities.emplace_back(L"room");
    }
}

//------------------------//
//----- Dungeon data -----//

void Inter::useData(Data& data)
{
    m_data = &data;
    setEmitter(&data);
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

    for (uint floor = 0u; floor < floorsCount; ++floor)
    for (uint room = 0u; room < roomsByFloor; ++room) {
        Tile tile;
        tile.coords = {floor, room};
        tile.room = &m_data->room({floor, room});
        m_tiles[{floor, room}] = std::move(tile);
    }

    // Sets the new size
    refreshSize();
    refreshTiles();
    refreshMonsters();
}

//---------------------------//
//----- Tile management -----//

sf::Vector2f Inter::tileLocalPosition(const sf::Vector2u& coords) const
{
    return m_grid.cellPosition(m_data->floorsCount() - coords.x - 1u, coords.y);
}

sf::Vector2u Inter::tileFromLocalPosition(const sf::Vector2f& pos) const
{
    auto coords = m_grid.rowColumnFromCoords(pos);
    coords.x = m_data->floorsCount() - coords.x - 1u;
    return coords;
}

sf::Vector2f Inter::relTileLocalPosition(const sf::Vector2f& relCoords) const
{
    sf::Vector2f pos;
    pos.x = relCoords.y * tileSize().x;
    pos.y = size().y - relCoords.x * tileSize().y;
    return pos;
}

sf::Vector2f Inter::relTileFromLocalPosition(const sf::Vector2f& pos) const
{
    sf::Vector2f relCoords;
    relCoords.x = (size().y - pos.y) / tileSize().y;
    relCoords.y = pos.x / tileSize().x;
    return relCoords;
}

void Inter::addLayer(const sf::Vector2u& coords, const std::string& textureID, float depth)
{
    auto& tile = m_tiles.at(coords);

    auto layer = std::make_unique<scene::RectangleShape>();
    layer->setTexture(textureID);
    layer->setLocalPosition(tileLocalPosition(coords));
    layer->setSize(tileSize());
    layer->setDepth(depth);

    tile.layers.emplace_back(std::move(layer));
    attachChild(*tile.layers.back());
}

void Inter::clearLayers(const sf::Vector2u& coords)
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
    selectTile(tileFromLocalPosition(pos));
}

void Inter::selectTile(const sf::Vector2u& coords)
{
    deselectTile();
    m_selectedTile = &m_tiles.at(coords);

    for (auto& layer : m_selectedTile->layers)
        layer->setShader("nui/select");

    // Animated sprites do not handle shaders right now...
    // so, nothing else to select
}

void Inter::deselectTile()
{
    returnif(m_selectedTile == nullptr);

    for (auto& layer : m_selectedTile->layers)
        layer->setShader("");

    m_selectedTile = nullptr;
}

//------------------------//
//----- Hovered tile -----//

void Inter::setHoveredTile(const sf::Vector2u& coords)
{
    resetHoveredTile();
    m_hoveredTile = &m_tiles.at(coords);

    returnif(m_hoveredTile == m_selectedTile);

    for (auto& layer : m_hoveredTile->layers)
        layer->setShader("nui/hover");
}

void Inter::resetHoveredTile()
{
    returnif(m_hoveredTile == nullptr);
    returnif(m_hoveredTile == m_selectedTile);

    for (auto& layer : m_hoveredTile->layers)
        layer->setShader("");

    m_hoveredTile = nullptr;
}

//------------------------//
//----- Context menu -----//

void Inter::showTileContextMenu(const sf::Vector2u& coords, const sf::Vector2f& nuiPos)
{
    auto& room = m_data->room(coords);
    m_contextMenu.clearChoices();

    // Context title
    std::wstringstream roomName;
    roomName << _("Room") << L' ' << coords.x << L'/' << coords.y;
    m_contextMenu.setTitle(roomName.str());

    // Room does not exists yet
    if (room.state == Data::RoomState::VOID) {
        std::wstring text = _("Build room") + L" (-" + toWString(m_data->onConstructRoomCost) + L"d)";
        if (m_data->onConstructRoomCost <= m_data->villain().doshWallet.value())
            m_contextMenu.addChoice(text, [=]() { m_data->constructRoom(coords); });
        else
            m_contextMenu.addChoice(text, nullptr);
    }

    // Room does exists
    else {
        // TODO Show really how much we get for destroying (adding all elements inside)
        m_contextMenu.addChoice(_("Destroy room") + L" (+" + toWString(m_data->onDestroyRoomGain) + L"d)", [=]() {
            m_data->destroyRoom(coords);
        });

        // If treasure, modifiy pop-up
        // TODO Let the facility itself edit its treasure content
        for (const auto& facilityInfo : m_data->room(coords).facilities)
            if (facilityInfo.treasure != -1u)
                m_contextMenu.addChoice(_("Edit treasure dosh"), [=]() { showEditTreasureDialog(coords); });
    }

    // Context positions
    m_contextMenu.setLocalPosition(nuiPos);
    m_contextMenu.markForVisible(true);
}

void Inter::showEditTreasureDialog(const sf::Vector2u& coords)
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
    m_treasureEditSpinBox.setLocalPosition(tileLocalPosition(coords));
    m_treasureEditSpinBox.setMaxLimit(treasureDosh + m_data->villain().doshWallet.value());
    m_treasureEditSpinBox.setOnValueChangeCallback([this, &coords, &treasureDosh] (uint32 oldValue, uint32 newValue) {
        // Sub or add
        if (newValue >= oldValue) m_data->villain().doshWallet.sub(newValue - oldValue);
        else m_data->villain().doshWallet.add(oldValue - newValue);
        treasureDosh = newValue;

        // Global dosh changed
        emitter()->emit("dosh_changed");

        // Treasure dosh changed
        Event devent;
        devent.type = "facility_changed";
        devent.facility.room = {coords.x, coords.y};
        emitter()->emit(devent);
    });
}

//----------------------//
//----- Prediction -----//

void Inter::resetPrediction()
{
    m_predictionID = L"";
    m_predictionSprite.setVisible(false);
}

void Inter::setPredictionMonster(const sf::Vector2f& relPos, const std::wstring& monsterID)
{
    if (m_predictionID != monsterID) {
        m_predictionID = monsterID;
        m_predictionSprite.load(toString(L"dungeon/monsters/" + monsterID));
        m_predictionSprite.setVisible(true);
    }

    auto rPosition = relPos / tileSize();
    rPosition.x = std::floor(rPosition.x) + 0.5f;
    rPosition.y = std::floor(rPosition.y) + 0.5f;
    m_predictionSprite.setLocalPosition(rPosition * tileSize());

    if (m_data->addMonsterValid(tileFromLocalPosition(relPos), monsterID))
        m_predictionSprite.setTiltColor({119u, 192u, 119u, 200u});
    else
        m_predictionSprite.setTiltColor({192u, 119u, 119u, 200u});
}

//---------------------//
//----- Structure -----//

bool Inter::hasFacility(const sf::Vector2u& coords, const std::wstring& facilityID) const
{
    return m_data->hasFacility(coords, facilityID);
}

void Inter::setRoomWidth(const float roomWidth)
{
    // Note: We want room to keep the same ratio as original image.
    const float scaleFactor = roomWidth / m_refRoomSize.x;
    m_roomScale = {scaleFactor, scaleFactor};
    m_predictionSprite.setLocalScale(m_roomScale);
    refreshSize();
}

bool Inter::createRoomFacility(const sf::Vector2u& coords, const std::wstring& facilityID)
{
    return m_data->createRoomFacility(coords, facilityID);
}

void Inter::setRoomFacilityLink(const sf::Vector2u& coords, const std::wstring& facilityID, const sf::Vector2u& linkCoords)
{
    m_data->setRoomFacilityLink(coords, facilityID, linkCoords);
}

void Inter::setRoomTrap(const sf::Vector2f& relPos, const std::wstring& trapID)
{
    m_data->setRoomTrap(tileFromLocalPosition(relPos), trapID);
}

void Inter::moveMonsterFromReserve(const sf::Vector2f& relPos, const std::wstring& monsterID)
{
    m_data->moveMonsterFromReserve(tileFromLocalPosition(relPos), monsterID);
}

void Inter::addMonsterToReserve(const std::wstring& monsterID)
{
    // Increase countdown
    const auto& hireCountdown = monstersDB().get(monsterID).hireCountdown;
    m_data->addMonsterToReserve(monsterID, hireCountdown);
}

void Inter::constructRoom(const sf::Vector2f& relPos)
{
    m_data->constructRoom(tileFromLocalPosition(relPos));
}

void Inter::destroyRoom(const sf::Vector2u& coords)
{
    m_data->destroyRoom(coords);
}

void Inter::removeRoomTrap(const sf::Vector2f& relPos)
{
    m_data->removeRoomTrap(tileFromLocalPosition(relPos));
}

void Inter::removeRoomFacilities(const sf::Vector2f& relPos)
{
    m_data->removeRoomFacilities(tileFromLocalPosition(relPos));
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
    refreshFromData();
}

void Inter::setRoomsByFloor(uint value)
{
    m_data->setRoomsByFloor(value);
    refreshFromData();
}

//----------------//
//----- Dosh -----//

void Inter::harvestTileDosh(const sf::Vector2u& coords)
{
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
        doshLabel->setColor(color);
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

void Inter::refreshMonsters()
{
    auto& activeMonsters = m_data->monstersInfo().active;
    uint monstersCountBefore = m_monsters.size();
    uint monstersCount = activeMonsters.size();
    m_monsters.resize(monstersCount);

    // Create new ones
    for (uint i = monstersCountBefore; i < monstersCount; ++i) {
        auto& monster = m_monsters[i];
        monster = std::make_unique<Monster>(*this, m_data->graph());
        attachChild(*monster);
    }

    // Reparameter them all
    for (uint i = 0u; i < monstersCount; ++i) {
        auto& monster = m_monsters.at(i);
        auto& monsterInfo = activeMonsters.at(i);
        monster->bindElementData(monsterInfo.data);
        monster->setLocalScale(m_roomScale);
    }
}

void Inter::refreshTiles()
{
    for (auto& tile : m_tiles)
        refreshTile(tile.second.coords);
}

void Inter::refreshTile(const sf::Vector2u& coords)
{
    returnif (coords.x >= m_data->floorsCount());
    returnif (coords.y >= m_data->roomsByFloor());

    refreshTileDoshLabel(coords);
    refreshTileLayers(coords);
    refreshTileTraps(coords);
    refreshTileFacilities(coords);
}

void Inter::refreshNeighboursLayers(const sf::Vector2u& coords)
{
    refreshTileLayers(m_data->roomNeighbourCoords(coords, WEST));
    refreshTileLayers(m_data->roomNeighbourCoords(coords, SOUTH));
    refreshTileLayers(m_data->roomNeighbourCoords(coords, EAST));
    refreshTileLayers(m_data->roomNeighbourCoords(coords, NORTH));
}

void Inter::refreshTileDoshLabel(const sf::Vector2u& coords)
{
    returnif (coords.x >= m_data->floorsCount());
    returnif (coords.y >= m_data->roomsByFloor());

    const auto& room = m_data->room(coords);
    const auto& localPosition = tileLocalPosition(coords);
    auto& tile = m_tiles[coords];

    // Remove label if no room
    if (room.state == Data::RoomState::VOID) {
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

void Inter::refreshTileLayers(const sf::Vector2u& coords)
{
    returnif (coords.x >= m_data->floorsCount());
    returnif (coords.y >= m_data->roomsByFloor());

    const auto& room = m_data->room(coords);
    const auto state = room.state;

    // Reset
    clearLayers(coords);

    // Room is somehow bugged
    if (state == Data::RoomState::UNKNOWN) {
        std::cerr << "/!\\ Found a room with unknown state at " << coords << "." << std::endl;
        std::cerr << "If that is a recurrent issue, please report this bug." << std::endl;
        addLayer(coords, "default");
        return;
    }

    // Room is not constructed
    if (state == Data::RoomState::VOID)
    {
        addLayer(coords, "dungeon/inter/void_room");

        if (m_data->isRoomConstructed(m_data->roomNeighbourCoords(coords, WEST)))
            addLayer(coords, "dungeon/inter/void_west_transition");
        if (m_data->isRoomConstructed(m_data->roomNeighbourCoords(coords, SOUTH)))
            addLayer(coords, "dungeon/inter/void_south_transition");
        if (m_data->isRoomConstructed(m_data->roomNeighbourCoords(coords, EAST)))
            addLayer(coords, "dungeon/inter/void_east_transition");

        return;
    }

    // Add room textures
    addLayer(coords, "dungeon/inter/inner_wall", 100.f);
    addLayer(coords, "dungeon/inter/floor", 75.f);

    if (!m_data->isRoomConstructed(m_data->roomNeighbourCoords(coords, EAST)))
        addLayer(coords, "dungeon/inter/right_wall", 75.f);
}

void Inter::refreshTileFacilities(const sf::Vector2u& coords)
{
    returnif (coords.x >= m_data->floorsCount());
    returnif (coords.y >= m_data->roomsByFloor());

    auto& room = m_data->room(coords);
    auto& tile = m_tiles[coords];

    // Reset
    tile.facilities.clear();

    // Room is not constructed
    returnif (room.state != Data::RoomState::CONSTRUCTED);

    // Facilities
    for (auto& facilityInfo : room.facilities) {
        auto facility = std::make_unique<Facility>(coords, facilityInfo, *this);
        tile.facilities.emplace_back(std::move(facility));
        attachChild(*tile.facilities.back());
    }
}

void Inter::refreshTileTraps(const sf::Vector2u& coords)
{
    returnif (coords.x >= m_data->floorsCount());
    returnif (coords.y >= m_data->roomsByFloor());

    auto& room = m_data->room(coords);
    auto& tile = m_tiles[coords];

    // Reset
    tile.trap = nullptr;

    // Room is not constructed
    returnif (room.state != Data::RoomState::CONSTRUCTED);

    // Trap or no trap
    returnif (!room.trap.exists());

    // Trap
    tile.trap = std::make_unique<Trap>(coords, room.trap, *this);
    tile.trap->setEmitter(m_data);
    attachChild(*tile.trap);
}

void Inter::refreshOuterWalls()
{
    const auto& roomSize = Application::context().textures.get("dungeon/inter/inner_wall").getSize();

    // West wall
    const auto& westTextureSize = Application::context().textures.get("dungeon/inter/outer_wall_west").getSize();
    sf::Vector2f westRealSize(westTextureSize.x * m_grid.cellSize().x / roomSize.x, size().y);
    sf::IntRect westWallsRect(0, 0, westTextureSize.x, westTextureSize.y * m_grid.rows());
    m_outerWalls[0].setSize(westRealSize);
    m_outerWalls[0].setPosition(-westRealSize.x, 0.f);
    m_outerWalls[0].setTextureRect(westWallsRect);

    // East wall
    const auto& eastTextureSize = Application::context().textures.get("dungeon/inter/outer_wall_east").getSize();
    sf::Vector2f eastRealSize(eastTextureSize.x * m_grid.cellSize().x / roomSize.x, size().y);
    sf::IntRect eastWallsRect(0, 0, eastTextureSize.x, eastTextureSize.y * m_grid.rows());
    m_outerWalls[1].setSize(eastRealSize);
    m_outerWalls[1].setPosition(size().x, 0.f);
    m_outerWalls[1].setTextureRect(eastWallsRect);
}
