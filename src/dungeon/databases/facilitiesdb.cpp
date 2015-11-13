#include "dungeon/databases/facilitiesdb.hpp"

#include "core/gettext.hpp"
#include "tools/filesystem.hpp"

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
    for (const auto& fileInfo : listFiles("res/data/facilities", true)) {
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
    if (facilityData.listed) ++m_listedCount;

    // Adding data
    for (const auto& dataNode : facilityNode.children(L"data")) {
        std::wstring name = dataNode.attribute(L"name").as_string();

        if (name == L"baseCost")    readCostNode(facilityData.baseCost, dataNode);
    }

    // Adding links
    for (const auto& linkNode : facilityNode.children(L"link")) {
        Link link;
        link.id = linkNode.attribute(L"id").as_string();

        std::wstring style = linkNode.attribute(L"style").as_string();
        if (style == L"implicit") {
            link.style = Link::Style::IMPLICIT;
            link.x = linkNode.attribute(L"x").as_int();
            link.y = linkNode.attribute(L"y").as_int();
        }
        else if (style == L"explicit") {
            link.style = Link::Style::EXPLICIT;
            link.id = linkNode.attribute(L"id").as_string();
        }

        facilityData.links.emplace_back(std::move(link));
    }
}

void FacilitiesDB::readCostNode(Cost& cost, const pugi::xml_node& node)
{
    cost.dosh = node.attribute(L"dosh").as_uint();
    cost.soul = node.attribute(L"soul").as_uint();
    cost.fame = node.attribute(L"fame").as_uint();
}
