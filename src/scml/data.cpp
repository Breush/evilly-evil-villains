#include "scml/data.hpp"

#include "scml/entityprototype.hpp"
#include "tools/tools.hpp"
#include "tools/debug.hpp"

#include <pugixml/pugixml.hpp>
#include <cstdarg> // va_start
#include <stdexcept> // runtime_error
#include <iostream>

using namespace scml;

Data::Data()
{}

Data::Data(const std::string& file)
{
    load(file);
}

Data::Data(pugi::xml_node& elem)
{
    load(elem);
}

Data::~Data()
{
    clear();
}

bool Data::load(const std::string& file)
{
    name = file;
    pugi::xml_document doc;

    if (!doc.load_file(file.c_str())) {
        throw std::runtime_error("scml::Data failed to load: Couldn't open " + file);
        return false;
    }

    auto root = doc.child(L"spriter_data");
    if (!root) {
        throw std::runtime_error("scml::Data failed to load: No spriter_data XML element in " + file);
        return false;
    }

    load(root);
    return true;
}

bool Data::loadFromFile(const std::string& file)
{
    return load(file);
}

bool Data::load(pugi::xml_node& elem)
{
    returnif (!elem) false;

    scml_version = elem.attribute(L"scml_version").as_string(scml_version.c_str());
    generator = elem.attribute(L"generator").as_string(generator.c_str());
    generator_version = elem.attribute(L"generator_version").as_string(generator_version.c_str());
    pixel_art_mode = elem.attribute(L"pixel_art_mode").as_bool(pixel_art_mode);
    debug_scml_1(log());

    // Meta data
    auto meta_data_child = elem.child(L"meta_data");
    if (meta_data_child) {
        if (meta_data == nullptr)
            meta_data = new Meta_Data;
        meta_data->load(meta_data_child);
    }

    // Folder
    for (auto& child : elem.children(L"folder")) {
        Folder* folder = new Folder;
        if (folder->load(child)) {
            if (!folders.insert(std::make_pair(folder->id, folder)).second) {
                throw std::runtime_error("scml::Data loaded a folder with a duplicated id.");
                delete folder;
            }
        } else {
            throw std::runtime_error("scml::Data failed to load a folder");
            delete folder;
        }
    }

    // Atlas
    for (auto& child : elem.children(L"atlas")) {
        Atlas* atlas = new Atlas;
        if (atlas->load(child)) {
            if (!atlases.insert(std::make_pair(atlas->id, atlas)).second) {
                throw std::runtime_error("scml::Data loaded an atlas with a duplicated id.");
                delete atlas;
            }
        } else {
            throw std::runtime_error("scml::Data failed to load an atlas");
            delete atlas;
        }
    }

    // Entity
    for (auto& child : elem.children(L"entity")) {
        Entity* entity = new Entity;
        if (entity->load(child)) {
            if (!entities.insert(std::make_pair(entity->id, entity)).second) {
                throw std::runtime_error("scml::Data loaded an entity with a duplicated id.");
                delete entity;
            }
        } else {
            throw std::runtime_error("scml::Data failed to load an entity");
            delete entity;
        }
    }

    // Character_map
    for (auto& child : elem.children(L"character_map")) {
        Character_Map* character_map = new Character_Map;
        if (character_map->load(child)) {
            if (!character_maps.insert(std::make_pair(character_map->id, character_map)).second) {
                throw std::runtime_error("scml::Data loaded a character_map with a duplicated id.");
                delete character_map;
            }
        } else {
            throw std::runtime_error("scml::Data failed to load a character_map");
            delete character_map;
        }
    }

    // Document information
    auto document_info_elem = elem.child(L"document_info");
    if (document_info_elem) {
        document_info.load(document_info_elem);
    }

    return true;
}

void Data::log() const
{
    std::wcout << "# Document >> ";
    std::wcout << "scml_version=" << scml_version << L" | ";
    std::wcout << "generator=" << generator << L" | ";
    std::wcout << "generator_version=" << generator_version << L" | ";
    std::wcout << "pixel_art_mode=" << pixel_art_mode << std::endl;
}

void Data::clear()
{
    scml_version = L"Unknown";
    generator = L"Unknown";
    generator_version = L"Unknown";
    pixel_art_mode = false;

    delete meta_data;
    meta_data = nullptr;

    for (auto const& folder: folders) {
        delete folder.second;
    }

    folders.clear();

    for (auto const& atlas : atlases) {
        delete atlas.second;
    }
    atlases.clear();

    for (auto const& entity : entities) {
        delete entity.second;
    }
    entities.clear();

    for (auto const& character_map : character_maps) {
        delete character_map.second;
    }
    character_maps.clear();

    document_info.clear();
}


int Data::getNumAnimations(int entity) const
{
    Entity* e = tools::mapFind(entities, entity);
    if (e == nullptr) {
        return -1;
    }

    return e->animations.size();
}

//---------------------//
//----- Meta data -----//

Data::Meta_Data::Meta_Data()
{}

Data::Meta_Data::Meta_Data(pugi::xml_node& elem)
{
    load(elem);
}

bool Data::Meta_Data::load(pugi::xml_node& elem)
{
    // Variable
    for (auto& child : elem.children(L"variable")) {
        Variable* variable = new Variable;
        if (variable->load(child)) {
            if (!variables.insert(std::make_pair(variable->name, variable)).second) {
                throw std::runtime_error("scml::Data::Meta_Data loaded a variable with the duplicate name.");
                delete variable;
            }
        } else {
            throw std::runtime_error("scml::Data::Meta_Data failed to load a variable.");
            delete variable;
        }
    }

    // Tag
    for (auto& child : elem.children(L"tag")) {
        Tag* tag = new Tag;
        if (tag->load(child)) {
            if (!tags.insert(std::make_pair(tag->name, tag)).second) {
                throw std::runtime_error("scml::Data::Meta_Data loaded a tag with a duplicate name.");
                delete tag;
            }
        } else {
            throw std::runtime_error("scml::Data::Meta_Data failed to load a tag.");
            delete tag;
        }
    }

    return true;
}

void Data::Meta_Data::clear()
{
    for (auto const& variable : variables) {
        delete variable.second;
    }
    variables.clear();

    for (auto const& tag : tags) {
        delete tag.second;
    }
    tags.clear();
}

//--------------------//
//----- Variable -----//

Data::Meta_Data::Variable::Variable()
    : type(L"string")
{}

Data::Meta_Data::Variable::Variable(pugi::xml_node& elem)
    : type(L"string")
{
    load(elem);
}

bool Data::Meta_Data::Variable::load(pugi::xml_node& elem)
{
    name = elem.attribute(L"name").as_string(name.c_str());
    type = elem.attribute(L"type").as_string(type.c_str());

    if (type == L"string") value_string = elem.attribute(L"value").as_string(value_string.c_str());
    else if (type == L"int") value_int = elem.attribute(L"value").as_int(value_int);
    else if (type == L"float")value_float = elem.attribute(L"value").as_float(value_float);
    else throw std::runtime_error("Data::Meta_Data::Variable loaded invalid variable type.");

    debug_scml_2(log());

    return true;
}

void Data::Meta_Data::Variable::log() const
{
    std::wcout << L"# Meta data :: Variable >> ";
    std::wcout << L"name=" << name << L" | ";
    std::wcout << L"type=" << type << std::endl;;
    if (type == L"string")     std::wcout << L"value=" << value_string << std::endl;
    else if (type == L"int")   std::wcout << L"value=" << value_int << std::endl;
    else if (type == L"float") std::wcout << L"value=" << value_float << std::endl;
}

void Data::Meta_Data::Variable::clear()
{
    name.clear();
    type = L"string";
    value_string.clear();
    value_int = 0;
    value_float = 0.0f;
}

//---------------//
//----- Tag -----//

Data::Meta_Data::Tag::Tag()
{}

Data::Meta_Data::Tag::Tag(pugi::xml_node& elem)
{
    load(elem);
}

bool Data::Meta_Data::Tag::load(pugi::xml_node& elem)
{
    name = elem.attribute(L"name").as_string(name.c_str());
    debug_scml_2(log());

    return true;
}

void Data::Meta_Data::Tag::log() const
{
    std::wcout << L"# Meta data :: Tag >> ";
    std::wcout << L"name=" << name << std::endl;;
}

void Data::Meta_Data::Tag::clear()
{
    name.clear();
}

//------------------//
//----- Folder -----//

Data::Folder::Folder()
{}

Data::Folder::Folder(pugi::xml_node& elem)
{
    load(elem);
}

Data::Folder::~Folder()
{
    clear();
}

bool Data::Folder::load(pugi::xml_node& elem)
{
    id = elem.attribute(L"id").as_int(id);
    name = elem.attribute(L"name").as_string(name.c_str());
    debug_scml_1(log());

    // File
    for (auto& child : elem.children(L"file")) {
        File* file = new File;
        if (file->load(child)) {
            if (!files.insert(std::make_pair(file->id, file)).second) {
                throw std::runtime_error("scml::Data::Folder loaded a file with a duplicated id.");
                delete file;
            }
        } else {
            throw std::runtime_error("scml::Data::Folder failed to load a file");
            delete file;
        }
    }

    return true;
}

void Data::Folder::log() const
{
    std::wcout << L"# Folder >> ";
    std::wcout << L"id=" << id << " | ";
    std::wcout << L"name=" << name << std::endl;;
}

void Data::Folder::clear()
{
    id = 0;
    name.clear();

    for (auto const& file : files)
        delete file.second;
    files.clear();
}

//----------------//
//----- File -----//

Data::Folder::File::File()
    : type(L"image")
    , pivot_x(0.f), pivot_y(1.f)
{}

Data::Folder::File::File(pugi::xml_node& elem)
    : type(L"image")
    , pivot_x(0.f), pivot_y(1.f)
{
    load(elem);
}

bool Data::Folder::File::load(pugi::xml_node& elem)
{
    type = elem.attribute(L"type").as_string(type.c_str());
    id = elem.attribute(L"id").as_int(id);
    name = elem.attribute(L"name").as_string(name.c_str());
    pivot_x = elem.attribute(L"pivot_x").as_float(pivot_x);
    pivot_y = elem.attribute(L"pivot_y").as_float(pivot_y);
    width = elem.attribute(L"width").as_int(width);
    height = elem.attribute(L"height").as_int(height);
    atlas_x = elem.attribute(L"atlas_x").as_int(atlas_x);
    atlas_y = elem.attribute(L"atlas_y").as_int(atlas_y);
    offset_x = elem.attribute(L"offset_x").as_int(offset_x);
    offset_y = elem.attribute(L"offset_y").as_int(offset_y);
    original_width = elem.attribute(L"original_width").as_int(original_width);
    original_height = elem.attribute(L"original_height").as_int(original_height);
    debug_scml_1(log());

    return true;
}

void Data::Folder::File::log() const
{
    std::wcout << L"# Folder :: File >> ";
    std::wcout << L"type=" << type << L" | ";
    std::wcout << L"id=" << id << " | ";
    std::wcout << L"name=" << name << L" | ";
    std::wcout << L"pivot_x=" << pivot_x << " | ";
    std::wcout << L"pivot_y=" << pivot_y << " | ";
    std::wcout << L"width=" << width << " | ";
    std::wcout << L"height=" << height << " | ";
    std::wcout << L"atlas_x=" << atlas_x << " | ";
    std::wcout << L"atlas_y=" << atlas_y << " | ";
    std::wcout << L"offset_x=" << offset_x << " | ";
    std::wcout << L"offset_y=" << offset_y << " | ";
    std::wcout << L"original_width=" << original_width << " | ";
    std::wcout << L"original_height=" << original_height << std::endl;
}

void Data::Folder::File::clear()
{
    type = L"image";
    id = 0;
    name.clear();
    pivot_x = 0.0f;
    pivot_y = 1.0f;
    width = 0;
    height = 0;
    atlas_x = 0;
    atlas_y = 0;
    offset_x = 0;
    offset_y = 0;
    original_width = 0;
    original_height = 0;
}

//-----------------//
//----- Atlas -----//

Data::Atlas::Atlas()
{}

Data::Atlas::Atlas(pugi::xml_node& elem)
{
    load(elem);
}

Data::Atlas::~Atlas()
{
    clear();
}

bool Data::Atlas::load(pugi::xml_node& elem)
{
    id = elem.attribute(L"id").as_int(id);
    data_path = elem.attribute(L"data_path").as_string(data_path.c_str());
    image_path = elem.attribute(L"image_path").as_string(image_path.c_str());
    debug_scml_2(log());

    // Folder
    for (auto& child : elem.children(L"folder")) {
        Folder* folder = new Folder;
        if (folder->load(child)) {
            if (!folders.insert(std::make_pair(folder->id, folder)).second) {
                throw std::runtime_error("scml::Data::Atlas loaded a folder with a duplicated id.");
                delete folder;
            }
        } else {
            throw std::runtime_error("scml::Data::Atlas failed to load a folder");
            delete folder;
        }
    }

    return true;
}

void Data::Atlas::log() const
{
    std::wcout << L"# Atlas >> ";
    std::wcout << L"id=" << id << " | ";
    std::wcout << L"data_path=" << data_path << L" | ";
    std::wcout << L"image_path=" << image_path << std::endl;;
}

void Data::Atlas::clear()
{
    id = 0;
    data_path.clear();
    image_path.clear();

    for (auto const& folder : folders)
        delete folder.second;
    folders.clear();
}

//------------------------//
//----- Atlas Folder -----//

Data::Atlas::Folder::Folder()
{}

Data::Atlas::Folder::Folder(pugi::xml_node& elem)
{
    load(elem);
}

bool Data::Atlas::Folder::load(pugi::xml_node& elem)
{
    id = elem.attribute(L"id").as_int(id);
    name = elem.attribute(L"name").as_string(name.c_str());

    for (auto& child : elem.children(L"image")) {
        Image* image = new Image;
        if (image->load(child)) {
            if (!images.insert(std::make_pair(image->id, image)).second) {
                throw std::runtime_error("scml::Data::Atlas::Folder loaded an image with a duplicated id.");
                delete image;
            }
        } else {
            throw std::runtime_error("scml::Data::Atlas::Folder failed to load an image");
            delete image;
        }
    }

    return true;
}

void Data::Atlas::Folder::log() const
{
    std::wcout << L"# Atlas :: Folder" << " | ";
    std::wcout << L"id=" << id << " | ";
    std::wcout << L"name=" << name << std::endl;;
}

void Data::Atlas::Folder::clear()
{
    id = 0;
    name.clear();

    for (auto const& image : images)
        delete image.second;
    images.clear();
}

//-----------------//
//----- Image -----//

Data::Atlas::Folder::Image::Image()
{}

Data::Atlas::Folder::Image::Image(pugi::xml_node& elem)
{
    load(elem);
}

bool Data::Atlas::Folder::Image::load(pugi::xml_node& elem)
{
    id = elem.attribute(L"id").as_int(id);
    full_path = elem.attribute(L"full_path").as_string(full_path.c_str());
    debug_scml_1(log());

    return true;
}

void Data::Atlas::Folder::Image::log() const
{
    std::wcout << L"# Atlas :: Folder :: Image" << " | ";
    std::wcout << L"id=" << id << " | ";
    std::wcout << L"full_path=" << full_path << std::endl;;
}

void Data::Atlas::Folder::Image::clear()
{
    id = 0;
    full_path.clear();
}

//------------------//
//----- Entity -----//

Data::Entity::Entity()
    : meta_data(nullptr)
{}

Data::Entity::Entity(pugi::xml_node& elem)
    : meta_data(nullptr)
{
    load(elem);
}

Data::Entity::~Entity()
{
    clear();
}

bool Data::Entity::load(pugi::xml_node& elem)
{
    id = elem.attribute(L"id").as_int(id);
    name = elem.attribute(L"name").as_string(name.c_str());

    // Meta data
    auto meta_data_child = elem.child(L"meta_data");
    if (meta_data_child) {
        if (meta_data == nullptr) {
            meta_data = new Meta_Data;
        }
        meta_data->load(meta_data_child);
    }

    // Animation
    for (auto& child : elem.children(L"animation")) {
        Animation* animation = new Animation;
        if (animation->load(child)) {
            if (!animations.insert(std::make_pair(animation->id, animation)).second) {
                throw std::runtime_error("scml::Data::Entity loaded an animation with a duplicated id.");
                delete animation;
            }
        } else {
            throw std::runtime_error("scml::Data::Entity failed to load an animation.");
            delete animation;
        }
    }

    return true;
}

void Data::Entity::log() const
{
    std::wcout << L"# Entity" << " | ";
    std::wcout << L"id=" << id << " | ";
    std::wcout << L"name=" << name << std::endl;;
}

void Data::Entity::clear()
{
    id = 0;
    name.clear();
    delete meta_data;
    meta_data = nullptr;

    for (auto const& animation : animations)
        delete animation.second;
    animations.clear();
}

//---------------------//
//----- Animation -----//

Data::Entity::Animation::Animation()
    : looping(L"true")
    , meta_data(nullptr)
{}

Data::Entity::Animation::Animation(pugi::xml_node& elem)
    : looping(L"true")
    , meta_data(nullptr)
{
    load(elem);
}

bool Data::Entity::Animation::load(pugi::xml_node& elem)
{
    id = elem.attribute(L"id").as_int(id);
    name = elem.attribute(L"name").as_string(name.c_str());
    length = elem.attribute(L"length").as_int(length);
    looping = elem.attribute(L"looping").as_string(looping.c_str());
    loop_to = elem.attribute(L"loop_to").as_int(loop_to);
    debug_scml_2(log());

    // Meta data
    auto meta_data_child = elem.child(L"meta_data");
    if (meta_data_child) {
        if (meta_data == nullptr)
            meta_data = new Meta_Data;
        meta_data->load(meta_data_child);
    }

    // Mainline
    auto mainline_elem = elem.child(L"mainline");
    if (!mainline_elem || !mainline.load(mainline_elem)) {
        throw std::runtime_error("scml::Data::Entity::Animation failed to load the mainline.");
        mainline.clear();
    }

    // Timeline
    for (auto& child : elem.children(L"timeline")) {
        Timeline* timeline = new Timeline;
        if (timeline->load(child)) {
            if (!timelines.insert(std::make_pair(timeline->id, timeline)).second) {
                throw std::runtime_error("scml::Data::Entity::Animation loaded a timeline with a duplicated id.");
                delete timeline;
            }
        } else {
            throw std::runtime_error("scml::Data::Entity::Animation failed to load a timeline.");
            delete timeline;
        }
    }

    // Soundline
    auto soundline_elem = elem.child(L"soundline");
    if (!soundline_elem || !soundline.load(soundline_elem)) {
        soundline.clear();
    }

    return true;
}

void Data::Entity::Animation::log() const
{
    std::wcout << L"# Entity:: Animation" << " | ";
    std::wcout << L"id=" << id << " | ";
    std::wcout << L"name=" << name << L" | ";
    std::wcout << L"length=" << length << " | ";
    std::wcout << L"looping=" << looping << L" | ";
    std::wcout << L"loop_to=" << loop_to << std::endl;
}

Data::Entity::Animation::~Animation()
{
    clear();
}

void Data::Entity::Animation::clear()
{
    id = 0;
    name.clear();
    length = 0;
    looping = L"true";
    loop_to = 0;

    delete meta_data;
    meta_data = nullptr;

    mainline.clear();

    for (auto const& timeline : timelines)
        delete timeline.second;
    timelines.clear();
}

//--------------------//
//----- Mainline -----//

Data::Entity::Animation::Mainline::Mainline()
{
}

Data::Entity::Animation::Mainline::Mainline(pugi::xml_node& elem)
{
    load(elem);
}

Data::Entity::Animation::Mainline::~Mainline()
{
    clear();
}

bool Data::Entity::Animation::Mainline::load(pugi::xml_node& elem)
{
    // Key
    for (auto& child : elem.children(L"key")) {
        Key* key = new Key;
        if (key->load(child)) {
            if (!keys.insert(std::make_pair(key->id, key)).second) {
                throw std::runtime_error("scml::Data::Entity::Animation::Mainline loaded a key with a duplicated id.");
                delete key;
            }
        } else {
            throw std::runtime_error("scml::Data::Entity::Animation::Mainline failed to load a key.");
            delete key;
        }
    }

    return true;
}

void Data::Entity::Animation::Mainline::clear()
{
    for (auto const& key : keys) {
        delete key.second;
    }
    keys.clear();
}

//------------------------//
//----- Mainline Key -----//

Data::Entity::Animation::Mainline::Key::Key()
    : id(0)
    , time(0)
    , meta_data(nullptr)
{}

Data::Entity::Animation::Mainline::Key::Key(pugi::xml_node& elem)
    : id(0)
    , time(0)
    , meta_data(nullptr)
{
    load(elem);
}

bool Data::Entity::Animation::Mainline::Key::load(pugi::xml_node& elem)
{
    id = elem.attribute(L"id").as_int(id);
    time = elem.attribute(L"time").as_int(time);
    debug_scml_2(log());

    // Meta data
    auto meta_data_child = elem.child(L"meta_data");
    if (meta_data_child) {
        if (meta_data == nullptr)
            meta_data = new Meta_Data;
        meta_data->load(meta_data_child);
    }

    // Bone
    for (auto& child : elem.children(L"bone")) {
        Bone* bone = new Bone;
        if (bone->load(child)) {
            if (!bones.insert(std::make_pair(bone->id, bone)).second) {
                throw std::runtime_error("scml::Data::Entity::Animation::Mainline::Key loaded a bone with a duplicated id.");
                delete bone;
            }
        } else {
            throw std::runtime_error("scml::Data::Entity::Animation::Mainline::Key failed to load a bone");
            delete bone;
        }
    }

    // Bone_ref
    for (auto& child : elem.children(L"bone_ref")) {
        Bone_Ref* bone_ref = new Bone_Ref;
        if (bone_ref->load(child)) {
            if (!bones.insert(std::make_pair(bone_ref->id, Bone_Container(bone_ref))).second) {
                throw std::runtime_error("scml::Data::Entity::Animation::Mainline::Key loaded a bone_ref with a duplicated id.");
                delete bone_ref;
            }
        } else {
            throw std::runtime_error("scml::Data::Entity::Animation::Mainline::Key failed to load a bone_ref");
            delete bone_ref;
        }
    }

    // Object
    for (auto& child : elem.children(L"object")) {
        Object* object = new Object;
        if (object->load(child)) {
            if (!objects.insert(std::make_pair(object->id, object)).second) {
                throw std::runtime_error("scml::Data::Entity::Animation::Mainline::Key loaded an object with a duplicated id.");
                delete object;
            }
        } else {
            throw std::runtime_error("scml::Data::Entity::Animation::Mainline::Key failed to load an object");
            delete object;
        }
    }

    // Object_ref
    for (auto& child : elem.children(L"object_ref")) {
        Object_Ref* object_ref = new Object_Ref;
        if (object_ref->load(child)) {
            if (!objects.insert(std::make_pair(object_ref->id, Object_Container(object_ref))).second) {
                throw std::runtime_error("scml::Data::Entity::Animation::Mainline::Key loaded an object_ref with a duplicated id.");
                delete object_ref;
            }
        } else {
            throw std::runtime_error("scml::Data::Entity::Animation::Mainline::Key failed to load an object_ref");
            delete object_ref;
        }
    }

    return true;
}

void Data::Entity::Animation::Mainline::Key::log() const
{
    std::wcout << L"# Entity :: Animation :: Mainline :: Key" << " | ";
    std::wcout << L"id=" << id << " | ";
    std::wcout << L"time=" << time << std::endl;
}

Data::Entity::Animation::Mainline::Key::~Key()
{
    clear();
}

void Data::Entity::Animation::Mainline::Key::clear()
{
    id = 0;
    time = 0;

    delete meta_data;
    meta_data = nullptr;

    for (auto const& bone : bones) {
        delete bone.second.bone;
        delete bone.second.bone_ref;
    }
    bones.clear();

    for (auto const& object : objects) {
        delete object.second.object;
        delete object.second.object_ref;
    }
    objects.clear();
}

//----------------//
//----- Bone -----//

Data::Entity::Animation::Mainline::Key::Bone::Bone()
    : id(0)
    , parent(-1)
    , x(0.0f), y(0.0f)
    , angle(0.0f)
    , scale_x(1.0f), scale_y(1.0f)
    , r(1.0f), g(1.0f), b(1.0f), a(1.0f)
    , meta_data(nullptr)
{}

Data::Entity::Animation::Mainline::Key::Bone::Bone(pugi::xml_node& elem)
    : id(0)
    , parent(-1)
    , x(0.0f), y(0.0f)
    , angle(0.0f)
    , scale_x(1.0f), scale_y(1.0f)
    , r(1.0f), g(1.0f), b(1.0f), a(1.0f)
    , meta_data(nullptr)
{
    load(elem);
}

bool Data::Entity::Animation::Mainline::Key::Bone::load(pugi::xml_node& elem)
{
    id = elem.attribute(L"id").as_int(id);
    parent = elem.attribute(L"parent").as_int(parent);
    x = elem.attribute(L"x").as_float(x);
    y = elem.attribute(L"y").as_float(y);
    angle = elem.attribute(L"angle").as_float(angle);
    scale_x = elem.attribute(L"scale_x").as_float(scale_x);
    scale_y = elem.attribute(L"scale_y").as_float(scale_y);
    r = elem.attribute(L"r").as_float(r);
    g = elem.attribute(L"g").as_float(g);
    b = elem.attribute(L"b").as_float(b);
    a = elem.attribute(L"a").as_float(a);

    auto meta_data_child = elem.child(L"meta_data");
    if (meta_data_child) {
        if (meta_data == nullptr)
            meta_data = new Meta_Data;
        meta_data->load(meta_data_child);
    }

    return true;
}

void Data::Entity::Animation::Mainline::Key::Bone::log() const
{
    std::wcout << L"id=" << id << " | ";
    std::wcout << L"parent=" << parent << " | ";
    std::wcout << L"x=" << x << " | ";
    std::wcout << L"y=" << y << " | ";
    std::wcout << L"angle=" << angle << " | ";
    std::wcout << L"scale_x=" << scale_x << " | ";
    std::wcout << L"scale_y=" << scale_y << " | ";
    std::wcout << L"r=" << r << " | ";
    std::wcout << L"g=" << g << " | ";
    std::wcout << L"b=" << b << " | ";
    std::wcout << L"a=" << a << std::endl;
}

void Data::Entity::Animation::Mainline::Key::Bone::clear()
{
    id = 0;
    parent = -1;
    x = 0.0f;
    y = 0.0f;
    angle = 0.0f;
    scale_x = 1.0f;
    scale_y = 1.0f;
    r = 1.0f;
    g = 1.0f;
    b = 1.0f;
    a = 1.0f;

    delete meta_data;
    meta_data = nullptr;
}

//--------------------//
//----- Bone_ref -----//

Data::Entity::Animation::Mainline::Key::Bone_Ref::Bone_Ref()
    : id(0)
    , parent(-1)
    , timeline(0)
    , key(0)
{}

Data::Entity::Animation::Mainline::Key::Bone_Ref::Bone_Ref(pugi::xml_node& elem)
    : id(0)
    , parent(-1)
    , timeline(0)
    , key(0)
{
    load(elem);
}

bool Data::Entity::Animation::Mainline::Key::Bone_Ref::load(pugi::xml_node& elem)
{
    id = elem.attribute(L"id").as_int(id);
    parent = elem.attribute(L"parent").as_int(parent);
    timeline = elem.attribute(L"timeline").as_int(timeline);
    key = elem.attribute(L"key").as_int(key);
    debug_scml_2(log());

    return true;
}

void Data::Entity::Animation::Mainline::Key::Bone_Ref::log() const
{
    std::wcout << L"# Entity :: Mainline :: Key :: Bone_Ref >> ";
    std::wcout << L"id=" << id << " | ";
    std::wcout << L"parent=" << parent << " | ";
    std::wcout << L"timeline=" << timeline << " | ";
    std::wcout << L"key=" << key << std::endl;

}

void Data::Entity::Animation::Mainline::Key::Bone_Ref::clear()
{
    id = 0;
    parent = -1;
    timeline = 0;
    key = 0;
}

//------------------//
//----- Object -----//

Data::Entity::Animation::Mainline::Key::Object::Object()
    : id(0)
    , parent(-1)
    , object_type(L"sprite")
    , atlas(0), folder(0), file(0)
    , usage(L"display")
    , blend_mode(L"alpha")
    , x(0.0f), y(0.0f)
    , pivot_x(0.0f), pivot_y(1.0f)
    , pixel_art_mode_x(0), pixel_art_mode_y(0)
    , pixel_art_mode_pivot_x(0), pixel_art_mode_pivot_y(0)
    , angle(0.0f)
    , w(0.0f), h(0.0f)
    , scale_x(1.0f), scale_y(1.0f)
    , r(1.0f), g(1.0f), b(1.0f), a(1.0f)
    , variable_type(L"string")
    , value_string(L"")
    , value_int(0), min_int(0), max_int(0)
    , value_float(0.0f), min_float(0.0f), max_float(0.0f)
    , animation(0), t(0.0f)
    , z_index(0)
    , volume(1.0f), panning(0.0f)
    , meta_data(nullptr)
{}

Data::Entity::Animation::Mainline::Key::Object::Object(pugi::xml_node& elem)
    : id(0)
    , parent(-1)
    , object_type(L"sprite")
    , atlas(0), folder(0), file(0)
    , usage(L"display")
    , blend_mode(L"alpha")
    , x(0.0f), y(0.0f)
    , pivot_x(0.0f), pivot_y(1.0f)
    , pixel_art_mode_x(0), pixel_art_mode_y(0)
    , pixel_art_mode_pivot_x(0), pixel_art_mode_pivot_y(0)
    , angle(0.0f)
    , w(0.0f), h(0.0f)
    , scale_x(1.0f), scale_y(1.0f)
    , r(1.0f), g(1.0f), b(1.0f), a(1.0f)
    , variable_type(L"string")
    , value_string(L"")
    , value_int(0), min_int(0), max_int(0)
    , value_float(0.0f), min_float(0.0f), max_float(0.0f)
    , animation(0), t(0.0f)
    , z_index(0)
    , volume(1.0f), panning(0.0f)
    , meta_data(nullptr)
{
    load(elem);
}

bool Data::Entity::Animation::Mainline::Key::Object::load(pugi::xml_node& elem)
{
    id = elem.attribute(L"id").as_int(id);
    parent = elem.attribute(L"parent").as_int(parent);
    object_type = elem.attribute(L"object_type").as_string(object_type.c_str());
    atlas = elem.attribute(L"atlas").as_int(atlas);
    folder = elem.attribute(L"folder").as_int(folder);
    file = elem.attribute(L"file").as_int(file);
    usage = elem.attribute(L"usage").as_string(usage.c_str());
    blend_mode = elem.attribute(L"blend_mode").as_string(blend_mode.c_str());
    x = elem.attribute(L"x").as_float(x);
    y = elem.attribute(L"y").as_float(y);
    pivot_x = elem.attribute(L"pivot_x").as_float(pivot_x);
    pivot_y = elem.attribute(L"pivot_y").as_float(pivot_y);
    pixel_art_mode_x = elem.attribute(L"x").as_int(pixel_art_mode_x);
    pixel_art_mode_y = elem.attribute(L"y").as_int(pixel_art_mode_y);
    pixel_art_mode_pivot_x = elem.attribute(L"pivot_x").as_int(pixel_art_mode_pivot_x);
    pixel_art_mode_pivot_y = elem.attribute(L"pivot_y").as_int(pixel_art_mode_pivot_y);
    angle = elem.attribute(L"angle").as_float(angle);
    w = elem.attribute(L"w").as_float(w);
    h = elem.attribute(L"h").as_float(h);
    scale_x = elem.attribute(L"scale_x").as_float(scale_x);
    scale_y = elem.attribute(L"scale_y").as_float(scale_y);
    r = elem.attribute(L"r").as_float(r);
    g = elem.attribute(L"g").as_float(g);
    b = elem.attribute(L"b").as_float(b);
    a = elem.attribute(L"a").as_float(a);
    variable_type = elem.attribute(L"variable_type").as_string(variable_type.c_str());
    debug_scml_2(log());

    if (variable_type == L"string") {
        value_string = elem.attribute(L"value").as_string(value_string.c_str());
    } else if (variable_type == L"int") {
        value_int = elem.attribute(L"value").as_int(value_int);
        min_int = elem.attribute(L"min").as_int(min_int);
        max_int = elem.attribute(L"max").as_int(max_int);
    } else if (variable_type == L"float") {
        value_float = elem.attribute(L"value").as_float(value_float);
        min_float = elem.attribute(L"min").as_float(min_float);
        max_float = elem.attribute(L"max").as_float(max_float);
    }

    animation = elem.attribute(L"animation").as_int(animation);
    t = elem.attribute(L"t").as_float(t);
    z_index = elem.attribute(L"z_index").as_int(z_index);

    if (object_type == L"sound") {
        volume = elem.attribute(L"volume").as_float(volume);
        panning = elem.attribute(L"panning").as_float(panning);
    }

    // Meta data
    auto meta_data_child = elem.child(L"meta_data");
    if (meta_data_child) {
        if (meta_data == nullptr) {
            meta_data = new Meta_Data;
        }
        meta_data->load(meta_data_child);
    }

    return true;
}

void Data::Entity::Animation::Mainline::Key::Object::log() const
{
    std::wcout << L"# Entity :: Animation :: Mainline :: Key :: Object >> ";
    std::wcout << L"id=" << id << " | ";
    std::wcout << L"parent=" << parent << " | ";
    std::wcout << L"object_type=" << object_type << L" | ";
    std::wcout << L"atlas=" << atlas << " | ";
    std::wcout << L"folder=" << folder << " | ";
    std::wcout << L"file=" << file << " | ";
    std::wcout << L"usage=" << usage << L" | ";
    std::wcout << L"blend_mode=" << blend_mode << L" | ";
    std::wcout << L"x=" << x << " | ";
    std::wcout << L"y=" << y << " | ";
    std::wcout << L"pivot_x=" << pivot_x << " | ";
    std::wcout << L"pivot_y=" << pivot_y << " | ";
    std::wcout << L"pixel_art_mode_x=" << pixel_art_mode_x << " | ";
    std::wcout << L"pixel_art_mode_y=" << pixel_art_mode_y << " | ";
    std::wcout << L"pixel_art_mode_pivot_x=" << pixel_art_mode_pivot_x << " | ";
    std::wcout << L"pixel_art_mode_pivot_y=" << pixel_art_mode_pivot_y << " | ";
    std::wcout << L"angle=" << angle << " | ";
    std::wcout << L"w=" << w << " | ";
    std::wcout << L"h=" << h << " | ";
    std::wcout << L"scale_x=" << scale_x << " | ";
    std::wcout << L"scale_y=" << scale_y << " | ";
    std::wcout << L"r=" << r << " | ";
    std::wcout << L"g=" << g << " | ";
    std::wcout << L"b=" << b << " | ";
    std::wcout << L"a=" << a << " | ";
    std::wcout << L"variable_type=" << variable_type << std::endl;;
}

void Data::Entity::Animation::Mainline::Key::Object::clear()
{
    id = 0;
    parent = -1;
    object_type = L"sprite";
    atlas = 0;
    folder = 0;
    file = 0;
    usage = L"display";
    blend_mode = L"alpha";
    x = 0.0f;
    y = 0.0f;
    pivot_x = 0.0f;
    pivot_y = 1.0f;
    pixel_art_mode_x = 0;
    pixel_art_mode_y = 0;
    pixel_art_mode_pivot_x = 0;
    pixel_art_mode_pivot_y = 0;
    angle = 0.0f;
    w = 0.0f;
    h = 0.0f;
    scale_x = 1.0f;
    scale_y = 1.0f;
    r = 1.0f;
    g = 1.0f;
    b = 1.0f;
    a = 1.0f;
    variable_type = L"string";
    value_string.clear();
    value_int = 0;
    value_float = 0.0f;
    min_int = 0;
    min_float = 0.0f;
    max_int = 0;
    max_float = 0.0f;
    animation = 0;
    t = 0.0f;
    z_index = 0;
    volume = 1.0f;
    panning = 0.0f;

    delete meta_data;
    meta_data = nullptr;
}

//----------------------//
//----- Object ref -----//

Data::Entity::Animation::Mainline::Key::Object_Ref::Object_Ref()
    : id(0)
    , parent(-1)
    , timeline(0)
    , key(0)
    , z_index(0)
    , abs_x(0.f), abs_y(0.f)
    , abs_pivot_x(0.f), abs_pivot_y(1.f)
    , abs_angle(0.f)
    , abs_scale_x(1.f), abs_scale_y(1.f)
    , abs_a(1.f)
{}

Data::Entity::Animation::Mainline::Key::Object_Ref::Object_Ref(pugi::xml_node& elem)
    : id(0)
    , parent(-1)
    , timeline(0)
    , key(0)
    , z_index(0)
    , abs_x(0.f), abs_y(0.f)
    , abs_pivot_x(0.f), abs_pivot_y(1.f)
    , abs_angle(0.f)
    , abs_scale_x(1.f), abs_scale_y(1.f)
    , abs_a(1.f)
{
    load(elem);
}

bool Data::Entity::Animation::Mainline::Key::Object_Ref::load(pugi::xml_node& elem)
{
    id = elem.attribute(L"id").as_int(id);
    parent = elem.attribute(L"parent").as_int(parent);
    timeline = elem.attribute(L"timeline").as_int(timeline);
    key = elem.attribute(L"key").as_int(key);
    z_index = elem.attribute(L"z_index").as_int(z_index);
    debug_scml_2(log());

    abs_x = elem.attribute(L"abs_x").as_float(abs_x);
    abs_y = elem.attribute(L"abs_y").as_float(abs_y);
    abs_pivot_x = elem.attribute(L"abs_pivot_x").as_float(abs_pivot_x);
    abs_pivot_y = elem.attribute(L"abs_pivot_y").as_float(abs_pivot_y);
    abs_angle = elem.attribute(L"abs_angle").as_float(abs_angle);
    abs_scale_x = elem.attribute(L"abs_scale_x").as_float(abs_scale_x);
    abs_scale_y = elem.attribute(L"abs_scale_y").as_float(abs_scale_y);
    abs_a = elem.attribute(L"abs_a").as_float(abs_a);

    return true;
}

void Data::Entity::Animation::Mainline::Key::Object_Ref::log() const
{
    std::wcout << L"# Entity :: Animation :: Mainline :: Key >> ";
    std::wcout << L"id=" << id << " | ";
    std::wcout << L"parent=" << parent << " | ";
    std::wcout << L"timeline=" << timeline << " | ";
    std::wcout << L"key=" << key << " | ";
    std::wcout << L"z_index=" << z_index << std::endl;

}

void Data::Entity::Animation::Mainline::Key::Object_Ref::clear()
{
    id = 0;
    parent = -1;
    timeline = 0;
    key = 0;
    z_index = 0;

    abs_x = 0.f;
    abs_y = 0.f;
    abs_pivot_x = 0.f;
    abs_pivot_y = 1.f;
    abs_angle = 0.f;
    abs_scale_x = 1.f;
    abs_scale_y = 1.f;
    abs_a = 1.f;
}

//--------------------//
//----- Timeline -----//

Data::Entity::Animation::Timeline::Timeline()
    : id(0)
    , name(L"")
    , object_type(L"sprite")
    , variable_type(L"string")
    , usage(L"display")
    , meta_data(nullptr)
{}

Data::Entity::Animation::Timeline::Timeline(pugi::xml_node& elem)
    : id(0)
    , name(L"")
    , object_type(L"sprite")
    , variable_type(L"string")
    , usage(L"display")
    , meta_data(nullptr)
{
    load(elem);
}

Data::Entity::Animation::Timeline::~Timeline()
{
    clear();
}

bool Data::Entity::Animation::Timeline::load(pugi::xml_node& elem)
{
    id = elem.attribute(L"id").as_int(id);
    object_type = elem.attribute(L"object_type").as_string(object_type.c_str());
    variable_type = elem.attribute(L"variable_type").as_string(variable_type.c_str());
    debug_scml_2(log());

    if (object_type != L"sound") name = elem.attribute(L"name").as_string(name.c_str());

    if (object_type == L"point") usage = elem.attribute(L"usage").as_string(L"neither");
    else if (object_type == L"box") usage = elem.attribute(L"usage").as_string(L"sprite");
    else if (object_type == L"sprite") usage = elem.attribute(L"usage").as_string(L"display");
    else if (object_type == L"entity") usage = elem.attribute(L"usage").as_string(L"display");

    // Meta data
    auto meta_data_child = elem.child(L"meta_data");
    if (meta_data_child) {
        if (meta_data == nullptr)
            meta_data = new Meta_Data;
        meta_data->load(meta_data_child);
    }

    for (auto& child : elem.children(L"key")) {
        Key* key = new Key;
        if (key->load(child)) {
            if (!keys.insert(std::make_pair(key->id, key)).second) {
                throw std::runtime_error("scml::Data::Entity::Animation::Timeline loaded a key with a duplicated id.");
                delete key;
            }
        } else {
            throw std::runtime_error("scml::Data::Entity::Animation::Timeline failed to load a key.");
            delete key;
        }
    }

    return true;
}

void Data::Entity::Animation::Timeline::log() const
{
    std::wcout << L"# Entity :: Animation :: Timeline >> ";
    std::wcout << L"id=" << id << " | ";
    std::wcout << L"name=" << name << L" | ";
    std::wcout << L"object_type=" << object_type << L" | ";
    std::wcout << L"variable_type=" << variable_type << L" | ";
    std::wcout << L"usage=" << usage << std::endl;;
}

void Data::Entity::Animation::Timeline::clear()
{
    id = 0;
    name.clear();
    object_type = L"sprite";
    variable_type = L"string";
    usage = L"display";

    delete meta_data;
    meta_data = nullptr;

    for (auto const& key : keys) {
        delete key.second;
    }
    keys.clear();
}

//------------------------//
//----- Timeline key -----//

Data::Entity::Animation::Timeline::Key::Key()
    : id(0)
    , time(0)
    , curve_type(L"linear")
    , c1(0.0f), c2(0.0f)
    , spin(1)
    , meta_data(nullptr)
{}

Data::Entity::Animation::Timeline::Key::Key(pugi::xml_node& elem)
    : id(0)
    , time(0)
    , curve_type(L"linear")
    , c1(0.0f), c2(0.0f)
    , spin(1)
    , meta_data(nullptr)
{
    load(elem);
}

Data::Entity::Animation::Timeline::Key::~Key()
{
    clear();
}

bool Data::Entity::Animation::Timeline::Key::load(pugi::xml_node& elem)
{
    id = elem.attribute(L"id").as_int(id);
    time = elem.attribute(L"time").as_int(time);
    curve_type = elem.attribute(L"curve_type").as_string(curve_type.c_str());
    c1 = elem.attribute(L"c1").as_float(c1);
    c2 = elem.attribute(L"c2").as_float(c2);
    spin = elem.attribute(L"spin").as_int(spin);
    debug_scml_2(log());

    // Meta data tweenable
    auto meta_data_child = elem.child(L"meta_data");
    if (meta_data_child) {
        if (meta_data == nullptr)
            meta_data = new Meta_Data_Tweenable;
        meta_data->load(meta_data_child);
    }

    has_object = true;

    // Bone
    auto child = elem.child(L"bone");
    if (child) {
        has_object = false;
        if (!bone.load(child)) {
            throw std::runtime_error("scml::Data::Entity::Animation::Timeline::Key failed to load a bone.");
        }
    }

    // Object
    child = elem.child(L"object");
    if (child != nullptr && !object.load(child)) {
        throw std::runtime_error("scml::Data::Entity::Animation::Timeline::Key failed to load an object.");
        has_object = true;
    }

    return true;
}

void Data::Entity::Animation::Timeline::Key::log() const
{
    std::wcout << L"# Entity :: Animation :: Timeline :: Key >> ";
    std::wcout << L"id=" << id << " | ";
    std::wcout << L"time=" << time << " | ";
    std::wcout << L"curve_type=" << curve_type << L" | ";
    std::wcout << L"c1=" << c1 << " | ";
    std::wcout << L"c2=" << c2 << " | ";
    std::wcout << L"spin=" << spin << std::endl;
}

void Data::Entity::Animation::Timeline::Key::clear()
{
    id = 0;
    time = 0;
    curve_type = L"linear";
    c1 = 0.0f;
    c2 = 0.0f;
    spin = 1;

    delete meta_data;
    meta_data = nullptr;

    bone.clear();
    object.clear();
}

//-------------------------------//
//----- Meta data tweenable -----//

Data::Meta_Data_Tweenable::Meta_Data_Tweenable()
{}

Data::Meta_Data_Tweenable::Meta_Data_Tweenable(pugi::xml_node& elem)
{
    load(elem);
}

bool Data::Meta_Data_Tweenable::load(pugi::xml_node& elem)
{
    for (auto& child : elem.children(L"variable")) {
        Variable* variable = new Variable;
        if (variable->load(child)) {
            if (!variables.insert(std::make_pair(variable->name, variable)).second) {
                throw std::runtime_error("scml::Data::Meta_Data_Tweenable loaded a variable with the duplicate name.");
                delete variable;
            }
        } else {
            throw std::runtime_error("scml::Data::Meta_Data_Tweenable failed to load a variable");
            delete variable;
        }
    }

    return true;
}

void Data::Meta_Data_Tweenable::clear()
{
    for (auto const& variable : variables) {
        delete variable.second;
    }
    variables.clear();
}

//--------------------//
//----- Variable -----//

Data::Meta_Data_Tweenable::Variable::Variable()
    : type(L"string")
    , value_string(L"")
    , value_int(0)
    , value_float(0.0f)
    , curve_type(L"linear")
    , c1(0.0f)
    , c2(0.0f)
{}

Data::Meta_Data_Tweenable::Variable::Variable(pugi::xml_node& elem)
    : type(L"string")
    , value_string(L"")
    , value_int(0)
    , value_float(0.0f)
    , curve_type(L"linear")
    , c1(0.0f)
    , c2(0.0f)
{
    load(elem);
}

bool Data::Meta_Data_Tweenable::Variable::load(pugi::xml_node& elem)
{
    type = elem.attribute(L"type").as_string(type.c_str());

    if (type == L"string") value_string = elem.attribute(L"value").as_string(value_string.c_str());
    else if (type == L"int") value_int = elem.attribute(L"value").as_int(value_int);
    else if (type == L"float") value_float = elem.attribute(L"value").as_float(value_float);

    curve_type = elem.attribute(L"curve_type").as_string(curve_type.c_str());
    c1 = elem.attribute(L"c1").as_float(c1);
    c2 = elem.attribute(L"c2").as_float(c2);
    debug_scml_2(log());

    return true;
}

void Data::Meta_Data_Tweenable::Variable::log() const
{
    std::wcout << L"# Meta_Data_Tweenable :: Variable >> ";
    std::wcout << L"type=" << type << std::endl;;
    if (type == L"string") std::wcout << L"value=" << value_string << std::endl;
    else if (type == L"int") std::wcout << L"value=" << value_int << std::endl;
    else if (type == L"float") std::wcout << L"value=" << value_float << std::endl;

    std::wcout << L"curve_type=" << curve_type << L" | ";
    std::wcout << L"c1=" << c1 << " | ";
    std::wcout << L"c2=" << c2 << std::endl;
}

void Data::Meta_Data_Tweenable::Variable::clear()
{
    type = L"string";
    value_string.clear();
    value_int = 0;
    value_float = 0.0f;

    curve_type = L"linear";
    c1 = 0.0f;
    c2 = 0.0f;
}

//----------------//
//----- Bone -----//

Data::Entity::Animation::Timeline::Key::Bone::Bone()
    : x(0.0f), y(0.0f)
    , angle(0.0f)
    , scale_x(1.0f), scale_y(1.0f)
    , r(1.0f), g(1.0f), b(1.0f), a(1.0f)
    , meta_data(nullptr)
{}

Data::Entity::Animation::Timeline::Key::Bone::Bone(pugi::xml_node& elem)
    : x(0.0f), y(0.0f)
    , angle(0.0f)
    , scale_x(1.0f), scale_y(1.0f)
    , r(1.0f), g(1.0f), b(1.0f), a(1.0f)
    , meta_data(nullptr)
{
    load(elem);
}

bool Data::Entity::Animation::Timeline::Key::Bone::load(pugi::xml_node& elem)
{
    x = elem.attribute(L"x").as_float(x);
    y = elem.attribute(L"y").as_float(y);
    angle = elem.attribute(L"angle").as_float(angle);
    scale_x = elem.attribute(L"scale_x").as_float(scale_x);
    scale_y = elem.attribute(L"scale_y").as_float(scale_y);
    r = elem.attribute(L"r").as_float(r);
    g = elem.attribute(L"g").as_float(g);
    b = elem.attribute(L"b").as_float(b);
    a = elem.attribute(L"a").as_float(a);

    // Meta data
    auto meta_data_child = elem.child(L"meta_data");
    if (meta_data_child) {
        if (meta_data == nullptr)
            meta_data = new Meta_Data_Tweenable;
        meta_data->load(meta_data_child);
    }

    return true;
}

void Data::Entity::Animation::Timeline::Key::Bone::log() const
{
    std::wcout << L"x=" << x << " | ";
    std::wcout << L"y=" << y << " | ";
    std::wcout << L"angle=" << angle << " | ";
    std::wcout << L"scale_x=" << scale_x << " | ";
    std::wcout << L"scale_y=" << scale_y << " | ";
    std::wcout << L"r=" << r << " | ";
    std::wcout << L"g=" << g << " | ";
    std::wcout << L"b=" << b << " | ";
    std::wcout << L"a=" << a << std::endl;
}

void Data::Entity::Animation::Timeline::Key::Bone::clear()
{
    x = 0.0f;
    y = 0.0f;
    angle = 0.0f;
    scale_x = 1.0f;
    scale_y = 1.0f;
    r = 1.0f;
    g = 1.0f;
    b = 1.0f;
    a = 1.0f;

    delete meta_data;
    meta_data = nullptr;
}

//------------------//
//----- Object -----//

Data::Entity::Animation::Timeline::Key::Object::Object()
    : atlas(0), folder(0), file(0)
    , x(0.0f), y(0.0f)
    , pivot_x(0.0f), pivot_y(1.0f)
    , angle(0.0f)
    , w(0.0f), h(0.0f)
    , scale_x(1.0f), scale_y(1.0f)
    , r(1.0f), g(1.0f), b(1.0f), a(1.0f)
    , blend_mode(L"alpha")
    , value_string(L"")
    , value_int(0), min_int(0), max_int(0)
    , value_float(0.0f), min_float(0.0f), max_float(0.0f)
    , animation(0), t(0.0f)
    , volume(1.0f), panning(0.0f)
    , meta_data(nullptr)
{}

Data::Entity::Animation::Timeline::Key::Object::Object(pugi::xml_node& elem)
    : atlas(0), folder(0), file(0)
    , x(0.0f), y(0.0f)
    , pivot_x(0.0f), pivot_y(1.0f)
    , angle(0.0f)
    , w(0.0f), h(0.0f)
    , scale_x(1.0f), scale_y(1.0f)
    , r(1.0f), g(1.0f), b(1.0f), a(1.0f)
    , blend_mode(L"alpha")
    , value_string(L"")
    , value_int(0), min_int(0), max_int(0)
    , value_float(0.0f), min_float(0.0f), max_float(0.0f)
    , animation(0), t(0.0f)
    , volume(1.0f), panning(0.0f)
    , meta_data(nullptr)
{
    load(elem);
}

bool Data::Entity::Animation::Timeline::Key::Object::load(pugi::xml_node& elem)
{
    atlas = elem.attribute(L"atlas").as_int(atlas);
    folder = elem.attribute(L"folder").as_int(folder);
    file = elem.attribute(L"file").as_int(file);
    x = elem.attribute(L"x").as_float(x);
    y = elem.attribute(L"y").as_float(y);
    pivot_x = elem.attribute(L"pivot_x").as_float(pivot_x);
    pivot_y = elem.attribute(L"pivot_y").as_float(pivot_y);
    angle = elem.attribute(L"angle").as_float(angle);
    w = elem.attribute(L"w").as_float(w);
    h = elem.attribute(L"h").as_float(h);
    scale_x = elem.attribute(L"scale_x").as_float(scale_x);
    scale_y = elem.attribute(L"scale_y").as_float(scale_y);
    r = elem.attribute(L"r").as_float(r);
    g = elem.attribute(L"g").as_float(g);
    b = elem.attribute(L"b").as_float(b);
    a = elem.attribute(L"a").as_float(a);
    blend_mode = elem.attribute(L"blend_mode").as_string(blend_mode.c_str());
    value_string = elem.attribute(L"value").as_string(value_string.c_str());
    value_int = elem.attribute(L"value").as_int(value_int);
    min_int = elem.attribute(L"min").as_int(min_int);
    max_int = elem.attribute(L"max").as_int(max_int);
    value_float = elem.attribute(L"value").as_float(value_float);
    min_float = elem.attribute(L"min").as_float(min_float);
    max_float = elem.attribute(L"max").as_float(max_float);
    animation = elem.attribute(L"animation").as_int(animation);
    t = elem.attribute(L"t").as_float(t);
    volume = elem.attribute(L"volume").as_float(volume);
    panning = elem.attribute(L"panning").as_float(panning);
    debug_scml_2(log());

    // Meta data
    auto meta_data_child = elem.child(L"meta_data");
    if (meta_data_child) {
        if (meta_data == nullptr)
            meta_data = new Meta_Data_Tweenable;
        meta_data->load(meta_data_child);
    }

    return true;
}

void Data::Entity::Animation::Timeline::Key::Object::log() const
{
    std::wcout << L"# Entity :: Animation :: Timeline :: Key :: Object >> ";
    std::wcout << L"atlas=" << atlas << " | ";
    std::wcout << L"folder=" << folder << " | ";
    std::wcout << L"file=" << file << " | ";
    std::wcout << L"x=" << x << " | ";
    std::wcout << L"y=" << y << " | ";
    std::wcout << L"pivot_x=" << pivot_x << " | ";
    std::wcout << L"pivot_y=" << pivot_y << " | ";
    std::wcout << L"angle=" << angle << " | ";
    std::wcout << L"w=" << w << " | ";
    std::wcout << L"h=" << h << " | ";
    std::wcout << L"scale_x=" << scale_x << " | ";
    std::wcout << L"scale_y=" << scale_y << " | ";
    std::wcout << L"r=" << r << " | ";
    std::wcout << L"g=" << g << " | ";
    std::wcout << L"b=" << b << " | ";
    std::wcout << L"a=" << a << " | ";
    std::wcout << L"blend_mode=" << blend_mode << L" | ";
    std::wcout << L"value=" << value_string << L" | ";
    std::wcout << L"animation=" << animation << " | ";
    std::wcout << L"t=" << t << " | ";
    std::wcout << L"volume=" << volume << " | ";
    std::wcout << L"panning=" << panning << std::endl;
}

void Data::Entity::Animation::Timeline::Key::Object::clear()
{
    atlas = 0;
    folder = 0;
    file = 0;
    x = 0.0f;
    y = 0.0f;
    pivot_x = 0.0f;
    pivot_y = 1.0f;
    angle = 0.0f;
    w = 0.0f;
    h = 0.0f;
    scale_x = 1.0f;
    scale_y = 1.0f;
    r = 1.0f;
    g = 1.0f;
    b = 1.0f;
    a = 1.0f;
    blend_mode = L"alpha";
    value_string.clear();
    value_int = 0;
    value_float = 0.0f;
    min_int = 0;
    min_float = 0.0f;
    max_int = 0;
    max_float = 0.0f;
    animation = 0;
    t = 0.0f;
    volume = 1.0f;
    panning = 0.0f;
}

//---------------------//
//----- Soundline -----//

Data::Entity::Animation::Soundline::Soundline()
    : id(0)
    , name(L"")
{}

Data::Entity::Animation::Soundline::Soundline(pugi::xml_node& elem)
    : id(0)
    , name(L"")
{}

Data::Entity::Animation::Soundline::~Soundline()
{
    clear();
}

bool Data::Entity::Animation::Soundline::load(pugi::xml_node& elem)
{
    id = elem.attribute(L"id").as_int(id);
    name = elem.attribute(L"name").as_string(name.c_str());
    debug_scml_1(log());

    for (auto& child : elem.children(L"key")) {
        Key* key = new Key;
        if (key->load(child)) {
            if (!keys.insert(std::make_pair(key->id, key)).second) {
                throw std::runtime_error("scml::Data::Entity::Animation::Soundline loaded a key with a duplicated id.");
                delete key;
            }
        } else {
            throw std::runtime_error("scml::Data::Entity::Animation::Soundline failed to load a key.");
            delete key;
        }
    }

    return true;
}

void Data::Entity::Animation::Soundline::log() const
{
    std::wcout << L"# Entity :: Animation :: Soundline >> ";
    std::wcout << L"id=" << id << " | ";
    std::wcout << L"name=" << name << std::endl;;
}

void Data::Entity::Animation::Soundline::clear()
{
    id = 0;
    name.clear();

    keys.clear();
}

//------------------------//
//----- Soundline key -----//

Data::Entity::Animation::Soundline::Key::Key()
    : id(0)
{}

Data::Entity::Animation::Soundline::Key::Key(pugi::xml_node& elem)
    : id(0)
{
    load(elem);
}

Data::Entity::Animation::Soundline::Key::~Key()
{
    clear();
}

bool Data::Entity::Animation::Soundline::Key::load(pugi::xml_node& elem)
{
    id = elem.attribute(L"id").as_int(id);
    debug_scml_1(log());

    // Object
    auto child = elem.child(L"object");
    if (child != nullptr && !object.load(child)) {
        throw std::runtime_error("scml::Data::Entity::Animation::Soundline::Key failed to load an object.");
    }

    return true;
}

void Data::Entity::Animation::Soundline::Key::log() const
{
    std::wcout << L"# Entity :: Animation :: Soundline :: Key >> ";
    std::wcout << L"id=" << id << " | " << std::endl;
}

void Data::Entity::Animation::Soundline::Key::clear()
{
    id = 0;

    object.clear();
}

//--------------------------------//
//----- Soundline Key Object -----//

Data::Entity::Animation::Soundline::Key::Object::Object()
    : atlas(0), folder(0), file(0)
{}

Data::Entity::Animation::Soundline::Key::Object::Object(pugi::xml_node& elem)
    : atlas(0), folder(0), file(0)
{
    load(elem);
}

bool Data::Entity::Animation::Soundline::Key::Object::load(pugi::xml_node& elem)
{
    atlas = elem.attribute(L"atlas").as_int(atlas);
    folder = elem.attribute(L"folder").as_int(folder);
    file = elem.attribute(L"file").as_int(file);
    debug_scml_1(log());

    return true;
}

void Data::Entity::Animation::Soundline::Key::Object::log() const
{
    std::wcout << L"# Entity :: Animation :: Soundline :: Key :: Object >> ";
    std::wcout << L"atlas=" << atlas << " | ";
    std::wcout << L"folder=" << folder << " | ";
    std::wcout << L"file=" << file << " | " << std::endl;
}

void Data::Entity::Animation::Soundline::Key::Object::clear()
{
    atlas = 0;
    folder = 0;
    file = 0;
}

//-------------------------//
//----- Character map -----//

Data::Character_Map::Character_Map()
    : id(0)
    , name(L"")
{}

Data::Character_Map::Character_Map(pugi::xml_node& elem)
    : id(0)
    , name(L"")
{
    load(elem);
}

bool Data::Character_Map::load(pugi::xml_node& elem)
{
    id = elem.attribute(L"id").as_int(id);
    name = elem.attribute(L"name").as_string(name.c_str());
    debug_scml_1(log());

    auto child = elem.child(L"map");
    if (child || !map.load(child))
        throw std::runtime_error("scml::Data::Entity failed to load an animation");

    return true;
}

void Data::Character_Map::log() const
{
    std::wcout << L"# Character map >> ";
    std::wcout << L"id=" << id << " | ";
    std::wcout << L"name=" << name << std::endl;;
}

void Data::Character_Map::clear()
{
    id = 0;
    name.clear();
    map.clear();
}

//---------------//
//----- Map -----//

Data::Character_Map::Map::Map()
    : atlas(0), folder(0), file(0)
    , target_atlas(0), target_folder(0), target_file(0)
{}

Data::Character_Map::Map::Map(pugi::xml_node& elem)
    : atlas(0), folder(0), file(0)
    , target_atlas(0), target_folder(0), target_file(0)
{
    load(elem);
}

bool Data::Character_Map::Map::load(pugi::xml_node& elem)
{
    atlas = elem.attribute(L"atlas").as_int(atlas);
    folder = elem.attribute(L"folder").as_int(folder);
    file = elem.attribute(L"file").as_int(file);
    target_atlas = elem.attribute(L"target_atlas").as_int(target_atlas);
    target_folder = elem.attribute(L"target_folder").as_int(target_folder);
    target_file = elem.attribute(L"target_file").as_int(target_file);

    return true;
}

void Data::Character_Map::Map::log() const
{
    std::wcout << L"# Character map :: Map >> ";
    std::wcout << L"atlas=" << atlas << " | ";
    std::wcout << L"folder=" << folder << " | ";
    std::wcout << L"file=" << file << " | ";
    std::wcout << L"target_atlas=" << target_atlas << " | ";
    std::wcout << L"target_folder=" << target_folder << " | ";
    std::wcout << L"target_file=" << target_file << std::endl;
}

void Data::Character_Map::Map::clear()
{
    atlas = 0;
    folder = 0;
    file = 0;
    target_atlas = 0;
    target_folder = 0;
    target_file = 0;
}

//-------------------------//
//----- Document Info -----//

Data::Document_Info::Document_Info()
    : author(L"Unknown")
    , copyright(L"Unknown")
    , license(L"Unknown")
    , version(L"Unknown")
    , last_modified(L"Unknown")
{}

Data::Document_Info::Document_Info(pugi::xml_node& elem)
    : author(L"Unknown")
    , copyright(L"Unknown")
    , license(L"Unknown")
    , version(L"Unknown")
    , last_modified(L"Unknown")
{
    load(elem);
}

bool Data::Document_Info::load(pugi::xml_node& elem)
{
    author = elem.attribute(L"author").as_string(author.c_str());
    copyright = elem.attribute(L"copyright").as_string(copyright.c_str());
    license = elem.attribute(L"license").as_string(license.c_str());
    version = elem.attribute(L"version").as_string(version.c_str());
    last_modified = elem.attribute(L"last_modified").as_string(last_modified.c_str());
    notes = elem.attribute(L"notes").as_string(notes.c_str());

    return true;
}

void Data::Document_Info::log() const
{
    std::wcout << L"# Document Info >> ";
    std::wcout << L"author=" << author << L" | ";
    std::wcout << L"copyright=" << copyright << L" | ";
    std::wcout << L"license=" << license << L" | ";
    std::wcout << L"version=" << version << L" | ";
    std::wcout << L"last_modified=" << last_modified << L" | ";
    std::wcout << L"notes=" << notes << std::endl;;
}

void Data::Document_Info::clear()
{
    author = L"Unknown";
    copyright = L"Unknown";
    license = L"Unknown";
    version = L"Unknown";
    last_modified = L"Unknown";
    notes = L"";
}
