#pragma once

#include <string>
#include <map>

// Forward declaration
namespace pugi
{
    class xml_node;
}

namespace scml
{

// Representation and storage of an SCML file in memory

class Data
{
public:
    std::string name;
    std::string scml_version;
    std::string generator;
    std::string generator_version;
    bool pixel_art_mode;

    class Folder;
    class Atlas;
    class Entity;
    class Character_Map;

    std::map<int, Folder*> folders;
    std::map<int, Atlas*> atlases;
    std::map<int, Entity*> entities;
    std::map<int, Character_Map*> character_maps;

    Data();
    Data(const std::string& file);
    Data(pugi::xml_node& elem);
    ~Data();

    bool load(const std::string& file);
    bool loadFromFile(const std::string& file);
    bool load(pugi::xml_node& elem);
    void log() const;
    void clear();

    //---------------------//
    //----- Meta_Data -----//

    class Meta_Data
    {
    public:

        class Variable;
        class Tag;

        std::map<std::string, Variable*> variables;
        std::map<std::string, Tag*> tags;

        Meta_Data();
        Meta_Data(pugi::xml_node& elem);

        bool load(pugi::xml_node& elem);
        void clear();

        //----- Variable -----//

        class Variable
        {
        public:

            std::string name;
            std::string type;

            std::string value_string;
            int value_int;
            float value_float;

            Variable();
            Variable(pugi::xml_node& elem);

            bool load(pugi::xml_node& elem);
            void log() const;
            void clear();
        };

        //----- Tag -----//

        class Tag
        {
        public:

            std::string name;

            Tag();
            Tag(pugi::xml_node& elem);

            bool load(pugi::xml_node& elem);
            void log() const;
            void clear();
        };
    };


    class Meta_Data_Tweenable
    {
    public:

        class Variable;
        class Tag;

        std::map<std::string, Variable*> variables;
        std::map<std::string, Tag*> tags;

        Meta_Data_Tweenable();
        Meta_Data_Tweenable(pugi::xml_node& elem);

        bool load(pugi::xml_node& elem);
        void log() const;
        void clear();

        class Variable
        {
        public:

            std::string name;
            std::string type;
            std::string value_string;
            int value_int;
            float value_float;
            std::string curve_type;
            float c1;
            float c2;

            Variable();
            Variable(pugi::xml_node& elem);

            bool load(pugi::xml_node& elem);
            void log() const;
            void clear();

        };

        class Tag
        {
        public:

            std::string name;

            Tag();
            Tag(pugi::xml_node& elem);

            bool load(pugi::xml_node& elem);
            void log() const;
            void clear();
        };
    };

    Meta_Data* meta_data;

    class Folder
    {
    public:

        int id;
        std::string name;

        class File;
        std::map<int, File*> files;

        Folder();
        Folder(pugi::xml_node& elem);

        ~Folder();

        bool load(pugi::xml_node& elem);
        void log() const;
        void clear();

        class File
        {
        public:

            std::string type;
            int id;
            std::string name;
            float pivot_x;
            float pivot_y;
            int width;
            int height;
            int atlas_x;
            int atlas_y;
            int offset_x;
            int offset_y;
            int original_width;
            int original_height;

            File();
            File(pugi::xml_node& elem);

            bool load(pugi::xml_node& elem);
            void log() const;
            void clear();

        };
    };

    class Atlas
    {
    public:
        int id;
        std::string data_path;
        std::string image_path;

        class Folder;
        std::map<int, Folder*> folders;

        Atlas();
        Atlas(pugi::xml_node& elem);

        ~Atlas();

        bool load(pugi::xml_node& elem);
        void log() const;
        void clear();

        class Folder
        {
        public:

            int id;
            std::string name;

            class Image;
            std::map<int, Image*> images;

            Folder();
            Folder(pugi::xml_node& elem);

            bool load(pugi::xml_node& elem);
            void log() const;
            void clear();


            class Image
            {
            public:

                int id;
                std::string full_path;

                Image();
                Image(pugi::xml_node& elem);

                bool load(pugi::xml_node& elem);
                void log() const;
                void clear();

            };
        };
    };

    class Entity
    {
    public:

        int id;
        std::string name;

        class Animation;
        std::map<int, Animation*> animations;

        Entity();
        Entity(pugi::xml_node& elem);

        ~Entity();

        bool load(pugi::xml_node& elem);
        void log() const;
        void clear();

        Meta_Data* meta_data;

        class Animation
        {
        public:

            int id;
            std::string name;
            int length;
            std::string looping;
            int loop_to;

            Meta_Data* meta_data;

            // More to follow...
            class Mainline
            {
            public:

                Mainline();
                Mainline(pugi::xml_node& elem);

                ~Mainline();

                bool load(pugi::xml_node& elem);
                void log() const;
                void clear();

                class Key;
                std::map<int, Key*> keys;

                class Key
                {
                public:

                    int id;
                    int time;
                    Meta_Data* meta_data;

                    Key();
                    Key(pugi::xml_node& elem);

                    ~Key();

                    bool load(pugi::xml_node& elem);
                    void log() const;
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
                        Meta_Data* meta_data;


                        Bone();
                        Bone(pugi::xml_node& elem);

                        bool load(pugi::xml_node& elem);
                        void log() const;
                        void clear();

                    };

                    class Bone_Ref
                    {
                    public:

                        int id;
                        int parent;  // a bone id
                        int timeline;
                        int key;

                        Bone_Ref();
                        Bone_Ref(pugi::xml_node& elem);

                        bool load(pugi::xml_node& elem);
                        void log() const;
                        void clear();
                    };

                    class Object
                    {
                    public:

                        int id;
                        int parent; // a bone id
                        std::string object_type;
                        int atlas;
                        int folder;
                        int file;
                        std::string usage;
                        std::string blend_mode;
                        std::string name;
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
                        std::string variable_type;
                        std::string value_string;
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

                        Meta_Data* meta_data;

                        Object();
                        Object(pugi::xml_node& elem);

                        bool load(pugi::xml_node& elem);
                        void log() const;
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

                        float abs_x;
                        float abs_y;
                        float abs_pivot_x;
                        float abs_pivot_y;
                        float abs_angle;
                        float abs_scale_x;
                        float abs_scale_y;
                        float abs_a;

                        Object_Ref();
                        Object_Ref(pugi::xml_node& elem);

                        bool load(pugi::xml_node& elem);
                        void log() const;
                        void clear();
                    };
                };

            };

            Mainline mainline;

            class Timeline;
            std::map<int, Timeline*> timelines;

            Animation();
            Animation(pugi::xml_node& elem);

            ~Animation();

            bool load(pugi::xml_node& elem);
            void log() const;
            void clear();



            class Timeline
            {
            public:

                int id;
                std::string name;
                std::string object_type;
                std::string variable_type;
                std::string usage;
                Meta_Data* meta_data;

                Timeline();
                Timeline(pugi::xml_node& elem);

                ~Timeline();

                bool load(pugi::xml_node& elem);
                void log() const;
                void clear();

                class Key;
                std::map<int, Key*> keys;

                class Key
                {
                public:

                    int id;
                    int time;
                    std::string curve_type;
                    float c1;
                    float c2;
                    int spin;

                    bool has_object;

                    Key();
                    Key(pugi::xml_node& elem);

                    ~Key();

                    bool load(pugi::xml_node& elem);
                    void log() const;
                    void clear();


                    Meta_Data_Tweenable* meta_data;

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
                        Meta_Data_Tweenable* meta_data;

                        Bone();
                        Bone(pugi::xml_node& elem);

                        bool load(pugi::xml_node& elem);
                        void log() const;
                        void clear();
                    };

                    Bone bone;

                    class Object
                    {
                    public:

                        int atlas;
                        int folder;
                        int file;
                        std::string name;
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
                        std::string blend_mode;
                        std::string value_string;
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
                        Meta_Data_Tweenable* meta_data;

                        Object();
                        Object(pugi::xml_node& elem);

                        bool load(pugi::xml_node& elem);
                        void log() const;
                        void clear();

                    };

                    Object object;
                };
            };

            // Soundline
            class Soundline
            {
            public:

                int id;
                std::string name;

                Soundline();
                Soundline(pugi::xml_node& elem);
                ~Soundline();

                bool load(pugi::xml_node& elem);
                void log() const;
                void clear();

                class Key;
                std::map<int, Key*> keys;

                class Key
                {
                public:

                    int id;

                    Key();
                    Key(pugi::xml_node& elem);
                    ~Key();

                    bool load(pugi::xml_node& elem);
                    void log() const;
                    void clear();

                    class Object
                    {
                    public:
                        int atlas;
                        int folder;
                        int file;

                        Object();
                        Object(pugi::xml_node& elem);

                        bool load(pugi::xml_node& elem);
                        void log() const;
                        void clear();
                    };

                    Object object;
                };
            };

            Soundline soundline;
        };
    };

    class Character_Map
    {
    public:

        int id;
        std::string name;

        Character_Map();
        Character_Map(pugi::xml_node& elem);

        bool load(pugi::xml_node& elem);
        void log() const;
        void clear();

        class Map
        {
        public:

            int atlas;
            int folder;
            int file;
            int target_atlas;
            int target_folder;
            int target_file;

            Map();
            Map(pugi::xml_node& elem);

            bool load(pugi::xml_node& elem);
            void log() const;
            void clear();
        };

        Map map;
    };

    class Document_Info
    {
    public:

        std::string author;
        std::string copyright;
        std::string license;
        std::string version;
        std::string last_modified;
        std::string notes;

        Document_Info();
        Document_Info(pugi::xml_node& elem);

        bool load(pugi::xml_node& elem);
        void log() const;
        void clear();

    };

    Document_Info document_info;

    int getNumAnimations(int entity) const;
};

}
