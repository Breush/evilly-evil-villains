#pragma once

#include <Spriter/global/global.h>
#include <Spriter/override/spriterfileloader.h>
#include <Spriter/loading/loadinghelpers.h>
#include <Spriter/loading/loader.h>

#include <pugixml/pugixml.hpp>

namespace SpriterEngine
{
    // Forward declarations

    class SpriterModel;
    class Entity;
    class VariableContainer;
    class Animation;
    class Object;
    class TimeInfo;
    class EasingCurveInterface;
    class UniversalObjectInterface;
    class Timeline;
    class MainlineKey;
    class FileFlattener;

    class PugiXmlScmlLoader : public SpriterFileLoader
    {
    public:

        void loadFile(SpriterModel *model, const std::string &fileName) override;

    private:

        bool equal(const std::string & a, const std::string & b);

        void getFolderFileStructureFromElement(pugi::xml_node spriterDataElement, SpriterModel * model, std::string scmlFileName, FileFlattener * fileFlattener);
        void getTagListFromElement(pugi::xml_node spriterDataElement, SpriterModel * model);

        void getEntitiesFromElement(pugi::xml_node spriterDataElement, SpriterModel * model, FileFlattener * fileFlattener);
        Entity * getNewEntityFromEntityElement(pugi::xml_node entityElement, SpriterModel * model);
        void getObjectInfoFromEntityElement(pugi::xml_node entityElement, Entity * entity, PointMap *defaultBoxPivotMap);

        void getVarDefArrayFromElement(pugi::xml_node parentObjectElement, VariableContainer * parentObjectAsVariableContainer);

        void getCharacterMapsFromEntityElement(pugi::xml_node entityElement, SpriterModel * model, Entity * entity, FileFlattener * fileFlattener);

        void getAnimationsFromEntityElement(pugi::xml_node entityElement, SpriterModel * model, Entity * entity, FileFlattener * fileFlattener, PointMap *defaultBoxPivotMap);
        Animation * getNewAnimationFromAnimationElement(pugi::xml_node animationElement, Entity * entity);

        void getTimelinesFromAnimationElement(pugi::xml_node animationElement, SpriterModel * model, Entity * entity, Animation * animation, FileFlattener * fileFlattener, SpriteKeyFileInfoObjectIdMap * spriteKeyFileInfoMap, SubEntityKeyInfoMap * subEntityKeyInfoMap, PointMap *defaultBoxPivotMap);

        void createRedundantFirstKeys(Animation * animation, Timeline * timeline);

        void getMetaDataFromElement(pugi::xml_node parentObjectElement, SpriterModel *model, Entity *entity, Animation *animation, int objectId);

        void getVarlinesFromMetaDataElement(pugi::xml_node animationElement, Entity *entity, Animation *animation, int objectId);

        UniversalObjectInterface * getObjectInfoFromVariableKeyElement(pugi::xml_node variableKeyElement, Variable *variable);

        void getTaglinesFromMetaDataElement(pugi::xml_node metaDataElement, SpriterModel *model, Animation *animation, int objectId);

        UniversalObjectInterface * getObjectInfoFromTagKeyElement(pugi::xml_node tagKeyElement, SpriterModel *model);

        Object * getObjectFromTimelineElement(pugi::xml_node  timelineElement, Entity * entity);

        TimeInfo * getTimeInfoFromElement(pugi::xml_node validCurrentKeyElement, pugi::xml_node nextKeyElement, pugi::xml_node validFirstKeyElement, real animationLength, bool animationLooping);
        EasingCurveInterface * getEasingCurveFromAttributes(pugi::xml_attribute  att);

        UniversalObjectInterface * getObjectInfoFromTimelineKeyElement(pugi::xml_node keyElement, Entity * entity, Object * object, Timeline * timeline, FileFlattener * fileFlattener, SpriteKeyFileInfo * spriteKeyFileInfo, SubEntityKeyInfo *subEntityKeyInfo, point *defaultBoxPivot);

        void getMainlineFromAnimationElement(pugi::xml_node animationElement, Animation * animation, SpriteKeyFileInfoObjectIdMap * spriteKeyFileInfoMap, SubEntityKeyInfoMap *subEntityKeyInfoMap);
        void getRefsFromMainlineKeyElement(pugi::xml_node keyElement, Animation * animation, MainlineKey * mainlineKey, SpriteKeyFileInfoObjectIdMap * spriteKeyFileInfoMap, SubEntityKeyInfoMap *subEntityKeyInfoMap);

        void getEventlinesFromAnimationElement(pugi::xml_node animationElement, Entity * entity, Animation * animation);

        void getSoundlinesFromAnimationElement(pugi::xml_node animationElement, Entity * entity, Animation * animation, FileFlattener * fileFlattener);
        UniversalObjectInterface * getSoundObjectInfoFromSoundlineKey(pugi::xml_node keyElement, Entity * entity, Object *object, FileFlattener * fileFlattener, bool * soundFileFound);
    };

}
