#include "tools/math.hpp" // interpolate
#include "tools/tools.hpp" // mapFind
#include "scml/entityprototype.hpp"

#include <cstring> // strcmp

using namespace scml;

EntityPrototype::EntityPrototype()
    : entity(-1), animation(-1), prevKey(-1), key(-1), time(0)
{}

EntityPrototype::EntityPrototype(scml::Data* data, int entity, int animation, int key)
    : entity(entity), animation(animation), prevKey(-1), key(key), time(0)
{
    load(data);
}

EntityPrototype::EntityPrototype(scml::Data* data, const std::wstring& entityName, int animation, int key)
    : entity(-1), animation(animation), prevKey(-1), key(key), time(0)
{
    load(data);
    for (auto const& e : data->entities) {
        auto const& entity_ptr = e.second;
        if (entity_ptr->name == entityName) {
            entity = entity_ptr->id;
            break;
        }
    }
}

EntityPrototype::~EntityPrototype()
{
    clear();
}

void EntityPrototype::load(scml::Data* data)
{
    returnif (data == NULL);

    scml::Data::Entity* entity_ptr = tools::mapFind(data->entities, entity);
    returnif (entity_ptr == NULL);

    name = entity_ptr->name;

    for (auto const& animation : entity_ptr->animations) {
        animations.insert(std::make_pair(animation.second->id, new Animation(animation.second)));
    }
}

void EntityPrototype::clear()
{
    entity = -1;
    animation = -1;
    prevKey = -1;
    key = -1;
    time = 0;

    for (auto const& animation : animations) {
        delete animation.second;
    }
    animations.clear();
}

void EntityPrototype::startAnimation(int animation)
{
    this->animation = animation;
    key = 0;
    time = 0;
}

void EntityPrototype::startAnimation(const std::wstring& animationName)
{
    scml::EntityPrototype::Animation* animation_ptr = getAnimation(animationName);
    this->animation = (animation_ptr == NULL)? -1 : animation_ptr->id;

    prevKey = -1;
    key = 0;
    time = 0;
}

void EntityPrototype::updateAnimation(int dt_ms)
{
    returnif (entity < 0 || animation < 0 || key < 0);

    scml::EntityPrototype::Animation* animation_ptr = getAnimation(animation);
    returnif (animation_ptr == NULL);

    time += dt_ms;

    if (animation_ptr->looping == L"true") {
        time %= animation_ptr->length;
    } else {
        if (time > animation_ptr->length) {
            time = animation_ptr->length;
        }
    }

    for (key = 0; key+1 < (int)animation_ptr->mainline.keys.size(); key++) {
        if (getKey(animation, key + 1)->time > time)
            break;
    }

    // Detect on key change
    if (prevKey != key) {
        // Soundline
        const auto& element = animation_ptr->soundline.keys.find(key);
        if (element != animation_ptr->soundline.keys.end()) {
            const auto& object = element->second->object;
            play_sound(object.folder, object.file);
        }
        prevKey = key;
    }
}

// This is for rotating untranslated points and offsetting them to a new origin.
static void rotate_point(float& x, float& y, float angle, float origin_x, float origin_y, bool flipped)
{
    float s = sinf(angle*M_PI/180);
    float c = cosf(angle*M_PI/180);
    float xnew = (x * c) - (y * s);
    float ynew = (x * s) + (y * c);
    xnew += origin_x;
    ynew += origin_y;

    x = xnew;
    y = ynew;
}

void EntityPrototype::draw(float x, float y, float angle, float scale_x, float scale_y)
{
    // Get key
    Animation::Mainline::Key* key_ptr = getKey(animation, key);
    returnif (key_ptr == NULL);

    convert_to_SCML_coords(x, y, angle);

    int nextKeyID = getNextKeyID(animation, key);
    Animation::Mainline::Key* nextkey_ptr = getKey(animation, nextKeyID);
    if (nextkey_ptr == NULL) {
        nextkey_ptr = key_ptr;
    }

    // Build up the bone transform hierarchy
    Transform base_transform(x, y, angle, scale_x, scale_y);
    if (bone_transform_state.should_rebuild(entity, animation, key, time, base_transform)) {
        bone_transform_state.rebuild(entity, animation, key, time, this, base_transform);
    }

    // Go through each object
    for (auto const& object : key_ptr->objects) {
        auto const& item = object.second;

        if (item.hasObject()) draw_simple_object(item.object);
        else draw_tweened_object(item.object_ref);
    }
}


void EntityPrototype::draw_simple_object(Animation::Mainline::Key::Object* obj1)
{
    // Get parent bone transform
    Transform parent_transform;

    if (obj1->parent < 0) parent_transform = bone_transform_state.base_transform;
    else parent_transform = bone_transform_state.transforms[obj1->parent];

    // Set object transform
    Transform obj_transform(obj1->x, obj1->y, obj1->angle, obj1->scale_x, obj1->scale_y);

    // Transform the sprite by the parent transform.
    obj_transform.apply_parent_transform(parent_transform);

    // Transform the sprite by its own transform now.
    float pivot_x_ratio = obj1->pivot_x;
    float pivot_y_ratio = obj1->pivot_y;

    // No image tweening
    std::pair<uint, uint> img_dims = getImageDimensions(obj1->folder, obj1->file);

    // Rotate about the pivot point and draw from the center of the image
    float offset_x = (pivot_x_ratio - 0.5f)*img_dims.first;
    float offset_y = (pivot_y_ratio - 0.5f)*img_dims.second;
    float sprite_x = -offset_x*obj_transform.scale_x;
    float sprite_y = -offset_y*obj_transform.scale_y;

    bool flipped = ((obj_transform.scale_x < 0) != (obj_transform.scale_y < 0));
    rotate_point(sprite_x, sprite_y, obj_transform.angle, obj_transform.x, obj_transform.y, flipped);

    // Let the renderer draw it
    draw_internal(obj1->folder, obj1->file, sprite_x, sprite_y, obj_transform.angle, obj_transform.scale_x, obj_transform.scale_y);
}


void EntityPrototype::draw_tweened_object(Animation::Mainline::Key::Object_Ref* ref)
{
    returnif (ref == NULL);

    // Dereference object_ref and get the next one in the timeline for tweening
    Animation* animation_ptr = getAnimation(animation);  // Need this only if looping...
    Animation::Timeline::Key* t_key1 = getTimelineKey(animation, ref->timeline, ref->key);
    Animation::Timeline::Key* t_key2 = getTimelineKey(animation, ref->timeline, ref->key+1);

    if (t_key2 == NULL) t_key2 = t_key1;
    returnif (t_key1 == NULL || !t_key1->has_object || !t_key2->has_object);

    Animation::Timeline::Key::Object* obj1 = &t_key1->object;
    Animation::Timeline::Key::Object* obj2 = &t_key2->object;
    if (obj2 == NULL) obj2 = obj1;

    if (obj1 != NULL) {
        // Get interpolation (tweening) factor
        float t = 0.0f;
        if (t_key2->time > t_key1->time)
            t = (time - t_key1->time)/float(t_key2->time - t_key1->time);
        else if (t_key2->time < t_key1->time)
            t = (time - t_key1->time)/float(animation_ptr->length - t_key1->time);

        // Get parent bone transform
        Transform parent_transform;
        if (ref->parent < 0) parent_transform = bone_transform_state.base_transform;
        else parent_transform = bone_transform_state.transforms[ref->parent];

        // Set object transform
        Transform obj_transform(obj1->x, obj1->y, obj1->angle, obj1->scale_x, obj1->scale_y);

        // Tween with next key's object
        obj_transform.lerp(Transform(obj2->x, obj2->y, obj2->angle, obj2->scale_x, obj2->scale_y), t, t_key1->spin);

        // Transform the sprite by the parent transform.
        obj_transform.apply_parent_transform(parent_transform);

        // Transform the sprite by its own transform now.
        float pivot_x_ratio = interpolate(obj1->pivot_x, obj2->pivot_x, t);
        float pivot_y_ratio = interpolate(obj1->pivot_y, obj2->pivot_y, t);

        // No image tweening
        std::pair<unsigned int, unsigned int> img_dims = getImageDimensions(obj1->folder, obj1->file);

        // Rotate about the pivot point and draw from the center of the image
        float offset_x = (pivot_x_ratio - 0.5f)*img_dims.first;
        float offset_y = (pivot_y_ratio - 0.5f)*img_dims.second;
        float sprite_x = -offset_x*obj_transform.scale_x;
        float sprite_y = -offset_y*obj_transform.scale_y;

        bool flipped = ((obj_transform.scale_x < 0) != (obj_transform.scale_y < 0));
        rotate_point(sprite_x, sprite_y, obj_transform.angle, obj_transform.x, obj_transform.y, flipped);

        // Let the renderer draw it
        draw_internal(obj1->folder, obj1->file, sprite_x, sprite_y, obj_transform.angle, obj_transform.scale_x, obj_transform.scale_y);
    }
}

EntityPrototype::Bone_Transform_State::Bone_Transform_State()
    : entity(-1), animation(-1), key(-1), time(-1)
{}

bool EntityPrototype::Bone_Transform_State::should_rebuild(int entity, int animation, int key, int time, const Transform& base_transform)
{
    return (entity != this->entity ||
            animation != this->animation ||
            key != this->key ||
            time != this->time ||
            this->base_transform != base_transform);
}

void EntityPrototype::Bone_Transform_State::rebuild(int entity, int animation, int key, int time, EntityPrototype* entity_ptr, const Transform& base_transform)
{
    if (entity_ptr == NULL) {
        this->entity = -1;
        this->animation = -1;
        this->key = -1;
        this->time = -1;
        return;
    }

    this->entity = entity;
    this->animation = animation;
    this->key = key;
    this->time = time;
    this->base_transform = base_transform;

    transforms.clear();

    EntityPrototype::Animation::Mainline::Key* key_ptr = entity_ptr->getKey(animation, key);

    // Resize the transform vector according to the biggest bone index
    int max_index = -1;
    for (auto const& bone : key_ptr->bones) {
        auto const& item = bone.second;

        int index = -1;
        if (item.hasBone_Ref()) {
            index = item.bone_ref->id;
        } else if (item.hasBone()) {
            index = item.bone->id;
        }

        if (max_index < index) {
            max_index = index;
        }
    }

    returnif (max_index < 0);

    transforms.resize(max_index+1);

    EntityPrototype::Animation* animation_ptr = entity_ptr->getAnimation(animation);

    // Calculate and store the transforms
    for (auto const& bone : key_ptr->bones) {
        auto const& item = bone.second;

        if (item.hasBone_Ref()) {
            Animation::Mainline::Key::Bone_Ref* ref = item.bone_ref;

            // Dereference bone_refs
            Animation::Timeline::Key* b_key1 = entity_ptr->getTimelineKey(animation, ref->timeline, ref->key);
            Animation::Timeline::Key* b_key2 = entity_ptr->getTimelineKey(animation, ref->timeline, ref->key+1);
            if (b_key2 == NULL) {
                b_key2 = b_key1;
            }
            if (b_key1 != NULL) {
                float t = 0.0f;
                if (b_key2->time > b_key1->time) {
                    t = (time - b_key1->time)/float(b_key2->time - b_key1->time);
                } else if (b_key2->time < b_key1->time) {
                    t = (time - b_key1->time)/float(animation_ptr->length - b_key1->time);
                }

                EntityPrototype::Animation::Timeline::Key::Bone* bone1 = &b_key1->bone;
                EntityPrototype::Animation::Timeline::Key::Bone* bone2 = &b_key2->bone;

                // Assuming that bones come in hierarchical order so that the parents have already been processed.
                Transform parent_transform;
                if (ref->parent < 0) {
                    parent_transform = base_transform;
                } else {
                    parent_transform = transforms[ref->parent];
                }

                // Set bone transform
                Transform b_transform(bone1->x, bone1->y, bone1->angle, bone1->scale_x, bone1->scale_y);

                // Tween with next key's bone
                b_transform.lerp(Transform(bone2->x, bone2->y, bone2->angle, bone2->scale_x, bone2->scale_y), t, b_key1->spin);

                // Transform the bone by the parent transform.
                b_transform.apply_parent_transform(parent_transform);

                transforms[ref->id] = b_transform;

            }

        } else if (item.hasBone()) {
            Animation::Mainline::Key::Bone* bone1 = item.bone;

            // Assuming that bones come in hierarchical order so that the parents have already been processed.
            Transform parent_transform;
            if (bone1->parent < 0) {
                parent_transform = base_transform;
            } else {
                parent_transform = transforms[bone1->parent];
            }

            // Set bone transform
            Transform b_transform(bone1->x, bone1->y, bone1->angle, bone1->scale_x, bone1->scale_y);

            // Transform the bone by the parent transform.
            b_transform.apply_parent_transform(parent_transform);

            transforms[bone1->id] = b_transform;

        }
    }
}




EntityPrototype::Animation::Animation(scml::Data::Entity::Animation* animation)
    : id(animation->id), name(animation->name), length(animation->length), looping(animation->looping), loop_to(animation->loop_to)
    , mainline(&animation->mainline), soundline(&animation->soundline)
{
    for (auto const& timeline : animation->timelines)
        timelines.insert(std::make_pair(timeline.second->id, new Timeline(timeline.second)));
}

EntityPrototype::Animation::~Animation()
{
    clear();
}

void EntityPrototype::Animation::clear()
{
    for (auto const& timeline : timelines)
        delete timeline.second;
    timelines.clear();
}


EntityPrototype::Animation::Mainline::Mainline(scml::Data::Entity::Animation::Mainline* mainline)
{
    for (auto const& key : mainline->keys)
        keys.insert(std::make_pair(key.second->id, new Key(key.second)));
}

EntityPrototype::Animation::Mainline::~Mainline()
{
    clear();
}

void EntityPrototype::Animation::Mainline::clear()
{
    for (auto const& key : keys)
        delete key.second;
    keys.clear();
}


EntityPrototype::Animation::Mainline::Key::Key(scml::Data::Entity::Animation::Mainline::Key* key)
    : id(key->id), time(key->time)
{
    // Load bones and objects
    for (auto const& bone : key->bones) {
        auto const& item = bone.second;

        if (item.hasBone()) {
            Bone* b = new Bone(item.bone);
            bones.insert(std::make_pair(b->id, Bone_Container(b)));
        }
        if (item.hasBone_Ref()) {
            Bone_Ref* b = new Bone_Ref(item.bone_ref);
            bones.insert(std::make_pair(b->id, Bone_Container(b)));
        }
    }


    for (auto const& object : key->objects) {
        auto const& item = object.second;

        if (item.hasObject()) {
            Object* b = new Object(item.object);
            objects.insert(std::make_pair(b->id, Object_Container(b)));
        }
        if (item.hasObject_Ref()) {
            Object_Ref* b = new Object_Ref(item.object_ref);
            objects.insert(std::make_pair(b->id, Object_Container(b)));
        }
    }
}

EntityPrototype::Animation::Mainline::Key::~Key()
{
    clear();
}

void EntityPrototype::Animation::Mainline::Key::clear()
{
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


EntityPrototype::Animation::Mainline::Key::Bone::Bone(scml::Data::Entity::Animation::Mainline::Key::Bone* bone)
    : parent(bone->parent)
    , x(bone->x), y(bone->y), angle(bone->angle), scale_x(bone->scale_x), scale_y(bone->scale_y), r(bone->r), g(bone->g), b(bone->b), a(bone->a)
{}

void EntityPrototype::Animation::Mainline::Key::Bone::clear()
{}


EntityPrototype::Animation::Mainline::Key::Bone_Ref::Bone_Ref(scml::Data::Entity::Animation::Mainline::Key::Bone_Ref* bone_ref)
    : id(bone_ref->id), parent(bone_ref->parent), timeline(bone_ref->timeline), key(bone_ref->key)
{}

void EntityPrototype::Animation::Mainline::Key::Bone_Ref::clear()
{}


EntityPrototype::Animation::Mainline::Key::Object::Object(scml::Data::Entity::Animation::Mainline::Key::Object* object)
    : id(object->id), parent(object->parent), object_type(object->object_type), atlas(object->atlas), folder(object->folder), file(object->file)
    , usage(object->usage), blend_mode(object->blend_mode), name(object->name)
    , x(object->x), y(object->y), pivot_x(object->pivot_x), pivot_y(object->pivot_y)
    , pixel_art_mode_x(object->pixel_art_mode_x), pixel_art_mode_y(object->pixel_art_mode_y), pixel_art_mode_pivot_x(object->pixel_art_mode_pivot_x), pixel_art_mode_pivot_y(object->pixel_art_mode_pivot_y), angle(object->angle)
    , w(object->w), h(object->h), scale_x(object->scale_x), scale_y(object->scale_y), r(object->r), g(object->g), b(object->b), a(object->a)
    , variable_type(object->variable_type), value_string(object->value_string), value_int(object->value_int), min_int(object->min_int), max_int(object->max_int)
    , value_float(object->value_float), min_float(object->min_float), max_float(object->max_float), animation(object->animation), t(object->t)
    , z_index(object->z_index)
    , volume(object->volume), panning(object->panning)
{}

void EntityPrototype::Animation::Mainline::Key::Object::clear()
{}


EntityPrototype::Animation::Mainline::Key::Object_Ref::Object_Ref(scml::Data::Entity::Animation::Mainline::Key::Object_Ref* object_ref)
    : id(object_ref->id), parent(object_ref->parent), timeline(object_ref->timeline), key(object_ref->key), z_index(object_ref->z_index)
{}

void EntityPrototype::Animation::Mainline::Key::Object_Ref::clear()
{}

//--------------------//
//----- Timeline -----//

EntityPrototype::Animation::Timeline::Timeline(scml::Data::Entity::Animation::Timeline* timeline)
    : id(timeline->id), name(timeline->name), object_type(timeline->object_type), variable_type(timeline->variable_type), usage(timeline->usage)
{
    for (auto const& key : timeline->keys)
        keys.insert(std::make_pair(key.second->id, new Key(key.second)));
}

EntityPrototype::Animation::Timeline::~Timeline()
{
    clear();
}

void EntityPrototype::Animation::Timeline::clear()
{
    for (auto const& key : keys)
        delete key.second;
    keys.clear();
}


EntityPrototype::Animation::Timeline::Key::Key(scml::Data::Entity::Animation::Timeline::Key* key)
    : id(key->id), time(key->time), curve_type(key->curve_type), c1(key->c1), c2(key->c2), spin(key->spin), has_object(key->has_object), bone(&key->bone), object(&key->object)
{

}
EntityPrototype::Animation::Timeline::Key::~Key()
{
    clear();
}


void EntityPrototype::Animation::Timeline::Key::clear()
{
    bone.clear();
    object.clear();
}


EntityPrototype::Animation::Timeline::Key::Bone::Bone(scml::Data::Entity::Animation::Timeline::Key::Bone* bone)
    : x(bone->x), y(bone->y), angle(bone->angle), scale_x(bone->scale_x), scale_y(bone->scale_y), r(bone->r), g(bone->g), b(bone->b), a(bone->a)
{}

void EntityPrototype::Animation::Timeline::Key::Bone::clear()
{}


EntityPrototype::Animation::Timeline::Key::Object::Object(scml::Data::Entity::Animation::Timeline::Key::Object* object)
    : atlas(object->atlas), folder(object->folder), file(object->file), name(object->name)
    , x(object->x), y(object->y), pivot_x(object->pivot_x), pivot_y(object->pivot_y), angle(object->angle)
    , w(object->w), h(object->h), scale_x(object->scale_x), scale_y(object->scale_y), r(object->r), g(object->g), b(object->b), a(object->a)
    , blend_mode(object->blend_mode), value_string(object->value_string), value_int(object->value_int), min_int(object->min_int), max_int(object->max_int)
    , value_float(object->value_float), min_float(object->min_float), max_float(object->max_float), animation(object->animation), t(object->t)
    , volume(object->volume), panning(object->panning)
{

}

void EntityPrototype::Animation::Timeline::Key::Object::clear()
{

}

//--------------------//
//----- Soundline -----//

EntityPrototype::Animation::Soundline::Soundline(scml::Data::Entity::Animation::Soundline* soundline)
    : id(soundline->id), name(soundline->name)
{
    for (auto const& key : soundline->keys)
        keys.insert(std::make_pair(key.second->id, new Key(key.second)));
}

EntityPrototype::Animation::Soundline::~Soundline()
{
    clear();
}

void EntityPrototype::Animation::Soundline::clear()
{
    for (auto const& key : keys)
        delete key.second;
    keys.clear();
}

EntityPrototype::Animation::Soundline::Key::Key(scml::Data::Entity::Animation::Soundline::Key* key)
    : id(key->id), object(&key->object)
{
}

EntityPrototype::Animation::Soundline::Key::~Key()
{
    clear();
}


void EntityPrototype::Animation::Soundline::Key::clear()
{
    object.clear();
}

EntityPrototype::Animation::Soundline::Key::Object::Object(scml::Data::Entity::Animation::Soundline::Key::Object* object)
    : atlas(object->atlas), folder(object->folder), file(object->file)
{

}

void EntityPrototype::Animation::Soundline::Key::Object::clear()
{

}

int EntityPrototype::getNumAnimations() const
{
    return animations.size();
}

EntityPrototype::Animation* EntityPrototype::getAnimation(int animation) const
{
    return tools::mapFind(animations, animation);
}

EntityPrototype::Animation* EntityPrototype::getAnimation(const std::wstring& animationName) const
{
    for (auto const& animation : animations)
        if (animation.second->name == animationName)
            return animation.second;

    return NULL;
}

EntityPrototype::Animation::Mainline::Key* EntityPrototype::getKey(int animation, int key) const
{
    Animation* a = tools::mapFind(animations, animation);
    returnif (a == NULL) NULL;

    return tools::mapFind(a->mainline.keys, key);
}


EntityPrototype::Animation::Mainline::Key::Bone_Ref* EntityPrototype::getBoneRef(int animation, int key, int bone_ref) const
{
    Animation* a = tools::mapFind(animations, animation);
    returnif (a == NULL) NULL;

    Animation::Mainline::Key* k = tools::mapFind(a->mainline.keys, key);
    returnif (k == NULL) NULL;

    Animation::Mainline::Key::Bone_Container b = tools::mapFind(k->bones, bone_ref);
    returnif (!b.hasBone_Ref()) NULL;

    return b.bone_ref;
}

EntityPrototype::Animation::Mainline::Key::Object_Ref* EntityPrototype::getObjectRef(int animation, int key, int object_ref) const
{
    Animation* a = tools::mapFind(animations, animation);
    returnif (a == NULL) NULL;

    Animation::Mainline::Key* k = tools::mapFind(a->mainline.keys, key);
    returnif (k == NULL) NULL;

    Animation::Mainline::Key::Object_Container o = tools::mapFind(k->objects, object_ref);
    returnif (!o.hasObject_Ref()) NULL;

    return o.object_ref;
}

// Gets the next key index according to the animation's looping setting.
int EntityPrototype::getNextKeyID(int animation, int lastKey) const
{
    returnif (entity < 0 || animation < 0 || lastKey < 0) -1;

    Animation* animation_ptr = getAnimation(animation);
    returnif (animation_ptr == NULL) -2;

    if (animation_ptr->looping == L"true") {
        // If we've reached the end of the keys, loop.
        if (lastKey+1 >= int(animation_ptr->mainline.keys.size())) {
            return animation_ptr->loop_to;
        } else {
            return lastKey+1;
        }
    } else if (animation_ptr->looping == L"ping_pong") {
        // TODO: Implement ping_pong animation
    }
    
    // assume looping "false"
    // If we've haven't reached the end of the keys, return the next one.
    if (lastKey+1 < int(animation_ptr->mainline.keys.size())) {
        return lastKey+1;
    }
    
    // if we have reached the end, stick to this key
    return lastKey;
}


EntityPrototype::Animation::Timeline::Key* EntityPrototype::getTimelineKey(int animation, int timeline, int key)
{
    Animation* a = tools::mapFind(animations, animation);
    returnif (a == NULL) NULL;

    Animation::Timeline* t = tools::mapFind(a->timelines, timeline);
    returnif (t == NULL) NULL;

    int no_keys = t->keys.size();
    if (key >= no_keys) {
        if (a->looping == L"true") {
            return tools::mapFind(t->keys, 0);
        } else {
            return tools::mapFind(t->keys, no_keys);
        }

    } else {
        return tools::mapFind(t->keys, key);
    }
}


EntityPrototype::Animation::Timeline::Key::Object* EntityPrototype::getTimelineObject(int animation, int timeline, int key)
{
    Animation* a = tools::mapFind(animations, animation);
    returnif (a == NULL) NULL;

    Animation::Timeline* t = tools::mapFind(a->timelines, timeline);
    returnif (t == NULL) NULL;

    Animation::Timeline::Key* k = tools::mapFind(t->keys, key);
    returnif (k == NULL || !k->has_object) NULL;

    return &k->object;
}

EntityPrototype::Animation::Timeline::Key::Bone* EntityPrototype::getTimelineBone(int animation, int timeline, int key)
{
    Animation* a = tools::mapFind(animations, animation);
    returnif (a == NULL) NULL;

    Animation::Timeline* t = tools::mapFind(a->timelines, timeline);
    returnif (t == NULL) NULL;

    Animation::Timeline::Key* k = tools::mapFind(t->keys, key);
    returnif (k == NULL || k->has_object) NULL;

    return &k->bone;
}

int EntityPrototype::getNumBones() const
{
    // Get key
    Animation::Mainline::Key* key_ptr = getKey(animation, key);
    returnif (key_ptr == NULL) 0;

    return key_ptr->bones.size();
}

int EntityPrototype::getNumObjects() const
{
    // Get key
    Animation::Mainline::Key* key_ptr = getKey(animation, key);
    returnif (key_ptr == NULL) 0;

    return key_ptr->objects.size();
}

bool EntityPrototype::getBoneTransform(Transform& result, int boneID)
{
    // Get key
    Animation::Mainline::Key* key_ptr = getKey(animation, key);
    returnif (key_ptr == NULL) false;

    // Find object
    Animation::Mainline::Key::Bone_Container item = tools::mapFind(key_ptr->bones, boneID);
    if (item.hasBone()) {
        // Get bone transform
        result = bone_transform_state.transforms[item.bone->id];
        convert_to_SCML_coords(result.x, result.y, result.angle);
        return true;
    } else if (item.hasBone_Ref()) {
        // Get bone transform
        result = bone_transform_state.transforms[item.bone_ref->id];
        convert_to_SCML_coords(result.x, result.y, result.angle);
        return true;
    } else {
        return false;
    }
}

bool EntityPrototype::getObjectTransform(Transform& result, int objectID)
{
    // Get key
    Animation::Mainline::Key* key_ptr = getKey(animation, key);
    returnif (key_ptr == NULL) false;

    // Find object
    Animation::Mainline::Key::Object_Container item = tools::mapFind(key_ptr->objects, objectID);
    if (item.hasObject()) return getSimpleObjectTransform(result, item.object);
    else if (item.hasObject_Ref()) return getTweenedObjectTransform(result, item.object_ref);

    return false;
}

bool EntityPrototype::getSimpleObjectTransform(Transform& result, scml::EntityPrototype::Animation::Mainline::Key::Object* obj1)
{
    if (obj1 == NULL) {
        return false;
    }

    // Get parent bone transform
    Transform parent_transform;

    if (obj1->parent < 0) parent_transform = bone_transform_state.base_transform;
    else parent_transform = bone_transform_state.transforms[obj1->parent];

    // Set object transform
    Transform obj_transform(obj1->x, obj1->y, obj1->angle, obj1->scale_x, obj1->scale_y);

    // Transform the sprite by the parent transform.
    obj_transform.apply_parent_transform(parent_transform);

    // Transform the sprite by its own transform now.
    float pivot_x_ratio = obj1->pivot_x;
    float pivot_y_ratio = obj1->pivot_y;

    // No image tweening
    std::pair<unsigned int, unsigned int> img_dims = getImageDimensions(obj1->folder, obj1->file);

    // Rotate about the pivot point and draw from the center of the image
    float offset_x = (pivot_x_ratio - 0.5f)*img_dims.first;
    float offset_y = (pivot_y_ratio - 0.5f)*img_dims.second;
    float sprite_x = -offset_x*obj_transform.scale_x;
    float sprite_y = -offset_y*obj_transform.scale_y;

    bool flipped = ((obj_transform.scale_x < 0) != (obj_transform.scale_y < 0));
    rotate_point(sprite_x, sprite_y, obj_transform.angle, obj_transform.x, obj_transform.y, flipped);

    // Save the result
    result.x = sprite_x;
    result.y = sprite_y;
    result.angle = flipped? -obj_transform.angle : obj_transform.angle;
    result.scale_x = obj_transform.scale_x;
    result.scale_y = obj_transform.scale_y;

    convert_to_SCML_coords(result.x, result.y, result.angle);
    return true;
}

bool EntityPrototype::getTweenedObjectTransform(Transform& result, scml::EntityPrototype::Animation::Mainline::Key::Object_Ref* ref)
{
    // Dereference object_ref and get the next one in the timeline for tweening
    Animation* animation_ptr = getAnimation(animation);  // Only needed if looping...
    Animation::Timeline::Key* t_key1 = getTimelineKey(animation, ref->timeline, ref->key);
    Animation::Timeline::Key* t_key2 = getTimelineKey(animation, ref->timeline, ref->key+1);
    if (t_key2 == NULL) {
        t_key2 = t_key1;
    }
    if (t_key1 == NULL || !t_key1->has_object || !t_key2->has_object) {
        return false;
    }

    Animation::Timeline::Key::Object* obj1 = &t_key1->object;
    Animation::Timeline::Key::Object* obj2 = &t_key2->object;
    if (obj2 == NULL) {
        obj2 = obj1;
    }
    if (obj1 == NULL) {
        return false;
    }

    // Get interpolation (tweening) factor
    float t = 0.0f;
    if (t_key2->time > t_key1->time)
        t = (time - t_key1->time)/float(t_key2->time - t_key1->time);
    else if (t_key2->time < t_key1->time)
        t = (time - t_key1->time)/float(animation_ptr->length - t_key1->time);

    // Get parent bone transform
    Transform parent_transform;
    if (ref->parent < 0) parent_transform = bone_transform_state.base_transform;
    else parent_transform = bone_transform_state.transforms[ref->parent];

    // Set object transform
    Transform obj_transform(obj1->x, obj1->y, obj1->angle, obj1->scale_x, obj1->scale_y);

    // Tween with next key's object
    obj_transform.lerp(Transform(obj2->x, obj2->y, obj2->angle, obj2->scale_x, obj2->scale_y), t, t_key1->spin);

    // Transform the sprite by the parent transform.
    obj_transform.apply_parent_transform(parent_transform);

    // Transform the sprite by its own transform now.
    float pivot_x_ratio = interpolate(obj1->pivot_x, obj2->pivot_x, t);
    float pivot_y_ratio = interpolate(obj1->pivot_y, obj2->pivot_y, t);

    // No image tweening
    std::pair<unsigned int, unsigned int> img_dims = getImageDimensions(obj1->folder, obj1->file);

    // Rotate about the pivot point and draw from the center of the image
    float offset_x = (pivot_x_ratio - 0.5f)*img_dims.first;
    float offset_y = (pivot_y_ratio - 0.5f)*img_dims.second;
    float sprite_x = -offset_x*obj_transform.scale_x;
    float sprite_y = -offset_y*obj_transform.scale_y;

    bool flipped = ((obj_transform.scale_x < 0) != (obj_transform.scale_y < 0));
    rotate_point(sprite_x, sprite_y, obj_transform.angle, obj_transform.x, obj_transform.y, flipped);

    // Save the result
    result.x = sprite_x;
    result.y = sprite_y;
    result.angle = flipped? -obj_transform.angle : obj_transform.angle;
    result.scale_x = obj_transform.scale_x;
    result.scale_y = obj_transform.scale_y;

    convert_to_SCML_coords(result.x, result.y, result.angle);
    return true;
}
