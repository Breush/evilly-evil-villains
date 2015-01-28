#include "scml/data.hpp"

#include "scml/entityprototype.hpp"
#include "tools/tools.hpp"
#include "tools/debug.hpp"

#include <pugixml.hpp>
#include <cstdarg> // va_start

using namespace scml;

Data::Data()
    : pixel_art_mode(false)
    , meta_data(nullptr)
    , generator("Unknown")
    , generator_version("Unkonwn")
{}

Data::Data(const std::string& file)
    : pixel_art_mode(false)
    , meta_data(nullptr)
    , generator("Unknown")
    , generator_version("Unkonwn")
{
    load(file);
}

Data::Data(pugi::xml_node& elem)
    : pixel_art_mode(false)
    , meta_data(nullptr)
    , generator("Unknown")
    , generator_version("Unknown")
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

    auto root = doc.child("spriter_data");
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

    scml_version = elem.attribute("scml_version").as_string(scml_version.c_str());
    generator = elem.attribute("generator").as_string(generator.c_str());
    generator_version = elem.attribute("generator_version").as_string(generator_version.c_str());
    pixel_art_mode = elem.attribute("pixel_art_mode").as_bool(pixel_art_mode);
    debug_scml(log());

    // Meta data
    auto meta_data_child = elem.child("meta_data");
    if (meta_data_child) {
        if (meta_data == nullptr)
            meta_data = new Meta_Data;
        meta_data->load(meta_data_child);
    }

    // Folder
    for (auto& child : elem.children("folder")) {
        Folder* folder = new Folder;
        if (folder->load(child)) {
            if (!folders.insert(std::make_pair(folder->id, folder)).second) {
                throw std::runtime_error("scml::Data loaded a folder with the duplicate id " + folder->id);
                delete folder;
            }
        } else {
            throw std::runtime_error("scml::Data failed to load a folder");
            delete folder;
        }
    }

    // Atlas
    for (auto& child : elem.children("atlas")) {
        Atlas* atlas = new Atlas;
        if (atlas->load(child)) {
            if (!atlases.insert(std::make_pair(atlas->id, atlas)).second) {
                throw std::runtime_error("scml::Data loaded an atlas with the duplicate id " + atlas->id);
                delete atlas;
            }
        } else {
            throw std::runtime_error("scml::Data failed to load an atlas");
            delete atlas;
        }
    }

    // Entity
    for (auto& child : elem.children("entity")) {
        Entity* entity = new Entity;
        if (entity->load(child)) {
            if (!entities.insert(std::make_pair(entity->id, entity)).second) {
                throw std::runtime_error("scml::Data loaded an entity with the duplicate id " + entity->id);
                delete entity;
            }
        } else {
            throw std::runtime_error("scml::Data failed to load an entity");
            delete entity;
        }
    }

    // Character_map
    for (auto& child : elem.children("character_map")) {
        Character_Map* character_map = new Character_Map;
        if (character_map->load(child)) {
            if (!character_maps.insert(std::make_pair(character_map->id, character_map)).second) {
                throw std::runtime_error("scml::Data loaded a character_map with the duplicate id " + character_map->id);
                delete character_map;
            }
        } else {
            throw std::runtime_error("scml::Data failed to load a character_map");
            delete character_map;
        }
    }

    // Document information
    auto document_info_elem = elem.child("document_info");
    if (document_info_elem) {
        document_info.load(document_info_elem);
    }

    return true;
}

void Data::log() const
{
    std::cout << "# Document >> ";
    std::cout << "scml_version=" << scml_version << " | ";
    std::cout << "generator=" << generator << " | ";
    std::cout << "generator_version=" << generator_version << " | ";
    std::cout << "pixel_art_mode=" << pixel_art_mode << std::endl;
}

void Data::clear()
{
    scml_version = "";
    generator = "(Spriter)";
    generator_version = "(1.0)";
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
    for (auto& child : elem.children("variable")) {
        Variable* variable = new Variable;
        if (variable->load(child)) {
            if (!variables.insert(std::make_pair(variable->name, variable)).second) {
                throw std::runtime_error("scml::Data::Meta_Data loaded a variable with the duplicate name " + variable->name);
                delete variable;
            }
        } else {
            throw std::runtime_error("scml::Data::Meta_Data failed to load a variable.");
            delete variable;
        }
    }

    // Tag
    for (auto& child : elem.children("tag")) {
        Tag* tag = new Tag;
        if (tag->load(child)) {
            if (!tags.insert(std::make_pair(tag->name, tag)).second) {
                throw std::runtime_error("scml::Data::Meta_Data loaded a tag with a duplicate name " + tag->name);
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
    : name("")
    , type("string")
    , value_int(0)
    , value_float(0.0f)
    , value_string("")
{}

Data::Meta_Data::Variable::Variable(pugi::xml_node& elem)
    : name("")
    , type("string")
    , value_int(0)
    , value_float(0.0f)
    , value_string("")
{
    load(elem);
}

bool Data::Meta_Data::Variable::load(pugi::xml_node& elem)
{
    name = elem.attribute("name").as_string(name.c_str());
    type = elem.attribute("type").as_string(type.c_str());

    if (type == "string") value_string = elem.attribute("value").as_string(value_string.c_str());
    else if (type == "int") value_int = elem.attribute("value").as_int(value_int);
    else if (type == "float")value_float = elem.attribute("value").as_float(value_float);
    else throw std::runtime_error("Data::Meta_Data::Variable loaded invalid variable type " + type + " named " + name);

    //debug_scml(log());

    return true;
}

void Data::Meta_Data::Variable::log() const
{
    std::cout << "# Meta data :: Variable >> ";
    std::cout << "name=" << name << " | ";
    std::cout << "type=" << type << std::endl;
    if (type == "string")     std::cout << "value=" << value_string << std::endl;
    else if (type == "int")   std::cout << "value=" << value_int << std::endl;
    else if (type == "float") std::cout << "value=" << value_float << std::endl;
}

void Data::Meta_Data::Variable::clear()
{
    name.clear();
    type = "string";
    value_string.clear();
    value_int = 0;
    value_float = 0.0f;
}

//---------------//
//----- Tag -----//

Data::Meta_Data::Tag::Tag()
    : name("")
{}

Data::Meta_Data::Tag::Tag(pugi::xml_node& elem)
    : name("")
{
    load(elem);
}

bool Data::Meta_Data::Tag::load(pugi::xml_node& elem)
{
    name = elem.attribute("name").as_string(name.c_str());
    //debug_scml(log());

    return true;
}

void Data::Meta_Data::Tag::log() const
{
    std::cout << "# Meta data :: Tag >> ";
    std::cout << "name=" << name << std::endl;
}

void Data::Meta_Data::Tag::clear()
{
    name.clear();
}

//------------------//
//----- Folder -----//

Data::Folder::Folder()
    : id(0)
    , name("")
{}

Data::Folder::Folder(pugi::xml_node& elem)
    : id(0)
    , name("")
{
    load(elem);
}

Data::Folder::~Folder()
{
    clear();
}

bool Data::Folder::load(pugi::xml_node& elem)
{
    id = elem.attribute("id").as_int(id);
    name = elem.attribute("name").as_string(name.c_str());
    debug_scml(log());

    // File
    for (auto& child : elem.children("file")) {
        File* file = new File;
        if (file->load(child)) {
            if (!files.insert(std::make_pair(file->id, file)).second) {
                throw std::runtime_error("scml::Data::Folder loaded a file with the duplicated id " + file->id);
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
    std::cout << "# Folder >> ";
    std::cout << "id=" << id << " | ";
    std::cout << "name=" << name << std::endl;
}

void Data::Folder::clear()
{
    id = 0;
    name.clear();

    for (auto const& file : files) {
        delete file.second;
    }
    files.clear();
}

//----------------//
//----- File -----//

Data::Folder::File::File()
    : id(0)
    , name("")
    , type("image")
    , pivot_x(0.f), pivot_y(1.f)
    , atlas_x(0), atlas_y(0)
    , offset_x(0), offset_y(0)
    , width(0), height(0)
    , original_width(0), original_height(0)
{}

Data::Folder::File::File(pugi::xml_node& elem)
    : id(0)
    , name("")
    , type("image")
    , pivot_x(0.f), pivot_y(1.f)
    , atlas_x(0), atlas_y(0)
    , offset_x(0), offset_y(0)
    , width(0), height(0)
    , original_width(0), original_height(0)
{
    load(elem);
}

bool Data::Folder::File::load(pugi::xml_node& elem)
{
    type = elem.attribute("type").as_string(type.c_str());
    id = elem.attribute("id").as_int(id);
    name = elem.attribute("name").as_string(name.c_str());
    pivot_x = elem.attribute("pivot_x").as_float(pivot_x);
    pivot_y = elem.attribute("pivot_y").as_float(pivot_y);
    width = elem.attribute("width").as_int(width);
    height = elem.attribute("height").as_int(height);
    atlas_x = elem.attribute("atlas_x").as_int(atlas_x);
    atlas_y = elem.attribute("atlas_y").as_int(atlas_y);
    offset_x = elem.attribute("offset_x").as_int(offset_x);
    offset_y = elem.attribute("offset_y").as_int(offset_y);
    original_width = elem.attribute("original_width").as_int(original_width);
    original_height = elem.attribute("original_height").as_int(original_height);
    debug_scml(log());

    return true;
}

void Data::Folder::File::log() const
{
    std::cout << "# Folder :: File >> ";
    std::cout << "type=" << type << " | ";
    std::cout << "id=" << id << " | ";
    std::cout << "name=" << name << " | ";
    std::cout << "pivot_x=" << pivot_x << " | ";
    std::cout << "pivot_y=" << pivot_y << " | ";
    std::cout << "width=" << width << " | ";
    std::cout << "height=" << height << " | ";
    std::cout << "atlas_x=" << atlas_x << " | ";
    std::cout << "atlas_y=" << atlas_y << " | ";
    std::cout << "offset_x=" << offset_x << " | ";
    std::cout << "offset_y=" << offset_y << " | ";
    std::cout << "original_width=" << original_width << " | ";
    std::cout << "original_height=" << original_height << std::endl;
}

void Data::Folder::File::clear()
{
    type = "image";
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
    : id(0)
    , data_path("")
    , image_path("")
{}

Data::Atlas::Atlas(pugi::xml_node& elem)
    : id(0)
    , data_path("")
    , image_path("")
{
    load(elem);
}

Data::Atlas::~Atlas()
{
    clear();
}

bool Data::Atlas::load(pugi::xml_node& elem)
{
    id = elem.attribute("id").as_int(id);
    data_path = elem.attribute("data_path").as_string(data_path.c_str());
    image_path = elem.attribute("image_path").as_string(image_path.c_str());
    //debug_scml(log());

    // Folder
    for (auto& child : elem.children("folder")) {
        Folder* folder = new Folder;
        if (folder->load(child)) {
            if (!folders.insert(std::make_pair(folder->id, folder)).second) {
                throw std::runtime_error("scml::Data::Atlas loaded a folder with the duplicated id " + folder->id);
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
    std::cout << "# Atlas >> ";
    std::cout << "id=" << id << " | ";
    std::cout << "data_path=" << data_path << " | ";
    std::cout << "image_path=" << image_path << std::endl;
}

void Data::Atlas::clear()
{
    id = 0;
    data_path.clear();
    image_path.clear();

    for (auto const& folder : folders) {
        delete folder.second;
    }
    folders.clear();
}

//------------------------//
//----- Atlas Folder -----//

Data::Atlas::Folder::Folder()
    : id(0)
    , name("")
{}

Data::Atlas::Folder::Folder(pugi::xml_node& elem)
    : id(0)
    , name("")
{
    load(elem);
}

bool Data::Atlas::Folder::load(pugi::xml_node& elem)
{
    id = elem.attribute("id").as_int(id);
    name = elem.attribute("name").as_string(name.c_str());

    for (auto& child : elem.children("image")) {
        Image* image = new Image;
        if (image->load(child)) {
            if (!images.insert(std::make_pair(image->id, image)).second) {
                throw std::runtime_error("scml::Data::Atlas::Folder loaded an image with the duplicated id " + image->id);
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
    std::cout << "# Atlas :: Folder" << " | ";
    std::cout << "id=" << id << " | ";
    std::cout << "name=" << name << std::endl;
}

void Data::Atlas::Folder::clear()
{
    id = 0;
    name.clear();

    for (auto const& image : images) {
        delete image.second;
    }
    images.clear();
}

//-----------------//
//----- Image -----//

Data::Atlas::Folder::Image::Image()
    : id(0)
    , full_path("")
{}

Data::Atlas::Folder::Image::Image(pugi::xml_node& elem)
    : id(0)
    , full_path("")
{
    load(elem);
}

bool Data::Atlas::Folder::Image::load(pugi::xml_node& elem)
{
    id = elem.attribute("id").as_int(id);
    full_path = elem.attribute("full_path").as_string(full_path.c_str());
    debug_scml(log());

    return true;
}

void Data::Atlas::Folder::Image::log() const
{
    std::cout << "# Atlas :: Folder :: Image" << " | ";
    std::cout << "id=" << id << " | ";
    std::cout << "full_path=" << full_path << std::endl;
}

void Data::Atlas::Folder::Image::clear()
{
    id = 0;
    full_path.clear();
}

//------------------//
//----- Entity -----//

Data::Entity::Entity()
    : id(0)
    , name("")
    , meta_data(nullptr)
{}

Data::Entity::Entity(pugi::xml_node& elem)
    : id(0)
    , name("")
    , meta_data(nullptr)
{
    load(elem);
}

Data::Entity::~Entity()
{
    clear();
}

bool Data::Entity::load(pugi::xml_node& elem)
{
    id = elem.attribute("id").as_int(id);
    name = elem.attribute("name").as_string(name.c_str());

    // Meta data
    auto meta_data_child = elem.child("meta_data");
    if (meta_data_child) {
        if (meta_data == nullptr) {
            meta_data = new Meta_Data;
        }
        meta_data->load(meta_data_child);
    }

    // Animation
    for (auto& child : elem.children("animation")) {
        Animation* animation = new Animation;
        if (animation->load(child)) {
            if (!animations.insert(std::make_pair(animation->id, animation)).second) {
                throw std::runtime_error("scml::Data::Entity loaded an animation with the duplicated id " + animation->id);
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
    std::cout << "# Entity" << " | ";
    std::cout << "id=" << id << " | ";
    std::cout << "name=" << name << std::endl;
}

void Data::Entity::clear()
{
    id = 0;
    name.clear();
    delete meta_data;
    meta_data = nullptr;

    for (auto const& animation : animations) {
        delete animation.second;
    }
    animations.clear();
}

//---------------------//
//----- Animation -----//

Data::Entity::Animation::Animation()
    : id(0)
    , name("")
    , length(0)
    , looping("true")
    , loop_to(0)
    , meta_data(nullptr)
{}

Data::Entity::Animation::Animation(pugi::xml_node& elem)
    : id(0)
    , name("")
    , length(0)
    , looping("true")
    , loop_to(0)
    , meta_data(nullptr)
{
    load(elem);
}

bool Data::Entity::Animation::load(pugi::xml_node& elem)
{
    id = elem.attribute("id").as_int(id);
    name = elem.attribute("name").as_string(name.c_str());
    length = elem.attribute("length").as_int(length);
    looping = elem.attribute("looping").as_string(looping.c_str());
    loop_to = elem.attribute("loop_to").as_int(loop_to);
    //debug_scml(log());

    // Meta data
    auto meta_data_child = elem.child("meta_data");
    if (meta_data_child) {
        if (meta_data == nullptr)
            meta_data = new Meta_Data;
        meta_data->load(meta_data_child);
    }

    // Mainline
    auto mainline_elem = elem.child("mainline");
    if (!mainline_elem || !mainline.load(mainline_elem)) {
        throw std::runtime_error("scml::Data::Entity::Animation failed to load the mainline.");
        mainline.clear();
    }

    // Timeline
    for (auto& child : elem.children("timeline")) {
        Timeline* timeline = new Timeline;
        if (timeline->load(child)) {
            if (!timelines.insert(std::make_pair(timeline->id, timeline)).second) {
                throw std::runtime_error("scml::Data::Entity::Animation loaded a timeline with the duplicated id " + timeline->id);
                delete timeline;
            }
        } else {
            throw std::runtime_error("scml::Data::Entity::Animation failed to load a timeline.");
            delete timeline;
        }
    }

    // Soundline
    auto soundline_elem = elem.child("soundline");
    if (!soundline_elem || !soundline.load(soundline_elem)) {
        throw std::runtime_error("scml::Data::Entity::Animation failed to load the soundline.");
        soundline.clear();
    }

    return true;
}

void Data::Entity::Animation::log() const
{
    std::cout << "# Entity:: Animation" << " | ";
    std::cout << "id=" << id << " | ";
    std::cout << "name=" << name << " | ";
    std::cout << "length=" << length << " | ";
    std::cout << "looping=" << looping << " | ";
    std::cout << "loop_to=" << loop_to << std::endl;
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
    looping = "true";
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
    for (auto& child : elem.children("key")) {
        Key* key = new Key;
        if (key->load(child)) {
            if (!keys.insert(std::make_pair(key->id, key)).second) {
                throw std::runtime_error("scml::Data::Entity::Animation::Mainline loaded a key with the duplicated id " + key->id);
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
    id = elem.attribute("id").as_int(id);
    time = elem.attribute("time").as_int(time);
    //debug_scml(log());

    // Meta data
    auto meta_data_child = elem.child("meta_data");
    if (meta_data_child) {
        if (meta_data == nullptr)
            meta_data = new Meta_Data;
        meta_data->load(meta_data_child);
    }

    // Bone
    for (auto& child : elem.children("bone")) {
        Bone* bone = new Bone;
        if (bone->load(child)) {
            if (!bones.insert(std::make_pair(bone->id, bone)).second) {
                throw std::runtime_error("scml::Data::Entity::Animation::Mainline::Key loaded a bone with the duplicated id " + bone->id);
                delete bone;
            }
        } else {
            throw std::runtime_error("scml::Data::Entity::Animation::Mainline::Key failed to load a bone");
            delete bone;
        }
    }

    // Bone_ref
    for (auto& child : elem.children("bone_ref")) {
        Bone_Ref* bone_ref = new Bone_Ref;
        if (bone_ref->load(child)) {
            if (!bones.insert(std::make_pair(bone_ref->id, Bone_Container(bone_ref))).second) {
                throw std::runtime_error("scml::Data::Entity::Animation::Mainline::Key loaded a bone_ref with the duplicated id " + bone_ref->id);
                delete bone_ref;
            }
        } else {
            throw std::runtime_error("scml::Data::Entity::Animation::Mainline::Key failed to load a bone_ref");
            delete bone_ref;
        }
    }

    // Object
    for (auto& child : elem.children("object")) {
        Object* object = new Object;
        if (object->load(child)) {
            if (!objects.insert(std::make_pair(object->id, object)).second) {
                throw std::runtime_error("scml::Data::Entity::Animation::Mainline::Key loaded an object with the duplicated id " + object->id);
                delete object;
            }
        } else {
            throw std::runtime_error("scml::Data::Entity::Animation::Mainline::Key failed to load an object");
            delete object;
        }
    }

    // Object_ref
    for (auto& child : elem.children("object_ref")) {
        Object_Ref* object_ref = new Object_Ref;
        if (object_ref->load(child)) {
            if (!objects.insert(std::make_pair(object_ref->id, Object_Container(object_ref))).second) {
                throw std::runtime_error("scml::Data::Entity::Animation::Mainline::Key loaded an object_ref with the duplicated id " + object_ref->id);
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
    std::cout << "# Entity :: Animation :: Mainline :: Key" << " | ";
    std::cout << "id=" << id << " | ";
    std::cout << "time=" << time << std::endl;
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
    id = elem.attribute("id").as_int(id);
    parent = elem.attribute("parent").as_int(parent);
    x = elem.attribute("x").as_float(x);
    y = elem.attribute("y").as_float(y);
    angle = elem.attribute("angle").as_float(angle);
    scale_x = elem.attribute("scale_x").as_float(scale_x);
    scale_y = elem.attribute("scale_y").as_float(scale_y);
    r = elem.attribute("r").as_float(r);
    g = elem.attribute("g").as_float(g);
    b = elem.attribute("b").as_float(b);
    a = elem.attribute("a").as_float(a);

    auto meta_data_child = elem.child("meta_data");
    if (meta_data_child) {
        if (meta_data == nullptr)
            meta_data = new Meta_Data;
        meta_data->load(meta_data_child);
    }

    return true;
}

void Data::Entity::Animation::Mainline::Key::Bone::log() const
{
    std::cout << "id=" << id << " | ";
    std::cout << "parent=" << parent << " | ";
    std::cout << "x=" << x << " | ";
    std::cout << "y=" << y << " | ";
    std::cout << "angle=" << angle << " | ";
    std::cout << "scale_x=" << scale_x << " | ";
    std::cout << "scale_y=" << scale_y << " | ";
    std::cout << "r=" << r << " | ";
    std::cout << "g=" << g << " | ";
    std::cout << "b=" << b << " | ";
    std::cout << "a=" << a << std::endl;
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
    id = elem.attribute("id").as_int(id);
    parent = elem.attribute("parent").as_int(parent);
    timeline = elem.attribute("timeline").as_int(timeline);
    key = elem.attribute("key").as_int(key);
    //debug_scml(log());

    return true;
}

void Data::Entity::Animation::Mainline::Key::Bone_Ref::log() const
{
    std::cout << "# Entity :: Mainline :: Key :: Bone_Ref >> ";
    std::cout << "id=" << id << " | ";
    std::cout << "parent=" << parent << " | ";
    std::cout << "timeline=" << timeline << " | ";
    std::cout << "key=" << key << std::endl;

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
    , object_type("sprite")
    , atlas(0), folder(0), file(0)
    , usage("display")
    , blend_mode("alpha")
    , x(0.0f), y(0.0f)
    , pivot_x(0.0f), pivot_y(1.0f)
    , pixel_art_mode_x(0), pixel_art_mode_y(0)
    , pixel_art_mode_pivot_x(0), pixel_art_mode_pivot_y(0)
    , angle(0.0f)
    , w(0.0f), h(0.0f)
    , scale_x(1.0f), scale_y(1.0f)
    , r(1.0f), g(1.0f), b(1.0f), a(1.0f)
    , variable_type("string")
    , value_string("")
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
    , object_type("sprite")
    , atlas(0), folder(0), file(0)
    , usage("display")
    , blend_mode("alpha")
    , x(0.0f), y(0.0f)
    , pivot_x(0.0f), pivot_y(1.0f)
    , pixel_art_mode_x(0), pixel_art_mode_y(0)
    , pixel_art_mode_pivot_x(0), pixel_art_mode_pivot_y(0)
    , angle(0.0f)
    , w(0.0f), h(0.0f)
    , scale_x(1.0f), scale_y(1.0f)
    , r(1.0f), g(1.0f), b(1.0f), a(1.0f)
    , variable_type("string")
    , value_string("")
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
    id = elem.attribute("id").as_int(id);
    parent = elem.attribute("parent").as_int(parent);
    object_type = elem.attribute("object_type").as_string(object_type.c_str());
    atlas = elem.attribute("atlas").as_int(atlas);
    folder = elem.attribute("folder").as_int(folder);
    file = elem.attribute("file").as_int(file);
    usage = elem.attribute("usage").as_string(usage.c_str());
    blend_mode = elem.attribute("blend_mode").as_string(blend_mode.c_str());
    x = elem.attribute("x").as_float(x);
    y = elem.attribute("y").as_float(y);
    pivot_x = elem.attribute("pivot_x").as_float(pivot_x);
    pivot_y = elem.attribute("pivot_y").as_float(pivot_y);
    pixel_art_mode_x = elem.attribute("x").as_int(pixel_art_mode_x);
    pixel_art_mode_y = elem.attribute("y").as_int(pixel_art_mode_y);
    pixel_art_mode_pivot_x = elem.attribute("pivot_x").as_int(pixel_art_mode_pivot_x);
    pixel_art_mode_pivot_y = elem.attribute("pivot_y").as_int(pixel_art_mode_pivot_y);
    angle = elem.attribute("angle").as_float(angle);
    w = elem.attribute("w").as_float(w);
    h = elem.attribute("h").as_float(h);
    scale_x = elem.attribute("scale_x").as_float(scale_x);
    scale_y = elem.attribute("scale_y").as_float(scale_y);
    r = elem.attribute("r").as_float(r);
    g = elem.attribute("g").as_float(g);
    b = elem.attribute("b").as_float(b);
    a = elem.attribute("a").as_float(a);
    variable_type = elem.attribute("variable_type").as_string(variable_type.c_str());
    //debug_scml(log());

    if (variable_type == "string") {
        value_string = elem.attribute("value").as_string(value_string.c_str());
    } else if (variable_type == "int") {
        value_int = elem.attribute("value").as_int(value_int);
        min_int = elem.attribute("min").as_int(min_int);
        max_int = elem.attribute("max").as_int(max_int);
    } else if (variable_type == "float") {
        value_float = elem.attribute("value").as_float(value_float);
        min_float = elem.attribute("min").as_float(min_float);
        max_float = elem.attribute("max").as_float(max_float);
    }

    animation = elem.attribute("animation").as_int(animation);
    t = elem.attribute("t").as_float(t);
    z_index = elem.attribute("z_index").as_int(z_index);

    if (object_type == "sound") {
        volume = elem.attribute("volume").as_float(volume);
        panning = elem.attribute("panning").as_float(panning);
    }

    // Meta data
    auto meta_data_child = elem.child("meta_data");
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
    std::cout << "# Entity :: Animation :: Mainline :: Key :: Object >> ";
    std::cout << "id=" << id << " | ";
    std::cout << "parent=" << parent << " | ";
    std::cout << "object_type=" << object_type << " | ";
    std::cout << "atlas=" << atlas << " | ";
    std::cout << "folder=" << folder << " | ";
    std::cout << "file=" << file << " | ";
    std::cout << "usage=" << usage << " | ";
    std::cout << "blend_mode=" << blend_mode << " | ";
    std::cout << "x=" << x << " | ";
    std::cout << "y=" << y << " | ";
    std::cout << "pivot_x=" << pivot_x << " | ";
    std::cout << "pivot_y=" << pivot_y << " | ";
    std::cout << "pixel_art_mode_x=" << pixel_art_mode_x << " | ";
    std::cout << "pixel_art_mode_y=" << pixel_art_mode_y << " | ";
    std::cout << "pixel_art_mode_pivot_x=" << pixel_art_mode_pivot_x << " | ";
    std::cout << "pixel_art_mode_pivot_y=" << pixel_art_mode_pivot_y << " | ";
    std::cout << "angle=" << angle << " | ";
    std::cout << "w=" << w << " | ";
    std::cout << "h=" << h << " | ";
    std::cout << "scale_x=" << scale_x << " | ";
    std::cout << "scale_y=" << scale_y << " | ";
    std::cout << "r=" << r << " | ";
    std::cout << "g=" << g << " | ";
    std::cout << "b=" << b << " | ";
    std::cout << "a=" << a << " | ";
    std::cout << "variable_type=" << variable_type << std::endl;
}

void Data::Entity::Animation::Mainline::Key::Object::clear()
{
    id = 0;
    parent = -1;
    object_type = "sprite";
    atlas = 0;
    folder = 0;
    file = 0;
    usage = "display";
    blend_mode = "alpha";
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
    variable_type = "string";
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
    id = elem.attribute("id").as_int(id);
    parent = elem.attribute("parent").as_int(parent);
    timeline = elem.attribute("timeline").as_int(timeline);
    key = elem.attribute("key").as_int(key);
    z_index = elem.attribute("z_index").as_int(z_index);
    //debug_scml(log());

    abs_x = elem.attribute("abs_x").as_float(abs_x);
    abs_y = elem.attribute("abs_y").as_float(abs_y);
    abs_pivot_x = elem.attribute("abs_pivot_x").as_float(abs_pivot_x);
    abs_pivot_y = elem.attribute("abs_pivot_y").as_float(abs_pivot_y);
    abs_angle = elem.attribute("abs_angle").as_float(abs_angle);
    abs_scale_x = elem.attribute("abs_scale_x").as_float(abs_scale_x);
    abs_scale_y = elem.attribute("abs_scale_y").as_float(abs_scale_y);
    abs_a = elem.attribute("abs_a").as_float(abs_a);

    return true;
}

void Data::Entity::Animation::Mainline::Key::Object_Ref::log() const
{
    std::cout << "# Entity :: Animation :: Mainline :: Key >> ";
    std::cout << "id=" << id << " | ";
    std::cout << "parent=" << parent << " | ";
    std::cout << "timeline=" << timeline << " | ";
    std::cout << "key=" << key << " | ";
    std::cout << "z_index=" << z_index << std::endl;

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
    , name("")
    , object_type("sprite")
    , variable_type("string")
    , usage("display")
    , meta_data(nullptr)
{}

Data::Entity::Animation::Timeline::Timeline(pugi::xml_node& elem)
    : id(0)
    , name("")
    , object_type("sprite")
    , variable_type("string")
    , usage("display")
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
    id = elem.attribute("id").as_int(id);
    object_type = elem.attribute("object_type").as_string(object_type.c_str());
    variable_type = elem.attribute("variable_type").as_string(variable_type.c_str());
    //debug_scml(log());

    if (object_type != "sound") name = elem.attribute("name").as_string(name.c_str());

    if (object_type == "point") usage = elem.attribute("usage").as_string("neither");
    else if (object_type == "box") usage = elem.attribute("usage").as_string("sprite");
    else if (object_type == "sprite") usage = elem.attribute("usage").as_string("display");
    else if (object_type == "entity") usage = elem.attribute("usage").as_string("display");

    // Meta data
    auto meta_data_child = elem.child("meta_data");
    if (meta_data_child) {
        if (meta_data == nullptr)
            meta_data = new Meta_Data;
        meta_data->load(meta_data_child);
    }

    for (auto& child : elem.children("key")) {
        Key* key = new Key;
        if (key->load(child)) {
            if (!keys.insert(std::make_pair(key->id, key)).second) {
                throw std::runtime_error("scml::Data::Entity::Animation::Timeline loaded a key with the duplicated id " + key->id);
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
    std::cout << "# Entity :: Animation :: Timeline >> ";
    std::cout << "id=" << id << " | ";
    std::cout << "name=" << name << " | ";
    std::cout << "object_type=" << object_type << " | ";
    std::cout << "variable_type=" << variable_type << " | ";
    std::cout << "usage=" << usage << std::endl;
}

void Data::Entity::Animation::Timeline::clear()
{
    id = 0;
    name.clear();
    object_type = "sprite";
    variable_type = "string";
    usage = "display";

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
    , curve_type("linear")
    , c1(0.0f), c2(0.0f)
    , spin(1)
    , meta_data(nullptr)
{}

Data::Entity::Animation::Timeline::Key::Key(pugi::xml_node& elem)
    : id(0)
    , time(0)
    , curve_type("linear")
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
    id = elem.attribute("id").as_int(id);
    time = elem.attribute("time").as_int(time);
    curve_type = elem.attribute("curve_type").as_string(curve_type.c_str());
    c1 = elem.attribute("c1").as_float(c1);
    c2 = elem.attribute("c2").as_float(c2);
    spin = elem.attribute("spin").as_int(spin);
    //debug_scml(log());

    // Meta data tweenable
    auto meta_data_child = elem.child("meta_data");
    if (meta_data_child) {
        if (meta_data == nullptr)
            meta_data = new Meta_Data_Tweenable;
        meta_data->load(meta_data_child);
    }

    has_object = true;

    // Bone
    auto child = elem.child("bone");
    if (child) {
        has_object = false;
        if (!bone.load(child)) {
            throw std::runtime_error("scml::Data::Entity::Animation::Timeline::Key failed to load a bone.");
        }
    }

    // Object
    child = elem.child("object");
    if (child != nullptr && !object.load(child)) {
        throw std::runtime_error("scml::Data::Entity::Animation::Timeline::Key failed to load an object.");
        has_object = true;
    }

    return true;
}

void Data::Entity::Animation::Timeline::Key::log() const
{
    std::cout << "# Entity :: Animation :: Timeline :: Key >> ";
    std::cout << "id=" << id << " | ";
    std::cout << "time=" << time << " | ";
    std::cout << "curve_type=" << curve_type << " | ";
    std::cout << "c1=" << c1 << " | ";
    std::cout << "c2=" << c2 << " | ";
    std::cout << "spin=" << spin << std::endl;
}

void Data::Entity::Animation::Timeline::Key::clear()
{
    id = 0;
    time = 0;
    curve_type = "linear";
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
    for (auto& child : elem.children("variable")) {
        Variable* variable = new Variable;
        if (variable->load(child)) {
            if (!variables.insert(std::make_pair(variable->name, variable)).second) {
                throw std::runtime_error("scml::Data::Meta_Data_Tweenable loaded a variable with the duplicate name " + variable->name);
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
    : type("string")
    , value_string("")
    , value_int(0)
    , value_float(0.0f)
    , curve_type("linear")
    , c1(0.0f)
    , c2(0.0f)
{}

Data::Meta_Data_Tweenable::Variable::Variable(pugi::xml_node& elem)
    : type("string")
    , value_string("")
    , value_int(0)
    , value_float(0.0f)
    , curve_type("linear")
    , c1(0.0f)
    , c2(0.0f)
{
    load(elem);
}

bool Data::Meta_Data_Tweenable::Variable::load(pugi::xml_node& elem)
{
    type = elem.attribute("type").as_string(type.c_str());

    if (type == "string") value_string = elem.attribute("value").as_string(value_string.c_str());
    else if (type == "int") value_int = elem.attribute("value").as_int(value_int);
    else if (type == "float") value_float = elem.attribute("value").as_float(value_float);

    curve_type = elem.attribute("curve_type").as_string(curve_type.c_str());
    c1 = elem.attribute("c1").as_float(c1);
    c2 = elem.attribute("c2").as_float(c2);
    //debug_scml(log());

    return true;
}

void Data::Meta_Data_Tweenable::Variable::log() const
{
    std::cout << "# Meta_Data_Tweenable :: Variable >> ";
    std::cout << "type=" << type << std::endl;
    if (type == "string") std::cout << "value=" << value_string << std::endl;
    else if (type == "int") std::cout << "value=" << value_int << std::endl;
    else if (type == "float") std::cout << "value=" << value_float << std::endl;

    std::cout << "curve_type=" << curve_type << " | ";
    std::cout << "c1=" << c1 << " | ";
    std::cout << "c2=" << c2 << std::endl;
}

void Data::Meta_Data_Tweenable::Variable::clear()
{
    type = "string";
    value_string.clear();
    value_int = 0;
    value_float = 0.0f;

    curve_type = "linear";
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
    x = elem.attribute("x").as_float(x);
    y = elem.attribute("y").as_float(y);
    angle = elem.attribute("angle").as_float(angle);
    scale_x = elem.attribute("scale_x").as_float(scale_x);
    scale_y = elem.attribute("scale_y").as_float(scale_y);
    r = elem.attribute("r").as_float(r);
    g = elem.attribute("g").as_float(g);
    b = elem.attribute("b").as_float(b);
    a = elem.attribute("a").as_float(a);

    // Meta data
    auto meta_data_child = elem.child("meta_data");
    if (meta_data_child) {
        if (meta_data == nullptr)
            meta_data = new Meta_Data_Tweenable;
        meta_data->load(meta_data_child);
    }

    return true;
}

void Data::Entity::Animation::Timeline::Key::Bone::log() const
{
    std::cout << "x=" << x << " | ";
    std::cout << "y=" << y << " | ";
    std::cout << "angle=" << angle << " | ";
    std::cout << "scale_x=" << scale_x << " | ";
    std::cout << "scale_y=" << scale_y << " | ";
    std::cout << "r=" << r << " | ";
    std::cout << "g=" << g << " | ";
    std::cout << "b=" << b << " | ";
    std::cout << "a=" << a << std::endl;
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
    , blend_mode("alpha")
    , value_string("")
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
    , blend_mode("alpha")
    , value_string("")
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
    atlas = elem.attribute("atlas").as_int(atlas);
    folder = elem.attribute("folder").as_int(folder);
    file = elem.attribute("file").as_int(file);
    x = elem.attribute("x").as_float(x);
    y = elem.attribute("y").as_float(y);
    pivot_x = elem.attribute("pivot_x").as_float(pivot_x);
    pivot_y = elem.attribute("pivot_y").as_float(pivot_y);
    angle = elem.attribute("angle").as_float(angle);
    w = elem.attribute("w").as_float(w);
    h = elem.attribute("h").as_float(h);
    scale_x = elem.attribute("scale_x").as_float(scale_x);
    scale_y = elem.attribute("scale_y").as_float(scale_y);
    r = elem.attribute("r").as_float(r);
    g = elem.attribute("g").as_float(g);
    b = elem.attribute("b").as_float(b);
    a = elem.attribute("a").as_float(a);
    blend_mode = elem.attribute("blend_mode").as_string(blend_mode.c_str());
    value_string = elem.attribute("value").as_string(value_string.c_str());
    value_int = elem.attribute("value").as_int(value_int);
    min_int = elem.attribute("min").as_int(min_int);
    max_int = elem.attribute("max").as_int(max_int);
    value_float = elem.attribute("value").as_float(value_float);
    min_float = elem.attribute("min").as_float(min_float);
    max_float = elem.attribute("max").as_float(max_float);
    animation = elem.attribute("animation").as_int(animation);
    t = elem.attribute("t").as_float(t);
    volume = elem.attribute("volume").as_float(volume);
    panning = elem.attribute("panning").as_float(panning);
    //debug_scml(log());

    // Meta data
    auto meta_data_child = elem.child("meta_data");
    if (meta_data_child) {
        if (meta_data == nullptr)
            meta_data = new Meta_Data_Tweenable;
        meta_data->load(meta_data_child);
    }

    return true;
}

void Data::Entity::Animation::Timeline::Key::Object::log() const
{
    std::cout << "# Entity :: Animation :: Timeline :: Key :: Object >> ";
    std::cout << "atlas=" << atlas << " | ";
    std::cout << "folder=" << folder << " | ";
    std::cout << "file=" << file << " | ";
    std::cout << "x=" << x << " | ";
    std::cout << "y=" << y << " | ";
    std::cout << "pivot_x=" << pivot_x << " | ";
    std::cout << "pivot_y=" << pivot_y << " | ";
    std::cout << "angle=" << angle << " | ";
    std::cout << "w=" << w << " | ";
    std::cout << "h=" << h << " | ";
    std::cout << "scale_x=" << scale_x << " | ";
    std::cout << "scale_y=" << scale_y << " | ";
    std::cout << "r=" << r << " | ";
    std::cout << "g=" << g << " | ";
    std::cout << "b=" << b << " | ";
    std::cout << "a=" << a << " | ";
    std::cout << "blend_mode=" << blend_mode << " | ";
    std::cout << "value=" << value_string << " | ";
    std::cout << "animation=" << animation << " | ";
    std::cout << "t=" << t << " | ";
    std::cout << "volume=" << volume << " | ";
    std::cout << "panning=" << panning << std::endl;
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
    blend_mode = "alpha";
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
    , name("")
{}

Data::Entity::Animation::Soundline::Soundline(pugi::xml_node& elem)
    : id(0)
    , name("")
{}

Data::Entity::Animation::Soundline::~Soundline()
{
    clear();
}

bool Data::Entity::Animation::Soundline::load(pugi::xml_node& elem)
{
    id = elem.attribute("id").as_int(id);
    name = elem.attribute("name").as_string(name.c_str());
    debug_scml(log());

    for (auto& child : elem.children("key")) {
        Key* key = new Key;
        if (key->load(child)) {
            if (!keys.insert(std::make_pair(key->id, key)).second) {
                throw std::runtime_error("scml::Data::Entity::Animation::Soundline loaded a key with the duplicated id " + key->id);
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
    std::cout << "# Entity :: Animation :: Soundline >> ";
    std::cout << "id=" << id << " | ";
    std::cout << "name=" << name << std::endl;
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
    id = elem.attribute("id").as_int(id);
    debug_scml(log());

    // Object
    auto child = elem.child("object");
    if (child != nullptr && !object.load(child)) {
        throw std::runtime_error("scml::Data::Entity::Animation::Soundline::Key failed to load an object.");
    }

    return true;
}

void Data::Entity::Animation::Soundline::Key::log() const
{
    std::cout << "# Entity :: Animation :: Soundline :: Key >> ";
    std::cout << "id=" << id << " | " << std::endl;
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
    atlas = elem.attribute("atlas").as_int(atlas);
    folder = elem.attribute("folder").as_int(folder);
    file = elem.attribute("file").as_int(file);
    debug_scml(log());

    return true;
}

void Data::Entity::Animation::Soundline::Key::Object::log() const
{
    std::cout << "# Entity :: Animation :: Soundline :: Key :: Object >> ";
    std::cout << "atlas=" << atlas << " | ";
    std::cout << "folder=" << folder << " | ";
    std::cout << "file=" << file << " | " << std::endl;
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
    , name("")
{}

Data::Character_Map::Character_Map(pugi::xml_node& elem)
    : id(0)
    , name("")
{
    load(elem);
}

bool Data::Character_Map::load(pugi::xml_node& elem)
{
    id = elem.attribute("id").as_int(id);
    name = elem.attribute("name").as_string(name.c_str());
    debug_scml(log());

    auto child = elem.child("map");
    if (child || !map.load(child))
        throw std::runtime_error("scml::Data::Entity failed to load an animation");

    return true;
}

void Data::Character_Map::log() const
{
    std::cout << "# Character map >> ";
    std::cout << "id=" << id << " | ";
    std::cout << "name=" << name << std::endl;
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
    atlas = elem.attribute("atlas").as_int(atlas);
    folder = elem.attribute("folder").as_int(folder);
    file = elem.attribute("file").as_int(file);
    target_atlas = elem.attribute("target_atlas").as_int(target_atlas);
    target_folder = elem.attribute("target_folder").as_int(target_folder);
    target_file = elem.attribute("target_file").as_int(target_file);

    return true;
}

void Data::Character_Map::Map::log() const
{
    std::cout << "# Character map :: Map >> ";
    std::cout << "atlas=" << atlas << " | ";
    std::cout << "folder=" << folder << " | ";
    std::cout << "file=" << file << " | ";
    std::cout << "target_atlas=" << target_atlas << " | ";
    std::cout << "target_folder=" << target_folder << " | ";
    std::cout << "target_file=" << target_file << std::endl;
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
    : author("Unknown")
    , copyright("Unknown")
    , license("Unknown")
    , version("Unknown")
    , last_modified("Unknown")
    , notes("")
{}

Data::Document_Info::Document_Info(pugi::xml_node& elem)
    : author("Unknown")
    , copyright("Unknown")
    , license("Unknown")
    , version("Unknown")
    , last_modified("Unknown")
    , notes("")
{
    load(elem);
}

bool Data::Document_Info::load(pugi::xml_node& elem)
{
    author = elem.attribute("author").as_string(author.c_str());
    copyright = elem.attribute("copyright").as_string(copyright.c_str());
    license = elem.attribute("license").as_string(license.c_str());
    version = elem.attribute("version").as_string(version.c_str());
    last_modified = elem.attribute("last_modified").as_string(last_modified.c_str());
    notes = elem.attribute("notes").as_string(notes.c_str());

    return true;
}

void Data::Document_Info::log() const
{
    std::cout << "# Document Info >> ";
    std::cout << "author=" << author << " | ";
    std::cout << "copyright=" << copyright << " | ";
    std::cout << "license=" << license << " | ";
    std::cout << "version=" << version << " | ";
    std::cout << "last_modified=" << last_modified << " | ";
    std::cout << "notes=" << notes << std::endl;
}

void Data::Document_Info::clear()
{
    author = "Unknown";
    copyright = "Unknown";
    license = "Unknown";
    version = "Unknown";
    last_modified = "Unknown";
    notes = "";
}
