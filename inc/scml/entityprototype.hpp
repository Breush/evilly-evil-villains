#pragma once

#include "scml/data.hpp"
#include "scml/transform.hpp"
#include "tools/int.hpp"

#include <vector>
#include <string>
#include <map>

namespace scml
{

/*! \brief A class to directly interface with SCML character data and draw it (to be inherited).
 *
 * Derived classes provide the means for the EntityPrototype to draw itself with a specific renderer.
 */
class EntityPrototype
{
public:

    /*! Integer index of the SCML entity */
    int entity;
    /*! Integer index of the current SCML entity's animation */
    int animation;
    /*! Integer index of the current animation's current mainline keyframe */
    int key;

    int prevKey;

    /*! Time (in milliseconds) tracking the position of the animation from its beginning. */
    int time;

    class Bone_Transform_State
    {
        public:
        int entity;
        int animation;
        int key;
        int time;

        Transform base_transform;
        std::vector<Transform> transforms;

        Bone_Transform_State();

        bool should_rebuild(int entity, int animation, int key, int time, const Transform& base_transform);
        void rebuild(int entity, int animation, int key, int time, EntityPrototype* entity_ptr, const Transform& base_transform);
    };

    Bone_Transform_State bone_transform_state;

    std::wstring name;

    class Animation;
    std::map<int, Animation*> animations;

    //Meta_Data* meta_data;

    /*! \brief Stores all of the data that the EntityPrototype needs to update and draw itself, independent of the definition in scml::Data.
     */
    class Animation
    {
    public:

        int id;
        std::wstring name;
        int length;
        std::wstring looping;
        int loop_to;

        //Meta_Data* meta_data;

        class Mainline
        {
        public:

            Mainline(scml::Data::Entity::Animation::Mainline* mainline);

            ~Mainline();

            void clear();

            class Key;
            std::map<int, Key*> keys;

            class Key
            {
            public:

                int id;
                int time;
                //Meta_Data* meta_data;

                Key(scml::Data::Entity::Animation::Mainline::Key* key);

                ~Key();

                void clear();

                class Object;
                class Object_Ref;

                class Object_Container
                {
                public:
                    Object* object;
                    Object_Ref* object_ref;

                    Object_Container()
                        : object(NULL), object_ref(NULL)
                    {}
                    Object_Container(Object* object)
                        : object(object), object_ref(NULL)
                    {}
                    Object_Container(Object_Ref* object_ref)
                        : object(NULL), object_ref(object_ref)
                    {}

                    bool hasObject() const
                    {
                        return (object != NULL);
                    }
                    bool hasObject_Ref() const
                    {
                        return (object_ref != NULL);
                    }
                };

                std::map<int, Object_Container> objects;


                class Bone;
                class Bone_Ref;

                class Bone_Container
                {
                public:
                    Bone* bone;
                    Bone_Ref* bone_ref;

                    Bone_Container()
                        : bone(NULL), bone_ref(NULL)
                    {}
                    Bone_Container(Bone* bone)
                        : bone(bone), bone_ref(NULL)
                    {}
                    Bone_Container(Bone_Ref* bone_ref)
                        : bone(NULL), bone_ref(bone_ref)
                    {}

                    bool hasBone() const
                    {
                        return (bone != NULL);
                    }
                    bool hasBone_Ref() const
                    {
                        return (bone_ref != NULL);
                    }
                };

                std::map<int, Bone_Container> bones;


                class Bone
                {
                public:

                    int id;
                    int parent;  // a bone id
                    float x;
                    float y;
                    float angle;
                    float scale_x;
                    float scale_y;
                    float r;
                    float g;
                    float b;
                    float a;
                    //Meta_Data* meta_data;

                    Bone(scml::Data::Entity::Animation::Mainline::Key::Bone* bone);

                    void clear();

                };

                class Bone_Ref
                {
                public:

                    int id;
                    int parent;  // a bone id
                    int timeline;
                    int key;

                    Bone_Ref(scml::Data::Entity::Animation::Mainline::Key::Bone_Ref* bone_ref);

                    void clear();
                };

                class Object
                {
                public:

                    int id;
                    int parent; // a bone id
                    std::wstring object_type;
                    int atlas;
                    int folder;
                    int file;
                    std::wstring usage;
                    std::wstring blend_mode;
                    std::wstring name;
                    float x;
                    float y;
                    float pivot_x;
                    float pivot_y;
                    int pixel_art_mode_x;
                    int pixel_art_mode_y;
                    int pixel_art_mode_pivot_x;
                    int pixel_art_mode_pivot_y;
                    float angle;
                    float w;
                    float h;
                    float scale_x;
                    float scale_y;
                    float r;
                    float g;
                    float b;
                    float a;
                    std::wstring variable_type;
                    std::wstring value_string;
                    int value_int;
                    int min_int;
                    int max_int;
                    float value_float;
                    float min_float;
                    float max_float;
                    int animation;
                    float t;
                    int z_index;
                    float volume;
                    float panning;

                    //Meta_Data* meta_data;

                    Object(scml::Data::Entity::Animation::Mainline::Key::Object* object);

                    void clear();

                };

                class Object_Ref
                {
                public:

                    int id;
                    int parent;  // a bone id
                    int timeline;
                    int key;
                    int z_index;

                    Object_Ref(scml::Data::Entity::Animation::Mainline::Key::Object_Ref* object_ref);

                    void clear();
                };
            };

        };

        Mainline mainline;

        class Timeline;
        std::map<int, Timeline*> timelines;

        Animation(scml::Data::Entity::Animation* animation);

        ~Animation();

        void clear();



        class Timeline
        {
        public:

            int id;
            std::wstring name;
            std::wstring object_type;
            std::wstring variable_type;
            std::wstring usage;
            //Meta_Data* meta_data;

            Timeline(scml::Data::Entity::Animation::Timeline* timeline);

            ~Timeline();

            void clear();

            class Key;
            std::map<int, Key*> keys;

            class Key
            {
            public:

                int id;
                int time;
                std::wstring curve_type;
                float c1;
                float c2;
                int spin;

                bool has_object;

                Key(scml::Data::Entity::Animation::Timeline::Key* key);

                ~Key();

                void clear();


                //Meta_Data_Tweenable* meta_data;

                class Bone
                {
                public:

                    float x;
                    float y;
                    float angle;
                    float scale_x;
                    float scale_y;
                    float r;
                    float g;
                    float b;
                    float a;
                    //Meta_Data_Tweenable* meta_data;

                    Bone(scml::Data::Entity::Animation::Timeline::Key::Bone* bone);

                    void clear();
                };

                Bone bone;

                class Object
                {
                public:

                    int atlas;
                    int folder;
                    int file;
                    std::wstring name;
                    float x;
                    float y;
                    float pivot_x;
                    float pivot_y;
                    float angle;
                    float w;
                    float h;
                    float scale_x;
                    float scale_y;
                    float r;
                    float g;
                    float b;
                    float a;
                    std::wstring blend_mode;
                    std::wstring value_string;
                    int value_int;
                    int min_int;
                    int max_int;
                    float value_float;
                    float min_float;
                    float max_float;
                    int animation;
                    float t;
                    float volume;
                    float panning;

                    Object(scml::Data::Entity::Animation::Timeline::Key::Object* object);

                    void clear();

                };

                Object object;
            };
        };

        class Soundline
        {
        public:

            int id;
            std::wstring name;

            Soundline(scml::Data::Entity::Animation::Soundline* timeline);
            ~Soundline();

            void clear();

            class Key;
            std::map<int, Key*> keys;

            class Key
            {
            public:

                int id;

                Key(scml::Data::Entity::Animation::Soundline::Key* key);
                ~Key();

                void clear();

                class Object
                {
                public:

                    int atlas;
                    int folder;
                    int file;

                    Object(scml::Data::Entity::Animation::Soundline::Key::Object* object);

                    void clear();
                };

                Object object;
            };
        };

        Soundline soundline;
    };



    EntityPrototype();
    EntityPrototype(scml::Data* data, int entity, int animation = 0, int key = 0);
    EntityPrototype(scml::Data* data, const std::wstring& entityName, int animation = 0, int key = 0);
    virtual ~EntityPrototype();

    virtual void load(scml::Data* data);

    virtual void clear();

    /*! \brief Converts the given values from the renderer-specific coordinate system to the SCML coordinate system.
     *        SCML coords: +x to the right, +y up, +angle counter-clockwise
     *
     * \param x x-position in renderer coordinate system
     * \param y y-position in renderer coordinate system
     * \param angle Angle (in degrees) in renderer coordinate system
     */
    virtual void convert_to_SCML_coords(float& x, float& y, float& angle)
    {}

    /*! \brief Gets the dimensions of an image (from a FileSystem, presumably)
     *
     * \param folderID Integer folder ID of the image
     * \param fileID Integer file ID of the image
     * \return A pair consisting of the width and height of the image.  Returns (0,0) on error.
     */
    virtual std::pair<uint, uint> getImageDimensions(int folderID, int fileID) const = 0;

    /*! \brief Updates the state of the entity, incrementing its timer and changing the keyframe.
     *
     * \param dt_ms Change in time since last update, in milliseconds
     */
    virtual void update(int dt_ms);

    /*! \brief Draws the entity using a specific renderer by calling draw_internal().
     *
     * \param x x-position in renderer coordinate system
     * \param y y-position in renderer coordinate system
     * \param angle Angle (in degrees) in renderer coordinate system
     * \param scale_x Scale factor in the x-direction
     * \param scale_y Scale factor in the y-direction
     */
    virtual void draw(float x, float y, float angle = 0.0f, float scale_x = 1.0f, float scale_y = 1.0f);

    virtual void draw_simple_object(Animation::Mainline::Key::Object* obj);
    virtual void draw_tweened_object(Animation::Mainline::Key::Object_Ref* ref);

    /*! \brief Draws an image using a specific renderer.
     *
     * \param folderID Integer folder ID of the image
     * \param fileID Integer file ID of the image
     * \param x x-position in SCML coordinate system
     * \param y y-position in SCML coordinate system
     * \param angle Angle (in degrees) in SCML coordinate system
     * \param scale_x Scale factor in the x-direction
     * \param scale_y Scale factor in the y-direction
     */
    virtual void draw_internal(int folderID, int fileID, float x, float y, float angle, float scale_x, float scale_y) = 0;

    /*! \brief Chooses and resets the current animation.
     *
     * \param animation Integer animation ID
     */
    virtual void startAnimation(int animation);
    virtual void startAnimation(const std::wstring& animationName);

    virtual void play_sound(int folderID, int fileID) = 0;

    int getNumAnimations() const;
    Animation* getAnimation(int animation) const;
    Animation* getAnimation(const std::wstring& animationName) const;
    Animation::Mainline::Key* getKey(int animation, int key) const;
    Animation::Mainline::Key::Bone_Ref* getBoneRef(int animation, int key, int bone_ref) const;
    Animation::Mainline::Key::Object_Ref* getObjectRef(int animation, int key, int object_ref) const;

    int getNextKeyID(int animation, int lastKey) const;
    Animation::Timeline::Key* getTimelineKey(int animation, int timeline, int key);
    Animation::Timeline::Key::Object* getTimelineObject(int animation, int timeline, int key);
    Animation::Timeline::Key::Bone* getTimelineBone(int animation, int timeline, int key);

    bool getSimpleObjectTransform(Transform& result, Animation::Mainline::Key::Object* obj1);
    bool getTweenedObjectTransform(Transform& result, Animation::Mainline::Key::Object_Ref* ref);

    int getNumBones() const;
    int getNumObjects() const;

    bool getBoneTransform(Transform& result, int boneID);
    bool getObjectTransform(Transform& result, int objectID);
};

}

