#include "dungeon/databases/facilitiesdb.hpp"

#include "core/gettext.hpp"
#include "tools/filesystem.hpp"
#include "tools/tools.hpp"

#include <pugixml/pugixml.hpp>
#include <stdexcept>

using namespace dungeon;

FacilitiesDB::FacilitiesDB()
{
    reload();
}

//------------------------//
//----- Data control -----//

void FacilitiesDB::reload()
{
    m_listedCount = 0u;
    m_facilitiesData.clear();

    // Loading from XML files
    for (const auto& fileInfo : listFiles("res/vanilla/facilities", true)) {
        // Check the file extension
        if (fileInfo.isDirectory || fileExtension(fileInfo.name) != "xml")
            continue;

        // Add its content to the map
        add(fileInfo.fullName);
    }
}

void FacilitiesDB::add(const std::string& filename)
{
    // Parsing XML
    pugi::xml_document doc;
    doc.load_file(filename.c_str());

    // Check that its a correct file
    const auto& facilityNode = doc.child(L"facility");
    if (!facilityNode)
        throw std::logic_error("File '" + filename + "' is not a correct facility data file.");

    // Getting the ID
    std::wstring id = facilityNode.attribute(L"id").as_string();
    if (id.empty())
        throw std::logic_error("Facility ID is unspecified. Cannot add '" + filename + "' file.");
    if (m_facilitiesData.find(id) != std::end(m_facilitiesData))
        throw std::logic_error("Facility ID already exists. Cannot add '" + filename + "' file.");

    // Create the corresponding data
    auto& facilityData = m_facilitiesData[id];
    std::wstring trName = facilityNode.attribute(L"trName").as_string();
    facilityData.name = _(toString(trName).c_str());
    facilityData.listed = facilityNode.attribute(L"listed").as_bool(true);
    facilityData.entrance = facilityNode.attribute(L"entrance").as_bool(false);
    facilityData.energetic = facilityNode.attribute(L"energetic").as_bool(false);
    facilityData.permissive = facilityNode.attribute(L"permissive").as_bool(false);
    readRoomFlagsAttribute(facilityData.lock, facilityNode.attribute(L"lock"));
    readRoomFlagsAttribute(facilityData.hide, facilityNode.attribute(L"hide"));
    if (facilityData.listed) ++m_listedCount;

    // Adding data
    for (const auto& dataNode : facilityNode.children(L"data")) {
        std::wstring name = dataNode.attribute(L"name").as_string();

        if (name == L"baseCost")    readCostNode(facilityData.baseCost, dataNode);
    }

    // Constraints
    for (const auto& constraintNode : facilityNode.children(L"constraint")) {
        Constraint constraint;
        readConstraintNode(constraint, constraintNode);
        facilityData.constraints.emplace_back(std::move(constraint));
    }

    // Fixed links
    for (const auto& linkNode : facilityNode.children(L"fixedLink")) {
        FixedLink link;
        readLinkNode(link, linkNode);
        link.originFacilityID = id;
        link.relative = linkNode.attribute(L"relative").as_bool();
        link.coords.x = linkNode.attribute(L"x").as_int();
        link.coords.y = linkNode.attribute(L"y").as_int();
        facilityData.fixedLinks.emplace_back(std::move(link));
    }

    // Interactive links
    for (const auto& linkNode : facilityNode.children(L"interactiveLink")) {
        InteractiveLink link;
        readLinkNode(link, linkNode);
        link.originFacilityID = id;
        for (const auto& constraintNode : linkNode.children(L"constraint")) {
            Constraint constraint;
            readConstraintNode(constraint, constraintNode);
            link.constraints.emplace_back(std::move(constraint));
        }
        facilityData.interactiveLinks.emplace_back(std::move(link));
    }
}

void FacilitiesDB::readLinkNode(Link& link, const pugi::xml_node& node)
{
    link.id = node.attribute(L"id").as_uint(-1u);
    link.facilityID = node.attribute(L"facilityID").as_string();
    link.unbreakable = node.attribute(L"unbreakable").as_bool();

    if (!link.facilityID.empty()) {
        link.strong = node.attribute(L"strong").as_bool();
        link.relink = node.attribute(L"relink").as_bool();
        link.relinkID = node.attribute(L"relinkID").as_uint(link.id);
    }
}

void FacilitiesDB::readRoomFlagsAttribute(uint8& lock, const pugi::xml_attribute& attribute)
{
    std::wstring lockString = attribute.as_string();

    if (lockString == L"wall")          lock = RoomFlag::WALL;
    else if (lockString == L"floor")    lock = RoomFlag::FLOOR;
    else if (lockString == L"full")     lock = RoomFlag::WALL | RoomFlag::FLOOR;
    else                                lock = RoomFlag::NONE;
}

void FacilitiesDB::readConstraintNode(Constraint& constraint, const pugi::xml_node& node)
{
    std::wstring mode = node.attribute(L"mode").as_string();
    if (mode == L"include")         constraint.mode = Constraint::Mode::INCLUDE;
    else if (mode == L"exclude")    constraint.mode = Constraint::Mode::EXCLUDE;
    readConstraintParameterAttribute(constraint.x, node.attribute(L"x"));
    readConstraintParameterAttribute(constraint.y, node.attribute(L"y"));
}

void FacilitiesDB::readConstraintParameterAttribute(ConstraintParameter& constraintParameter, const pugi::xml_attribute& attribute)
{
    returnif (!attribute);
    constraintParameter.type = ConstraintParameter::Type::EQUAL;
    constraintParameter.value = attribute.as_int();
}

void FacilitiesDB::readCostNode(Cost& cost, const pugi::xml_node& node)
{
    cost.dosh = node.attribute(L"dosh").as_uint();
    cost.soul = node.attribute(L"soul").as_uint();
    cost.fame = node.attribute(L"fame").as_uint();
}
