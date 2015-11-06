#include "spriter/pugixmlscmlloader.hpp"

#include "tools/string.hpp"

#include <Spriter/model/spritermodel.h>
#include <Spriter/global/global.h>

#include <Spriter/timeinfo/instanteasingcurve.h>
#include <Spriter/timeinfo/lineareasingcurve.h>
#include <Spriter/timeinfo/timeinfo.h>

#include <Spriter/objectinfo/boneobjectinfo.h>
#include <Spriter/objectinfo/boxobjectinfo.h>
#include <Spriter/objectinfo/entityobjectinfo.h>
#include <Spriter/objectinfo/triggerobjectinfo.h>
#include <Spriter/objectinfo/tagobjectinfo.h>

#include <Spriter/objectref/boneref.h>
#include <Spriter/objectref/objectref.h>
#include <Spriter/objectref/spriteref.h>
#include <Spriter/objectref/entityref.h>

#include <pugixml/pugixml.hpp>

using namespace SpriterEngine;

void PugiXmlScmlLoader::loadFile(SpriterModel *model, const std::string &fileName)
{
    pugi::xml_document doc;
    doc.load_file(fileName.c_str());

    auto spriterDataElement = doc.child(L"spriter_data");
    if (spriterDataElement)
    {
        FileFlattener fileFlattener;
        getFolderFileStructureFromElement(spriterDataElement, model, fileName, &fileFlattener);
        getTagListFromElement(spriterDataElement, model);
        getEntitiesFromElement(spriterDataElement, model, &fileFlattener);
    }
    else
    {
        // error
        return;
    }
}

bool PugiXmlScmlLoader::equal(const std::string &a, const std::string &b)
{
    return a == b;
}

void PugiXmlScmlLoader::getFolderFileStructureFromElement(pugi::xml_node spriterDataElement, SpriterModel *model, std::string scmlFileName, FileFlattener *fileFlattener)
{
    std::string filePath = extractFilePath(scmlFileName);

    for (auto folderElement : spriterDataElement.children(L"folder"))
    {
        fileFlattener->appendFolder();

        for (auto fileElement : folderElement.children(L"file"))
        {
            fileFlattener->appendFile();

            std::wstring fileName = fileElement.attribute(L"name").as_string();
            // We keep it relative to the program
            // auto fileFullName = filePath + toString(fileName);
            auto fileFullName = toString(fileName);

            if (auto att = fileElement.attribute(L"type"))
            {
                if (wcscmp(att.as_string(), L"sound") == 0)
                {
                    model->pushBackSoundFile(fileFullName);
                }
            }
            else
            {
                point pivot(0., 1.);

                if (auto att = fileElement.attribute(L"width"))
                {
                    // If you need the width of the file for your implementation retrieve it here;
                }
                if (auto att = fileElement.attribute(L"height"))
                {
                    // If you need the height of the file for your implementation retrieve it here;
                }
                if (auto att = fileElement.attribute(L"pivot_x"))
                {
                    pivot.x = att.as_double();
                }
                if (auto att = fileElement.attribute(L"pivot_y"))
                {
                    pivot.y = 1. - att.as_double();
                }

                model->pushBackImageFile(fileFullName, pivot);
            }
        }
    }
}

void PugiXmlScmlLoader::getTagListFromElement(pugi::xml_node spriterDataElement, SpriterModel *model)
{
    if (auto tagListElement = spriterDataElement.child(L"tag-list"))
        for (auto tagElement : tagListElement.children()) {
            std::wstring name = tagElement.attribute(L"name").as_string();
            model->pushBackTag(toString(name));
        }
}

void PugiXmlScmlLoader::getEntitiesFromElement(pugi::xml_node spriterDataElement, SpriterModel *model, FileFlattener *fileFlattener)
{
    for (auto entityElement : spriterDataElement.children(L"entity"))
    {
        Entity* entity = getNewEntityFromEntityElement(entityElement, model);
        PointMap defaultBoxPivotMap;
        getObjectInfoFromEntityElement(entityElement, entity, &defaultBoxPivotMap);
        getVarDefArrayFromElement(entityElement, entity);
        getCharacterMapsFromEntityElement(entityElement, model, entity, fileFlattener);
        getAnimationsFromEntityElement(entityElement, model, entity, fileFlattener, &defaultBoxPivotMap);
        entity->setupDefaultVariableTimelines();
    }

}

Entity *PugiXmlScmlLoader::getNewEntityFromEntityElement(pugi::xml_node entityElement, SpriterModel *model)
{
    std::wstring name = entityElement.attribute(L"name").as_string();
    return model->pushBackEntity(toString(name));
}

void PugiXmlScmlLoader::getObjectInfoFromEntityElement(pugi::xml_node entityElement, Entity *entity, PointMap *defaultBoxPivotMap)
{
    for (auto objInfoElement : entityElement.children(L"obj_info"))
    {
        std::wstring name = objInfoElement.attribute(L"name").as_string();
        std::string objectName = toString(name);
        Object::ObjectType objectType = Object::OBJECTTYPE_SPRITE;

        if (auto att = objInfoElement.attribute(L"type"))
        {
            std::wstring type = att.as_string();
            objectType = objectTypeNameToType(toString(type));
        }

        Object* newObject = entity->setObject(objectName, objectType);
        if (newObject != nullptr)
        {
            if (objectType == Object::OBJECTTYPE_BONE || objectType == Object::OBJECTTYPE_BOX)
            {
                point size;
                if (auto att = objInfoElement.attribute(L"w"))
                {
                    size.x = att.as_double();
                }
                if (auto att = objInfoElement.attribute(L"h"))
                {
                    size.y = att.as_double();
                }
                if (auto att = objInfoElement.attribute(L"pivot_x"))
                {
                    (*defaultBoxPivotMap)[newObject->getId()].x = att.as_double();
                }
                if (auto att = objInfoElement.attribute(L"pivot_y"))
                {
                    (*defaultBoxPivotMap)[newObject->getId()].y = 1. - att.as_double();
                }
                newObject->setSize(size);
            }

            getVarDefArrayFromElement(objInfoElement, newObject);
        }
    }
}

void PugiXmlScmlLoader::getVarDefArrayFromElement(pugi::xml_node parentObjectElement, VariableContainer *parentObjectAsVariableContainer)
{
    if (auto varDefArrayElement = parentObjectElement.child(L"var_defs"))
    {
        for (auto varDefElement : varDefArrayElement.children(L"i"))
        {
            std::wstring name = varDefElement.attribute(L"name").as_string();
            std::wstring type = varDefElement.attribute(L"type").as_string();
            std::string varName = toString(name);
            std::string varType = toString(type);

            if (auto att = varDefElement.attribute(L"default"))
            {
                if (varType == "float")
                {
                    parentObjectAsVariableContainer->addRealVariable(varName, att.as_double());
                }
                else if (varType == "string")
                {
                    std::wstring string = att.as_string();
                    parentObjectAsVariableContainer->addStringVariable(varName, toString(string));
                }
                else
                {
                    parentObjectAsVariableContainer->addIntVariable(varName, att.as_int());
                }
            }
        }
    }
}

void PugiXmlScmlLoader::getCharacterMapsFromEntityElement(pugi::xml_node entityElement, SpriterModel *model, Entity *entity, FileFlattener *fileFlattener)
{
    for (auto characterMapElement : entityElement.children(L"character_map"))
    {
        std::wstring name = characterMapElement.attribute(L"name").as_string();
        CharacterMap* newCharacterMap = entity->addCharacterMap(toString(name));

        for (auto mapElement : characterMapElement.children())
        {
            int sourceFolderIndex = mapElement.attribute(L"folder").as_int();
            int sourceFileIndex = mapElement.attribute(L"file").as_int();

            int targetFolderIndex = NO_FILE;
            int targetFileIndex = NO_FILE;

            if (auto att = mapElement.attribute(L"target_folder"))
            {
                targetFolderIndex = att.as_int();
                if (auto att = mapElement.attribute(L"target_file"))
                    targetFileIndex = att.as_int();
            }

            newCharacterMap->appendMapInstruction(fileFlattener->getFlattenedIndex(sourceFolderIndex, sourceFileIndex), model->getFileAtIndex(fileFlattener->getFlattenedIndex(targetFolderIndex, targetFileIndex)));
        }
    }
}

void PugiXmlScmlLoader::getAnimationsFromEntityElement(pugi::xml_node entityElement, SpriterModel *model, Entity *entity, FileFlattener *fileFlattener, PointMap *defaultBoxPivotMap)
{
    for (auto animationElement : entityElement.children(L"animation"))
    {
        Animation* newAnimation = getNewAnimationFromAnimationElement(animationElement, entity);
        SpriteKeyFileInfoObjectIdMap spriteKeyFileInfoMap;
        SubEntityKeyInfoMap subEntityKeyInfoMap;
        getTimelinesFromAnimationElement(animationElement, model, entity, newAnimation, fileFlattener, &spriteKeyFileInfoMap, &subEntityKeyInfoMap, defaultBoxPivotMap);
        getMainlineFromAnimationElement(animationElement, newAnimation, &spriteKeyFileInfoMap, &subEntityKeyInfoMap);
        getEventlinesFromAnimationElement(animationElement, entity, newAnimation);
        getSoundlinesFromAnimationElement(animationElement, entity, newAnimation, fileFlattener);
        getMetaDataFromElement(animationElement, model, entity, newAnimation, THIS_ENTITY);
    }
}

Animation *PugiXmlScmlLoader::getNewAnimationFromAnimationElement(pugi::xml_node animationElement, Entity *entity)
{
    std::wstring name = animationElement.attribute(L"name").as_string();
    std::string animationName = toString(name);
    real animationLength = animationElement.attribute(L"length").as_double();

    bool animationLooping = true;
    if (auto att = animationElement.attribute(L"looping"))
    {
        animationLooping = att.as_bool();
    }

    return entity->pushBackAnimation(animationName, animationLength, animationLooping);
}

void PugiXmlScmlLoader::getTimelinesFromAnimationElement(pugi::xml_node animationElement, SpriterModel *model, Entity *entity, Animation *animation, FileFlattener *fileFlattener, SpriteKeyFileInfoObjectIdMap *spriteKeyFileInfoMap, SubEntityKeyInfoMap *subEntityKeyInfoMap, PointMap *defaultBoxPivotMap)
{
    int timelineIndex = 0;

    for (auto timelineElement : animationElement.children(L"timeline"))
    {
        Object* object = getObjectFromTimelineElement(timelineElement, entity);
        Timeline* newTimeline = animation->pushBackObjectTimeline(object->getId());

        SpriteKeyFileInfoVector* tempSpriteKeyFileInfoTimeline = nullptr;
        SubEntityKeyInfoVector* tempSubEntityKeyInfoTimeline = nullptr;
        point* defaultBoxPivot = nullptr;

        if (object->getType() == Object::OBJECTTYPE_SPRITE)
        {
            tempSpriteKeyFileInfoTimeline = &(*spriteKeyFileInfoMap)[object->getId()];
        }
        else if (object->getType() == Object::OBJECTTYPE_ENTITY)
        {
            tempSubEntityKeyInfoTimeline = &(*subEntityKeyInfoMap)[object->getId()];
        }
        else if (object->getType() == Object::OBJECTTYPE_BOX)
        {
            auto it = defaultBoxPivotMap->find(object->getId());
            if (it != defaultBoxPivotMap->end())
            {
                defaultBoxPivot = &(*it).second;
            }
        }

        auto firstKeyElement = timelineElement.child(L"key");
        auto keyElement = firstKeyElement;

        TimelineKey* firstKey = nullptr;
        TimelineKey* previousKey = nullptr;

        while (keyElement)
        {
            SpriteKeyFileInfo* tempSpriteKeyFileInfo = nullptr;
            SubEntityKeyInfo* tempSubEntityKeyInfo = nullptr;
            if (tempSpriteKeyFileInfoTimeline)
            {
                tempSpriteKeyFileInfoTimeline->push_back(SpriteKeyFileInfo());
                tempSpriteKeyFileInfo = &tempSpriteKeyFileInfoTimeline->back();
            }
            else if (tempSubEntityKeyInfoTimeline)
            {
                tempSubEntityKeyInfoTimeline->push_back(SubEntityKeyInfo());
                tempSubEntityKeyInfo = &tempSubEntityKeyInfoTimeline->back();
            }

            auto nextKeyElement = keyElement.next_sibling(L"key");
            TimeInfo* timeInfo = getTimeInfoFromElement(keyElement, nextKeyElement, firstKeyElement, animation->getLength(), animation->getIsLooping());
            UniversalObjectInterface* objectInfo = getObjectInfoFromTimelineKeyElement(keyElement, entity, object, newTimeline, fileFlattener, tempSpriteKeyFileInfo, tempSubEntityKeyInfo, defaultBoxPivot);

            if (previousKey)
            {
                previousKey->setNextObjectInfo(objectInfo);
            }
            previousKey = newTimeline->pushBackKey(timeInfo, objectInfo);
            if (!firstKey)
            {
                firstKey = previousKey;
            }

            keyElement = nextKeyElement;
        }

        if (previousKey)
        {
            if (animation->getIsLooping())
            {
                previousKey->setNextObjectInfo(firstKey->getObjectInfo());
            }
            else
            {
                previousKey->setNextObjectInfo(previousKey->getObjectInfo());
            }
        }

        createRedundantFirstKeys(animation, newTimeline);

        getMetaDataFromElement(timelineElement, model, entity, animation, object->getId());

        ++timelineIndex;
    }
}

void PugiXmlScmlLoader::createRedundantFirstKeys(Animation *animation, Timeline *timeline)
{
    TimelineKey *firstKey = timeline->getKey(0);
    if (firstKey && firstKey->getTime() > 0)
    {
        TimelineKey *lastKey = timeline->getLastKey();
        timeline->pushFrontProxyKey(lastKey->getTime() - animation->getLength(), firstKey->getTime(), animation->getIsLooping());
    }
}

void PugiXmlScmlLoader::getMetaDataFromElement(pugi::xml_node parentObjectElement, SpriterModel *model, Entity *entity, Animation *animation, int objectId)
{
    if (auto metaDataElement = parentObjectElement.child(L"meta"))
    {
        getVarlinesFromMetaDataElement(metaDataElement, entity, animation, objectId);
        getTaglinesFromMetaDataElement(metaDataElement, model, animation, objectId);
    }
}

void PugiXmlScmlLoader::getVarlinesFromMetaDataElement(pugi::xml_node animationElement, Entity *entity, Animation *animation, int objectId)
{
    for (auto varlineElement : animationElement.children(L"varline"))
    {
        int variableId = varlineElement.attribute(L"def").as_int();
        Timeline *newTimeline = animation->setVariableTimeline(objectId, variableId);
        Variable *variable = entity->getVariable(objectId, variableId);

        auto firstKeyElement = varlineElement.child(L"key");
        auto keyElement = firstKeyElement;

        TimelineKey *firstKey = nullptr;
        TimelineKey *previousKey = nullptr;
        while (keyElement)
        {
            pugi::xml_node nextKeyElement = keyElement.next_sibling(L"key");
            TimeInfo *timeInfo = getTimeInfoFromElement(keyElement, nextKeyElement, firstKeyElement, animation->getLength(), animation->getIsLooping());
            UniversalObjectInterface *objectInfo = getObjectInfoFromVariableKeyElement(keyElement, variable);

            if (previousKey)
            {
                previousKey->setNextObjectInfo(objectInfo);
            }
            previousKey = newTimeline->pushBackKey(timeInfo, objectInfo);
            if (!firstKey)
            {
                firstKey = previousKey;
            }

            keyElement = nextKeyElement;
        }

        if (previousKey)
        {
            if (animation->getIsLooping())
            {
                previousKey->setNextObjectInfo(firstKey->getObjectInfo());
            }
            else
            {
                previousKey->setNextObjectInfo(previousKey->getObjectInfo());
            }
        }

        createRedundantFirstKeys(animation, newTimeline);
    }
}

UniversalObjectInterface *PugiXmlScmlLoader::getObjectInfoFromVariableKeyElement(pugi::xml_node variableKeyElement, Variable *variable)
{
    auto valAtt = variableKeyElement.attribute(L"val");
    UniversalObjectInterface* objectInfo = variable->getNewObjectInfoInstance();

    switch (variable->getType())
    {
    case Variable::VARIABLETYPE_INT:
        objectInfo->setIntValue(valAtt.as_int());
        break;

    case Variable::VARIABLETYPE_REAL:
        objectInfo->setRealValue(valAtt.as_double());
        break;

    case Variable::VARIABLETYPE_STRING:
        std::wstring stringValue = valAtt.as_string();
        objectInfo->setStringValue(toString(stringValue));
        break;
    }

    return objectInfo;
}

void PugiXmlScmlLoader::getTaglinesFromMetaDataElement(pugi::xml_node metaDataElement, SpriterModel *model, Animation *animation, int objectId)
{
    for (auto taglineElement : metaDataElement.children(L"tagline"))
    {
        Timeline *newTimeline = animation->pushBackTagTimeline(objectId);

        pugi::xml_node firstKeyElement = taglineElement.child(L"key");
        pugi::xml_node keyElement = firstKeyElement;
        while (keyElement)
        {
            pugi::xml_node nextKeyElement = keyElement.next_sibling(L"key");
            TimeInfo *timeInfo = getTimeInfoFromElement(keyElement, nextKeyElement, firstKeyElement, animation->getLength(), animation->getIsLooping());
            UniversalObjectInterface *objectInfo = getObjectInfoFromTagKeyElement(keyElement, model);
            TimelineKey * newKey = newTimeline->pushBackKey(timeInfo, objectInfo);

            newKey->setNextObjectInfo(newKey->getObjectInfo());
            keyElement = nextKeyElement;
        }

        createRedundantFirstKeys(animation, newTimeline);
    }
}

UniversalObjectInterface * PugiXmlScmlLoader::getObjectInfoFromTagKeyElement(pugi::xml_node  tagKeyElement, SpriterModel *model)
{
    TagObjectInfo *tagInfo = new TagObjectInfo();

    for (auto tagElement : tagKeyElement.children())
    {
        auto tagIdAtt = tagElement.attribute(L"t");
        const auto* tag = model->getTag(tagIdAtt.as_int());
        tagInfo->pushBackTag(tag);
    }

    return tagInfo;
}

Object *PugiXmlScmlLoader::getObjectFromTimelineElement(pugi::xml_node timelineElement, Entity *entity)
{
    if (auto att = timelineElement.attribute(L"name"))
    {
        std::wstring name = att.as_string();
        std::string timelineName = toString(name);
        Object::ObjectType timelineType = Object::OBJECTTYPE_SPRITE;

        if (auto att = timelineElement.attribute(L"object_type"))
        {
            std::wstring object_type = att.as_string();
            timelineType = objectTypeNameToType(toString(object_type));
        }

        return entity->setObject(timelineName, timelineType);
    }

    return nullptr;
}

TimeInfo *PugiXmlScmlLoader::getTimeInfoFromElement(pugi::xml_node validCurrentKeyElement, pugi::xml_node nextKeyElement, pugi::xml_node validFirstKeyElement, real animationLength, bool animationLooping)
{
    real time = 0;
    real nextTime = 0;
    EasingCurveInterface *easingCurve = 0;

    if (nextKeyElement)
    {
        if (auto nextTimeAtt = nextKeyElement.attribute(L"time"))
        {
            nextTime = nextTimeAtt.as_double();
        }
    }
    else if (animationLooping)
    {
        if (validFirstKeyElement == validCurrentKeyElement)
        {
            easingCurve = new InstantEasingCurve();
        }
        else
        {
            if (auto nextTimeAtt = validFirstKeyElement.attribute(L"time"))
            {
                nextTime = nextTimeAtt.as_double();
            }
            nextTime += animationLength;
        }
    }
    else
    {
        easingCurve = new InstantEasingCurve();
    }

    if (auto att = validCurrentKeyElement.attribute(L"time"))
    {
        time = att.as_double();
    }

    if (auto att = validCurrentKeyElement.attribute(L"curve_type"))
    {
        easingCurve = getEasingCurveFromAttributes(att);
    }

    if (!easingCurve)
    {
        easingCurve = new LinearEasingCurve();
    }

    return new TimeInfo(time, nextTime, easingCurve);
}

EasingCurveInterface *PugiXmlScmlLoader::getEasingCurveFromAttributes(pugi::xml_attribute att)
{
    CurveType curveType = CURVETYPE_NONE;
    ControlPointArray controlPoints = { 0 };
    std::wstring type = att.as_string();
    curveType = curveTypeNameToType(toString(type));

    if (curveType > CURVETYPE_LINEAR)
    {
        int i = 0;
        while ((att = att.next_attribute()) && i < MAX_CONTROL_POINTS)
        {
            controlPoints[i] = att.as_int();
            ++i;
        }
    }

    return getNewEasingCurve(curveType, &controlPoints);
}

UniversalObjectInterface *PugiXmlScmlLoader::getObjectInfoFromTimelineKeyElement(pugi::xml_node keyElement, Entity *entity, Object *object, Timeline *timeline, FileFlattener *fileFlattener, SpriteKeyFileInfo *spriteKeyFileInfo, SubEntityKeyInfo *subEntityKeyInfo, point *defaultBoxPivot)
{
    int spin = 1;
    if (auto spinAtt = keyElement.attribute(L"spin"))
    {
        spin = spinAtt.as_int();
    }

    UniversalObjectInterface *objectInfo = nullptr;
    if (auto objectInfoElement = keyElement.first_child())
    {
        if (object->getType() == Object::OBJECTTYPE_SPRITE)
        {
            if (objectInfoElement.attribute(L"pivot_x"))
            {
                spriteKeyFileInfo->useDefaultPivot = false;
                objectInfo = new BoxObjectInfo();
            }
            else
            {
                spriteKeyFileInfo->useDefaultPivot = true;
                objectInfo = new BoneObjectInfo();
            }
        }
        else if (object->getType() == Object::OBJECTTYPE_ENTITY)
        {
            objectInfo = new EntityObjectInfo();
        }
        else
        {
            objectInfo = entity->getNewObjectInfoInstance(object->getId());
        }

        objectInfo->setSpin(spin);

        int folder = NO_FILE;
        int file = NO_FILE;
        int entityId = OUT_OF_RANGE;
        int animationIndex = OUT_OF_RANGE;

        point position(0, 0);
        point scale(1, 1);
        point pivot(0, 1);

        if (defaultBoxPivot)
        {
            pivot = *defaultBoxPivot;
        }

        if (auto att = objectInfoElement.attribute(L"x"))
        {
            position.x = att.as_double();
        }
        if (auto att = objectInfoElement.attribute(L"y"))
        {
            position.y = -att.as_double();
        }
        if (auto att = objectInfoElement.attribute(L"angle"))
        {
            objectInfo->setAngle(toRadians(360. - att.as_double()));
        }
        if (auto att = objectInfoElement.attribute(L"scale_x"))
        {
            scale.x = att.as_double();
        }
        if (auto att = objectInfoElement.attribute(L"scale_y"))
        {
            scale.y = att.as_double();
        }
        if (auto att = objectInfoElement.attribute(L"pivot_x"))
        {
            pivot.x = att.as_double();
        }
        if (auto att = objectInfoElement.attribute(L"pivot_y"))
        {
            pivot.y = 1. - att.as_double();
        }
        if (auto att = objectInfoElement.attribute(L"a"))
        {
            objectInfo->setAlpha(att.as_double());
        }
        if (auto att = objectInfoElement.attribute(L"folder"))
        {
            folder = att.as_int();
        }
        if (auto att = objectInfoElement.attribute(L"file"))
        {
            file = att.as_int();
        }
        if (auto att = objectInfoElement.attribute(L"entity"))
        {
            entityId = att.as_int();
        }
        if (auto att = objectInfoElement.attribute(L"animation"))
        {
            animationIndex = att.as_int();
        }
        if (auto att = objectInfoElement.attribute(L"t"))
        {
            objectInfo->setTimeRatio(att.as_double());
        }

        objectInfo->setPosition(position);
        objectInfo->setScale(scale);
        objectInfo->setPivot(pivot);

        if (spriteKeyFileInfo)
        {
            spriteKeyFileInfo->fileIndex = fileFlattener->getFlattenedIndex(folder, file);
        }
        else if (subEntityKeyInfo)
        {
            subEntityKeyInfo->entityId = entityId;
            subEntityKeyInfo->animationIndex = animationIndex;
            object->addInitializationId(entityId);
        }
    }

    return objectInfo;
}

void PugiXmlScmlLoader::getMainlineFromAnimationElement(pugi::xml_node animationElement, Animation *animation, SpriteKeyFileInfoObjectIdMap *spriteKeyFileInfoMap, SubEntityKeyInfoMap *subEntityKeyInfoMap)
{
    if (auto mainlineElement = animationElement.child(L"mainline"))
    {
        auto firstKeyElement = mainlineElement.child(L"key");
        auto keyElement = firstKeyElement;
        while (keyElement)
        {
            auto nextKeyElement = keyElement.next_sibling(L"key");
            MainlineKey *mainlineKey = animation->pushBackMainlineKey(getTimeInfoFromElement(keyElement, nextKeyElement, firstKeyElement, animation->getLength(), animation->getIsLooping()));

            getRefsFromMainlineKeyElement(keyElement, animation, mainlineKey, spriteKeyFileInfoMap, subEntityKeyInfoMap);
            keyElement = nextKeyElement;
        }
    }
}

void PugiXmlScmlLoader::getRefsFromMainlineKeyElement(pugi::xml_node keyElement, Animation *animation, MainlineKey *mainlineKey, SpriteKeyFileInfoObjectIdMap *spriteKeyFileInfoMap, SubEntityKeyInfoMap *subEntityKeyInfoMap)
{
    std::vector<int> refObjectIds;
    for (auto refElement : keyElement.children())
    {
        const int NOT_FOUND = -1;
        int keyIndex = NOT_FOUND;
        int timelineIndex = NOT_FOUND;
        int objectId = NOT_FOUND;
        int parentObjectId = THIS_ENTITY;

        if (auto att = refElement.attribute(L"parent"))
        {
            int parentRefIndex = att.as_int();
            parentObjectId = refObjectIds.at(parentRefIndex);
        }
        if (auto att = refElement.attribute(L"timeline"))
        {
            timelineIndex = att.as_int();
            objectId = animation->getObjectIdFromTimelineIndex(timelineIndex);
            refObjectIds.push_back(objectId);
        }
        if (auto att = refElement.attribute(L"key"))
        {
            keyIndex = att.as_int();
        }

        TimelineKey *timelineKey = animation->getObjectTimelineKey(timelineIndex, keyIndex);

        if (wcscmp(refElement.name(), L"bone_ref") == 0)
        {
            mainlineKey->pushBackBoneRef(new BoneRef(objectId, parentObjectId, timelineKey));
        }
        else if (wcscmp(refElement.name(), L"object_ref") == 0)
        {
            auto it = spriteKeyFileInfoMap->find(objectId);
            if (it != spriteKeyFileInfoMap->end())
            {
                SpriteKeyFileInfoVector *spriteKeyInfoVector = &(*it).second;
                SpriteKeyFileInfo *spriteKeyInfo = &spriteKeyInfoVector->at(keyIndex);
                mainlineKey->pushBackZOrderRef(new SpriteRef(objectId, parentObjectId, timelineKey, spriteKeyInfo->fileIndex, spriteKeyInfo->useDefaultPivot));
            }
            else
            {
                auto it = subEntityKeyInfoMap->find(objectId);
                if (it != subEntityKeyInfoMap->end())
                {
                    SubEntityKeyInfoVector *subEntityKeyInfoVector = &(*it).second;
                    SubEntityKeyInfo *subEntityKeyInfo = &subEntityKeyInfoVector->at(keyIndex);
                    mainlineKey->pushBackZOrderRef(new EntityRef(objectId, parentObjectId, timelineKey, subEntityKeyInfo->entityId, subEntityKeyInfo->animationIndex));
                }
                else
                {
                    mainlineKey->pushBackZOrderRef(new ObjectRef(objectId, parentObjectId, timelineKey));
                }
            }
        }
    }
}

void PugiXmlScmlLoader::getEventlinesFromAnimationElement(pugi::xml_node animationElement, Entity *entity, Animation *animation)
{
    for (auto eventlineElement : animationElement.children(L"eventline"))
    {
        std::wstring name = eventlineElement.attribute(L"name").as_string();
        auto timelineName = toString(name);

        Object *object = entity->setObject(timelineName, Object::OBJECTTYPE_TRIGGER);
        Timeline *newTimeline = animation->pushBackTriggerTimeline(object->getId());

        auto firstKeyElement = eventlineElement.child(L"key");
        auto keyElement = firstKeyElement;
        while (keyElement)
        {
            auto nextKeyElement = keyElement.next_sibling(L"key");
            TimeInfo *timeInfo = getTimeInfoFromElement(keyElement, nextKeyElement, firstKeyElement, animation->getLength(), animation->getIsLooping());
            TimelineKey * newKey = newTimeline->pushBackKey(timeInfo, new TriggerObjectInfo());
            newKey->setNextObjectInfo(newKey->getObjectInfo());

            keyElement = nextKeyElement;
        }

        createRedundantFirstKeys(animation, newTimeline);
    }
}

void PugiXmlScmlLoader::getSoundlinesFromAnimationElement(pugi::xml_node animationElement, Entity *entity, Animation *animation, FileFlattener *fileFlattener)
{
    for (auto soundlineElement : animationElement.children(L"soundline"))
    {
        std::wstring name = soundlineElement.attribute(L"name").as_string();
        auto timelineName = toString(name);

        Object *object = entity->setObject(timelineName, Object::OBJECTTYPE_SOUND);
        Timeline *newTimeline = animation->pushBackSoundTimeline(object->getId());

        TimelineKey *firstKey = 0;
        TimelineKey *previousKey = 0;

        bool soundFileFound = false;

        auto firstKeyElement = soundlineElement.child(L"key");
        auto keyElement = firstKeyElement;
        while (keyElement)
        {
            pugi::xml_node nextKeyElement = keyElement.next_sibling(L"key");
            TimeInfo *timeInfo = getTimeInfoFromElement(keyElement, nextKeyElement, firstKeyElement, animation->getLength(), animation->getIsLooping());
            UniversalObjectInterface *objectInfo = getSoundObjectInfoFromSoundlineKey(keyElement, entity, object, fileFlattener, &soundFileFound);

            if (previousKey)
            {
                previousKey->setNextObjectInfo(objectInfo);
            }

            previousKey = newTimeline->pushBackKey(timeInfo, objectInfo);

            if (!firstKey)
            {
                firstKey = previousKey;
            }

            keyElement = nextKeyElement;
        }
        if (previousKey)
        {
            if (animation->getIsLooping())
            {
                previousKey->setNextObjectInfo(firstKey->getObjectInfo());
            }
            else
            {
                previousKey->setNextObjectInfo(previousKey->getObjectInfo());
            }
        }

        createRedundantFirstKeys(animation, newTimeline);
    }
}

UniversalObjectInterface *PugiXmlScmlLoader::getSoundObjectInfoFromSoundlineKey(pugi::xml_node keyElement, Entity *entity, Object *object, FileFlattener *fileFlattener, bool *soundFileFound)
{
    if (auto objectInfoElement = keyElement.first_child())
    {
        UniversalObjectInterface *objectInfo = object->getNewObjectInfoInstance();

        int trigger = 1;
        real volume = 1;
        real panning = 0;

        if (auto att = objectInfoElement.attribute(L"volume"))
        {
            volume = att.as_double();
        }
        if (auto att = objectInfoElement.attribute(L"panning"))
        {
            panning = att.as_double();
        }
        if (auto att = objectInfoElement.attribute(L"volume"))
        {
            trigger = att.as_int();
        }
        if (auto att = objectInfoElement.attribute(L"folder"))
        {
            if (!*soundFileFound)
            {
                int folder = att.as_int();
                int file = objectInfoElement.attribute(L"file").as_int();
                object->addInitializationId(fileFlattener->getFlattenedIndex(folder, file));
            }
        }

        objectInfo->setTriggerCount(trigger);
        objectInfo->setVolume(volume);
        objectInfo->setPanning(panning);

        return objectInfo;
    }

    return nullptr;
}
