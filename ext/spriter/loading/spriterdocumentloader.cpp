#include "loading/spriterdocumentloader.h"

#include "global/settings.h"

#include "override/spriterfiledocumentwrapper.h"
#include "override/spriterfileelementwrapper.h"
#include "override/spriterfileattributewrapper.h"

#include "model/spritermodel.h"
#include "global/global.h"

#include "timeinfo/instanteasingcurve.h"
#include "timeinfo/lineareasingcurve.h"
#include "timeinfo/timeinfo.h"

#include "objectinfo/boneobjectinfo.h"
#include "objectinfo/boxobjectinfo.h"
#include "objectinfo/entityobjectinfo.h"
#include "objectinfo/triggerobjectinfo.h"
#include "objectinfo/tagobjectinfo.h"

#include "objectref/boneref.h"
#include "objectref/objectref.h"
#include "objectref/spriteref.h"
#include "objectref/entityref.h"


namespace SpriterEngine
{
    void SpriterDocumentLoader::loadFile(SpriterModel *model, SpriterFileDocumentWrapper *documentWrapper, const std::string &fileName)
    {
        documentWrapper->loadFile(fileName);

        SpriterFileElementWrapper *spriterDataElement = documentWrapper->getFirstChildElement("spriter_data");
        if (spriterDataElement->isValid())
        {
            FileFlattener fileFlattener;
            getFolderFileStructureFromElement(spriterDataElement, model, fileName, &fileFlattener);
            getTagListFromElement(spriterDataElement, model);
            getEntitiesFromElement(spriterDataElement, model, &fileFlattener);
        }
        else
        {
            Settings::error("SpriterDocumentLoader::loadFile - invalid Spriter file : missing \"spriter_data\" element");
            return;
        }
    }

    void SpriterDocumentLoader::getFolderFileStructureFromElement(SpriterFileElementWrapper *spriterDataElement, SpriterModel *model, std::string scmlFileName, FileFlattener *fileFlattener)
    {
        std::string filePath = extractFilePath(scmlFileName);
        SpriterFileElementWrapper *folderElement = spriterDataElement->getFirstChildElement("folder");
        while (folderElement->isValid())
        {
            fileFlattener->appendFolder();
            SpriterFileElementWrapper *fileElement = folderElement->getFirstChildElement("file");
            while (fileElement->isValid())
            {
                fileFlattener->appendFile();

                SpriterFileAttributeWrapper *att = fileElement->getFirstAttribute("name");
                std::string fileName;
                if (att->isValid())
                {
                    fileName = filePath + att->getStringValue();
                    att->advanceToNextAttribute();
                }
                else
                {
                    Settings::error("SpriterDocumentLoader::getFolderFileStructureFromElement - \"file\" element missing \"name\" attribute");
                    return;
                }

                if (att->isValid() && att->getName() == "type")
                {
                    if (att->getStringValue() == "sound")
                    {
                        // model->pushBackSoundFile(filePath + fileName);
                        model->pushBackSoundFile(fileName);
                    }
                }
                else
                {
                    point pivot(0, 1);
                    if (att->isValid() && att->getName() == "width")
                    {
                        att->advanceToNextAttribute();
                    }
                    if (att->isValid() && att->getName() == "height")
                    {
                        att->advanceToNextAttribute();
                    }
                    if (att->isValid() && att->getName() == "pivot_x")
                    {
                        pivot.x = att->getRealValue();
                        att->advanceToNextAttribute();
                    }
                    if (att->isValid() && att->getName() == "pivot_y")
                    {
                        pivot.y = 1 - att->getRealValue();
                        att->advanceToNextAttribute();
                    }

                    // model->pushBackImageFile(filePath + fileName, pivot);
                    model->pushBackImageFile(fileName, pivot);
                }

                fileElement->advanceToNextSiblingElementOfSameName();
            }

            folderElement->advanceToNextSiblingElementOfSameName();
        }
    }

    void SpriterDocumentLoader::getTagListFromElement(SpriterFileElementWrapper *spriterDataElement, SpriterModel *model)
    {
        SpriterFileElementWrapper *tagListElement = spriterDataElement->getFirstChildElement("tag_list");
        if (tagListElement->isValid())
        {
            SpriterFileElementWrapper *tagElement = tagListElement->getFirstChildElement();
            while (tagElement->isValid())
            {
                SpriterFileAttributeWrapper *att = tagElement->getFirstAttribute("name");
                if (att->isValid())
                {
                    model->pushBackTag(att->getStringValue());
                }
                else
                {
                    Settings::error("SpriterDocumentLoader::getTagListFromElement - \"tag_list\" element missing \"name\" attribute");
                    return;
                }

                tagElement->advanceToNextSiblingElementOfSameName();
            }

            tagListElement->advanceToNextSiblingElementOfSameName();
        }
    }

    void SpriterDocumentLoader::getEntitiesFromElement(SpriterFileElementWrapper *spriterDataElement, SpriterModel *model, FileFlattener *fileFlattener)
    {
        SpriterFileElementWrapper *entityElement = spriterDataElement->getFirstChildElement("entity");
        while (entityElement->isValid())
        {
            Entity *entity = getNewEntityFromEntityElement(entityElement, model);
            if (entity)
            {
                PointMap defaultBoxPivotMap;
                getObjectInfoFromEntityElement(entityElement, entity, &defaultBoxPivotMap);
                getVarDefArrayFromElement(entityElement, entity);
                getCharacterMapsFromEntityElement(entityElement, model, entity, fileFlattener);
                getAnimationsFromEntityElement(entityElement, model, entity, fileFlattener, &defaultBoxPivotMap);
                entity->setupDefaultVariableTimelines();
            }
            else
            {
                Settings::error("SpriterDocumentLoader::getEntitiesFromElement - unable to create entity from invalid \"entity\" element");
                return;
            }

            entityElement->advanceToNextSiblingElementOfSameName();
        }

    }

    Entity *SpriterDocumentLoader::getNewEntityFromEntityElement(SpriterFileElementWrapper *entityElement, SpriterModel *model)
    {
        SpriterFileAttributeWrapper *nameAtt = entityElement->getFirstAttribute("name");
        if (nameAtt->isValid())
        {
            return model->pushBackEntity(nameAtt->getStringValue());
        }
        else
        {
            Settings::error("SpriterDocumentLoader::getNewEntityFromEntityElement - \"entity\" element missing \"name\" attribute");
            return 0;
        }
    }

    void SpriterDocumentLoader::getObjectInfoFromEntityElement(SpriterFileElementWrapper *entityElement, Entity *entity, PointMap *defaultBoxPivotMap)
    {
        SpriterFileElementWrapper *objInfoElement = entityElement->getFirstChildElement("obj_info");
        while (objInfoElement->isValid())
        {
            SpriterFileAttributeWrapper *att = objInfoElement->getFirstAttribute("name");
            if (att->isValid())
            {
                std::string objectName = att->getStringValue();
                att->advanceToNextAttribute();

                if (att->isValid() && att->getName() == "realname")
                {
                    objectName = att->getStringValue();
                    att->advanceToNextAttribute();
                }

                Object::ObjectType objectType = Object::OBJECTTYPE_SPRITE;
                if (att->isValid() && att->getName() == "type")
                {
                    objectType = objectTypeNameToType(att->getStringValue());
                    att->advanceToNextAttribute();
                }

                Object *newObject = entity->setObject(objectName, objectType);

                if (newObject)
                {
                    if (objectType == Object::OBJECTTYPE_BONE || objectType == Object::OBJECTTYPE_BOX)
                    {
                        point size;
                        if (att->isValid() && att->getName() == "w")
                        {
                            size.x = att->getRealValue();
                            att->advanceToNextAttribute();
                        }
                        if (att->isValid() && att->getName() == "h")
                        {
                            size.y = att->getRealValue();
                            att->advanceToNextAttribute();
                        }
                        if (att->isValid() && att->getName() == "pivot_x")
                        {
                            (*defaultBoxPivotMap)[newObject->getId()].x = att->getRealValue();
                            att->advanceToNextAttribute();
                        }
                        if (att->isValid() && att->getName() == "pivot_y")
                        {
                            (*defaultBoxPivotMap)[newObject->getId()].y = 1 - att->getRealValue();
                            att->advanceToNextAttribute();
                        }

                        newObject->setSize(size);
                    }

                    getVarDefArrayFromElement(objInfoElement, newObject);
                }
                else
                {
                    Settings::error("SpriterDocumentLoader::getObjectInfoFromEntityElement - failed to create new object from \"obj_info\" element");
                }
            }
            else
            {
                Settings::error("SpriterDocumentLoader::getObjectInfoFromEntityElement - \"obj_info\" element missing \"name\" attribute");
                return;
            }

            objInfoElement->advanceToNextSiblingElementOfSameName();
        }
    }

    void SpriterDocumentLoader::getVarDefArrayFromElement(SpriterFileElementWrapper *parentObjectElement, VariableContainer *parentObjectAsVariableContainer)
    {
        SpriterFileElementWrapper *varDefArrayElement = parentObjectElement->getFirstChildElement("var_defs");
        if (varDefArrayElement->isValid())
        {
            SpriterFileElementWrapper *varDefElement = varDefArrayElement->getFirstChildElement("i");
            while (varDefElement->isValid())
            {
                SpriterFileAttributeWrapper *att = varDefElement->getFirstAttribute("name");
                if (att->isValid())
                {
                    std::string varName = att->getStringValue();
                    att->advanceToNextAttribute();

                    std::string varType = "";
                    if (att->isValid() && att->getName() == "type")
                    {
                        varType = att->getStringValue();
                        att->advanceToNextAttribute();
                    }
                    else
                    {
                        Settings::error("SpriterDocumentLoader::getVarDefArrayFromElement - sub-element of \"var_defs\" element missing \"type\" attribute");
                        return;
                    }

                    if (att->isValid() && att->getName() == "default")
                    {
                        if (varType == "float")
                        {
                            parentObjectAsVariableContainer->addRealVariable(varName, att->getRealValue());
                        }
                        else if (varType == "string")
                        {
                            parentObjectAsVariableContainer->addStringVariable(varName, att->getStringValue());
                        }
                        else
                        {
                            parentObjectAsVariableContainer->addIntVariable(varName, att->getIntValue());
                        }
                    }
                    else
                    {
                        Settings::error("SpriterDocumentLoader::getVarDefArrayFromElement - sub-element of \"var_defs\" element missing \"default\" attribute");
                        return;
                    }
                }
                else
                {
                    Settings::error("SpriterDocumentLoader::getVarDefArrayFromElement - sub-element of \"var_defs\" element missing \"name\" attribute");
                    return;
                }

                varDefElement->advanceToNextSiblingElementOfSameName();
            }
        }
    }

    void SpriterDocumentLoader::getCharacterMapsFromEntityElement(SpriterFileElementWrapper *entityElement, SpriterModel *model, Entity *entity, FileFlattener *fileFlattener)
    {
        SpriterFileElementWrapper *characterMapElement = entityElement->getFirstChildElement("character_map");

        while (characterMapElement->isValid())
        {
            SpriterFileAttributeWrapper *nameAtt = characterMapElement->getFirstAttribute("name");
            CharacterMap *newCharacterMap = 0;
            if (nameAtt->isValid())
            {
                newCharacterMap = entity->addCharacterMap(nameAtt->getStringValue());
            }
            else
            {
                Settings::error("SpriterDocumentLoader::getCharacterMapsFromEntityElement - \"character_map\" element missing \"name\" attribute");
                return;
            }

            SpriterFileElementWrapper *mapElement = characterMapElement->getFirstChildElement();
            while (mapElement->isValid())
            {
                SpriterFileAttributeWrapper *att = mapElement->getFirstAttribute();
                if (att->isValid())
                {
                    int sourceFolderIndex = NO_FILE;
                    if (att->isValid() && att->getName() == "folder")
                    {
                        sourceFolderIndex = att->getIntValue();
                        att->advanceToNextAttribute();
                    }
                    else
                    {
                        Settings::error("SpriterDocumentLoader::getCharacterMapsFromEntityElement - \"map\" element missing \"folder\" attribute");
                        return;
                    }

                    int sourceFileIndex = NO_FILE;
                    if (att->isValid() && att->getName() == "file")
                    {
                        sourceFileIndex = att->getIntValue();
                        att->advanceToNextAttribute();
                    }
                    else
                    {
                        Settings::error("SpriterDocumentLoader::getCharacterMapsFromEntityElement - \"map\" element missing \"file\" attribute");
                        return;
                    }

                    int targetFolderIndex = NO_FILE;
                    int targetFileIndex = NO_FILE;
                    if (att->isValid() && att->getName() == "target_folder")
                    {
                        targetFolderIndex = att->getIntValue();
                        att->advanceToNextAttribute();
                        if (att && att->getName() == "target_file")
                        {
                            targetFileIndex = att->getIntValue();
                        }
                    }

                    newCharacterMap->appendMapInstruction(fileFlattener->getFlattenedIndex(sourceFolderIndex, sourceFileIndex), model->getFileAtIndex(fileFlattener->getFlattenedIndex(targetFolderIndex, targetFileIndex)));
                }
                else
                {
                    Settings::error("SpriterDocumentLoader::getCharacterMapsFromEntityElement - \"map\" element has no attributes");
                    return;
                }

                mapElement->advanceToNextSiblingElementOfSameName();
            }

            characterMapElement->advanceToNextSiblingElementOfSameName();
        }
    }

    void SpriterDocumentLoader::getAnimationsFromEntityElement(SpriterFileElementWrapper *entityElement, SpriterModel *model, Entity *entity, FileFlattener *fileFlattener, PointMap *defaultBoxPivotMap)
    {
        SpriterFileElementWrapper *animationElement = entityElement->getFirstChildElement("animation");
        while (animationElement->isValid())
        {
            Animation *newAnimation = getNewAnimationFromAnimationElement(animationElement, entity);
            if (newAnimation)
            {
                SpriteKeyFileInfoObjectIdMap spriteKeyFileInfoMap;
                SubEntityKeyInfoMap subEntityKeyInfoMap;
                BooleanVector timelineRedunantKeyedVector;
                getTimelinesFromAnimationElement(animationElement, model, entity, newAnimation, fileFlattener, &spriteKeyFileInfoMap, &subEntityKeyInfoMap, defaultBoxPivotMap, &timelineRedunantKeyedVector);
                getMainlineFromAnimationElement(animationElement, newAnimation, &spriteKeyFileInfoMap, &subEntityKeyInfoMap, &timelineRedunantKeyedVector);
                getEventlinesFromAnimationElement(animationElement, entity, newAnimation);
                getSoundlinesFromAnimationElement(animationElement, entity, newAnimation, fileFlattener);
                getMetaDataFromElement(animationElement, model, entity, newAnimation, THIS_ENTITY);
            }
            else
            {
                Settings::error("SpriterDocumentLoader::getAnimationsFromEntityElement - unable to create animation from invalid \"animation\" element");
                return;
            }

            animationElement->advanceToNextSiblingElementOfSameName();
        }
    }

    Animation *SpriterDocumentLoader::getNewAnimationFromAnimationElement(SpriterFileElementWrapper *animationElement, Entity *entity)
    {
        real animationLength = 0;
        bool animationLooping = true;
        SpriterFileAttributeWrapper *att = animationElement->getFirstAttribute("name");
        if (att->isValid())
        {
            std::string animationName = att->getStringValue();
            att->advanceToNextAttribute();

            if (att->isValid() && att->getName() == "length")
            {
                animationLength = att->getRealValue();
                att->advanceToNextAttribute();
            }
            else
            {
                Settings::error("SpriterDocumentLoader::getNewAnimationFromAnimationElement - \"animation\" element missing \"length\" attribute");
                return 0;
            }

            att = animationElement->getFirstAttribute("looping");
            if (att->isValid())
            {
                animationLooping = att->getStringValue() != "false";
            }

            return entity->pushBackAnimation(animationName, animationLength, animationLooping);
        }
        else
        {
            Settings::error("SpriterDocumentLoader::getNewAnimationFromAnimationElement - \"animation\" element missing \"name\" attribute");
            return 0;
        }
    }

    void SpriterDocumentLoader::getTimelinesFromAnimationElement(SpriterFileElementWrapper *animationElement, SpriterModel *model, Entity *entity,
                                                                 Animation *animation, FileFlattener *fileFlattener, SpriteKeyFileInfoObjectIdMap *spriteKeyFileInfoMap,
                                                                 SubEntityKeyInfoMap *subEntityKeyInfoMap, PointMap *defaultBoxPivotMap, BooleanVector* timelineRedunantKeyedVector)
    {
        SpriterFileElementWrapper *timelineElement = animationElement->getFirstChildElement("timeline");
        int timelineIndex = 0;
        while (timelineElement->isValid())
        {
            Object *object = getObjectFromTimelineElement(timelineElement, entity);
            if (!object)
            {
                Settings::error("SpriterDocumentLoader::getTimelinesFromAnimationElement - unable to create object from invalid \"timeline\" element");
                return;
            }

            Timeline *newTimeline = animation->pushBackObjectTimeline(object->getId());

            SpriterFileElementWrapper *firstKeyElement = timelineElement->getFirstChildElement("key");
            SpriterFileElementWrapper *keyElement = firstKeyElement->getCloneElement();

            SpriteKeyFileInfoVector *tempSpriteKeyFileInfoTimeline = 0;
            SubEntityKeyInfoVector *tempSubEntityKeyInfoTimeline = 0;

            point *defaultBoxPivot = 0;

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
                PointMap::iterator it = defaultBoxPivotMap->find(object->getId());
                if (it != defaultBoxPivotMap->end())
                {
                    defaultBoxPivot = &(*it).second;
                }
            }

            TimelineKey *firstKey = 0;
            TimelineKey *previousKey = 0;
            while (keyElement->isValid())
            {
                SpriteKeyFileInfo *tempSpriteKeyFileInfo = 0;
                SubEntityKeyInfo *tempSubEntityKeyInfo = 0;
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

                SpriterFileElementWrapper *nextKeyElement = keyElement->getNextSiblingElement();
                TimeInfo *timeInfo = getTimeInfoFromElement(keyElement, nextKeyElement, firstKeyElement, animation->getLength(), animation->getIsLooping());
                UniversalObjectInterface *objectInfo = getObjectInfoFromTimelineKeyElement(keyElement, entity, object, newTimeline, fileFlattener, tempSpriteKeyFileInfo, tempSubEntityKeyInfo, defaultBoxPivot);

                if (!objectInfo)
                {
                    Settings::error("SpriterDocumentLoader::getTimelinesFromAnimationElement - unable to create object info from invalid \"key\" element");
                    return;
                }

                if (timeInfo)
                {
                    if (previousKey)
                    {
                        previousKey->setNextObjectInfo(objectInfo);
                    }
                    previousKey = newTimeline->pushBackKey(timeInfo, objectInfo);
                    if (!firstKey)
                    {
                        firstKey = previousKey;
                    }
                }
                else
                {
                    Settings::error("SpriterDocumentLoader::getTimelinesFromAnimationElement - unable to create time info from invalid \"key\" element");
                    return;
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

            timelineRedunantKeyedVector->push_back(createRedundantFirstKeys(animation, newTimeline));

            getMetaDataFromElement(timelineElement, model, entity, animation, object->getId());

            timelineElement->advanceToNextSiblingElementOfSameName();
            ++timelineIndex;
        }
    }

    bool SpriterDocumentLoader::createRedundantFirstKeys(Animation *animation, Timeline *timeline)
    {
        TimelineKey *firstKey = timeline->getKey(0);
        if (firstKey && firstKey->getTime() > 0)
        {
            TimelineKey *lastKey = timeline->getLastKey();
            timeline->pushFrontProxyKey(lastKey->getTime() - animation->getLength(), firstKey->getTime(), animation->getIsLooping());

            return true;
        }
        else
        {
            return false;
        }
    }

    void SpriterDocumentLoader::getMetaDataFromElement(SpriterFileElementWrapper *parentObjectElement, SpriterModel *model, Entity *entity, Animation *animation, int objectId)
    {
        SpriterFileElementWrapper *metaDataElement = parentObjectElement->getFirstChildElement("meta");
        if (metaDataElement->isValid())
        {
            getVarlinesFromMetaDataElement(metaDataElement, entity, animation, objectId);
            getTaglinesFromMetaDataElement(metaDataElement, model, animation, objectId);
        }
    }

    void SpriterDocumentLoader::getVarlinesFromMetaDataElement(SpriterFileElementWrapper *animationElement, Entity *entity, Animation *animation, int objectId)
    {
        SpriterFileElementWrapper *varlineElement = animationElement->getFirstChildElement("varline");
        while (varlineElement->isValid())
        {
            SpriterFileAttributeWrapper *att = varlineElement->getFirstAttribute("def");
            if (!att->isValid())
            {
                Settings::error("SpriterDocument::getVarlinesFromMetaDataElement - \"varline\" element missing \"def\" attribute");
                return;
            }

            int variableId = att->getIntValue();
            Timeline *newTimeline = animation->setVariableTimeline(objectId, variableId);
            Variable *variable = entity->getVariable(objectId, variableId);
            if (!variable)
            {
                Settings::error("SpriterDocument::getVarlinesFromMetaDataElement - unable to find variable with object id " + std::to_string(objectId) + " and variable id " + std::to_string(variableId));
                return;
            }

            SpriterFileElementWrapper *firstKeyElement = varlineElement->getFirstChildElement("key");
            SpriterFileElementWrapper *keyElement = firstKeyElement->getCloneElement();

            TimelineKey *firstKey = 0;
            TimelineKey *previousKey = 0;
            while (keyElement->isValid())
            {
                SpriterFileElementWrapper *nextKeyElement = keyElement->getNextSiblingElement();
                TimeInfo *timeInfo = getTimeInfoFromElement(keyElement, nextKeyElement, firstKeyElement, animation->getLength(), animation->getIsLooping());

                UniversalObjectInterface *objectInfo = getObjectInfoFromVariableKeyElement(keyElement, variable);
                if (!objectInfo)
                {
                    Settings::error("SpriterDocument::getVarlinesFromMetaDataElement - unable to create object info from invalid \"key\" element");
                    return;
                }

                if (timeInfo)
                {
                    if (previousKey)
                    {
                        previousKey->setNextObjectInfo(objectInfo);
                    }
                    previousKey = newTimeline->pushBackKey(timeInfo, objectInfo);
                    if (!firstKey)
                    {
                        firstKey = previousKey;
                    }
                }
                else
                {
                    Settings::error("SpriterDocument::getVarlinesFromMetaDataElement - unable to create time info from invalid \"key\" element");
                    return;
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
            varlineElement->advanceToNextSiblingElementOfSameName();
        }
    }

    UniversalObjectInterface *SpriterDocumentLoader::getObjectInfoFromVariableKeyElement(SpriterFileElementWrapper *variableKeyElement, Variable *variable)
    {
        SpriterFileAttributeWrapper *valAtt = variableKeyElement->getFirstAttribute("val");
        if (!valAtt->isValid())
        {
            Settings::error("SpriterDocument::getObjectInfoFromVariableKeyElement - \"key\" element missing \"val\" attribute");
            return 0;
        }

        UniversalObjectInterface *objectInfo = variable->getNewObjectInfoInstance();
        if (!objectInfo)
        {
            Settings::error("SpriterDocument::getObjectInfoFromVariableKeyElement - invalid variable unable to create object info");
            return 0;
        }

        switch (variable->getType())
        {
        case Variable::VARIABLETYPE_INT:
            objectInfo->setIntValue(valAtt->getIntValue());
            break;

        case Variable::VARIABLETYPE_REAL:
            objectInfo->setRealValue(valAtt->getRealValue());
            break;

        case Variable::VARIABLETYPE_STRING:
            objectInfo->setStringValue(valAtt->getStringValue());
            break;

        default:
            Settings::error("SpriterDocument::getObjectInfoFromVariableKeyElement - variable has invalid type");
            break;
        }

        return objectInfo;
    }

    void SpriterDocumentLoader::getTaglinesFromMetaDataElement(SpriterFileElementWrapper *metaDataElement, SpriterModel *model, Animation *animation, int objectId)
    {
        SpriterFileElementWrapper *taglineElement = metaDataElement->getFirstChildElement("tagline");
        while (taglineElement->isValid())
        {
            Timeline *newTimeline = animation->pushBackTagTimeline(objectId);

            SpriterFileElementWrapper *firstKeyElement = taglineElement->getFirstChildElement("key");
            SpriterFileElementWrapper *keyElement = firstKeyElement->getCloneElement();
            while (keyElement->isValid())
            {
                SpriterFileElementWrapper *nextKeyElement = keyElement->getNextSiblingElement();
                TimeInfo *timeInfo = getTimeInfoFromElement(keyElement, nextKeyElement, firstKeyElement, animation->getLength(), animation->getIsLooping());
                if (!timeInfo)
                {
                    Settings::error("SpriterDocument::getTaglinesFromMetaDataElement - unable to create time info from invalid \"key\" element");
                    return;
                }

                UniversalObjectInterface *objectInfo = getObjectInfoFromTagKeyElement(keyElement, model);
                if (!objectInfo)
                {
                    Settings::error("SpriterDocument::getTaglinesFromMetaDataElement - unable to create object info from invalid \"key\" element");
                    return;
                }

                TimelineKey * newKey = newTimeline->pushBackKey(timeInfo, objectInfo);
                newKey->setNextObjectInfo(newKey->getObjectInfo());

                keyElement->advanceToNextSiblingElementOfSameName();
            }

            createRedundantFirstKeys(animation, newTimeline);
            taglineElement->advanceToNextSiblingElementOfSameName();
        }
    }

    UniversalObjectInterface * SpriterDocumentLoader::getObjectInfoFromTagKeyElement(SpriterFileElementWrapper * tagKeyElement, SpriterModel *model)
    {
        TagObjectInfo *tagInfo = new TagObjectInfo();
        SpriterFileElementWrapper *tagElement = tagKeyElement->getFirstChildElement();
        while (tagElement->isValid())
        {
            SpriterFileAttributeWrapper *tagIdAtt = tagElement->getFirstAttribute("t");
            if (!tagIdAtt->isValid())
            {
                Settings::error("SpriterDocument::getObjectInfoFromTagKeyElement - \"tag\" element missing \"t\" attribute");
                return tagInfo;
            }

            const std::string *tag = model->getTag(tagIdAtt->getIntValue());
            if (!tag)
            {
                Settings::error("SpriterDocument::getObjectInfoFromTagKeyElement - tag id " + std::to_string(tagIdAtt->getIntValue()) + " not found in model");
                return tagInfo;
            }

            tagInfo->pushBackTag(tag);
            tagElement->advanceToNextSiblingElementOfSameName();
        }

        return tagInfo;
    }

    Object *SpriterDocumentLoader::getObjectFromTimelineElement(SpriterFileElementWrapper *timelineElement, Entity *entity)
    {
        std::string timelineName;
        Object::ObjectType timelineType = Object::OBJECTTYPE_SPRITE;
        SpriterFileAttributeWrapper *att = timelineElement->getFirstAttribute("name");
        if (att->isValid())
        {
            timelineName = att->getStringValue();
            att->advanceToNextAttribute();
            if (att->isValid() && att->getName() == "object_type")
            {
                timelineType = objectTypeNameToType(att->getStringValue());
            }

            return entity->setObject(timelineName, timelineType);
        }
        else
        {
            return 0;
        }
    }

    TimeInfo *SpriterDocumentLoader::getTimeInfoFromElement(SpriterFileElementWrapper *validCurrentKeyElement, SpriterFileElementWrapper *nextKeyElement, SpriterFileElementWrapper *validFirstKeyElement, real animationLength, bool animationLooping)
    {
        real time = 0;
        real nextTime = 0;
        EasingCurveInterface *easingCurve = 0;
        if (nextKeyElement->isValid())
        {
            SpriterFileAttributeWrapper *nextTimeAtt = nextKeyElement->getFirstAttribute("time");
            if (nextTimeAtt->isValid())
            {
                nextTime = nextTimeAtt->getRealValue();
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
                SpriterFileAttributeWrapper *nextTimeAtt = validFirstKeyElement->getFirstAttribute("time");
                if (nextTimeAtt->isValid())
                {
                    nextTime = nextTimeAtt->getRealValue();
                }
                nextTime += animationLength;
            }
        }
        else
        {
            easingCurve = new InstantEasingCurve();
        }

        SpriterFileAttributeWrapper *att = validCurrentKeyElement->getFirstAttribute();
        while (att->isValid())
        {
            if (att->getName() == "time")
            {
                time = att->getRealValue();
            }
            else if (!easingCurve && att->getName() == "curve_type")
            {
                easingCurve = getEasingCurveFromAttributes(att);
            }
            att->advanceToNextAttribute();
        }

        if (!easingCurve)
        {
            easingCurve = new LinearEasingCurve();
        }

        return new TimeInfo(time, nextTime, easingCurve);
    }

    EasingCurveInterface *SpriterDocumentLoader::getEasingCurveFromAttributes(SpriterFileAttributeWrapper *att)
    {
        CurveType curveType = CURVETYPE_NONE;
        ControlPointArray controlPoints = { 0 };
        curveType = curveTypeNameToType(att->getStringValue());
        if (curveType > CURVETYPE_LINEAR)
        {
            att->advanceToNextAttribute();
            int i = 0;
            while (att->isValid() && i < MAX_CONTROL_POINTS)
            {
                controlPoints[i] = att->getIntValue();
                ++i;
                att->advanceToNextAttribute();
            }
        }

        return getNewEasingCurve(curveType, &controlPoints);
    }

    UniversalObjectInterface *SpriterDocumentLoader::getObjectInfoFromTimelineKeyElement(SpriterFileElementWrapper *keyElement, Entity *entity, Object *object, Timeline *timeline, FileFlattener *fileFlattener, SpriteKeyFileInfo *spriteKeyFileInfo, SubEntityKeyInfo *subEntityKeyInfo, point *defaultBoxPivot)
    {
        int spin = 1;
        SpriterFileAttributeWrapper *spinAtt = keyElement->getFirstAttribute("spin");
        if (spinAtt->isValid())
        {
            spin = spinAtt->getIntValue();
        }

        UniversalObjectInterface *objectInfo = 0;
        SpriterFileElementWrapper *objectInfoElement = keyElement->getFirstChildElement();

        if (objectInfoElement->isValid())
        {
            if (object->getType() == Object::OBJECTTYPE_SPRITE)
            {
                spriteKeyFileInfo->useDefaultPivot = !objectInfoElement->getFirstAttribute("pivot_x")->isValid();
                if (spriteKeyFileInfo->useDefaultPivot)
                {
                    objectInfo = new BoneObjectInfo();
                }
                else
                {
                    objectInfo = new BoxObjectInfo();
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

            if (!objectInfo) objectInfo = new EntityObjectInfo();

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

            SpriterFileAttributeWrapper *att = objectInfoElement->getFirstAttribute();
            while (att->isValid())
            {
                if (att->getName() == "x")
                {
                    position.x = att->getRealValue();
                }
                else if (att->getName() == "y")
                {
                    position.y = -att->getRealValue();
                }
                else if (att->getName() == "angle")
                {
                    objectInfo->setAngle(toRadians(360 - att->getRealValue()));
                }
                else if (att->getName() == "scale_x")
                {
                    scale.x = att->getRealValue();
                }
                else if (att->getName() == "scale_y")
                {
                    scale.y = att->getRealValue();
                }
                else if (att->getName() == "pivot_x")
                {
                    pivot.x = att->getRealValue();
                }
                else if (att->getName() == "pivot_y")
                {
                    pivot.y = 1 - att->getRealValue();
                }
                else if (att->getName() == "a")
                {
                    objectInfo->setAlpha(att->getRealValue());
                }
                else if (att->getName() == "folder")
                {
                    folder = att->getIntValue();
                }
                else if (att->getName() == "file")
                {
                    file = att->getIntValue();
                }
                else if (att->getName() == "entity")
                {
                    entityId = att->getIntValue();
                }
                else if (att->getName() == "animation")
                {
                    animationIndex = att->getIntValue();
                }
                else if (att->getName() == "t")
                {
                    objectInfo->setTimeRatio(att->getRealValue());
                }

                att->advanceToNextAttribute();
            }

            Settings::suppressErrorOutput();

            objectInfo->setPosition(position);
            objectInfo->setScale(scale);
            objectInfo->setPivot(pivot);

            Settings::suppressErrorOutput(false);

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

            return objectInfo;
        }
        else
        {
            Settings::error("SpriterDocumentLoader::getObjectFromTimelineKeyElement - \"key\" element missing \"object\" or \"bone\" element");
            return 0;
        }
    }

    void SpriterDocumentLoader::getMainlineFromAnimationElement(SpriterFileElementWrapper *animationElement, Animation *animation,
                                                                SpriteKeyFileInfoObjectIdMap *spriteKeyFileInfoMap, SubEntityKeyInfoMap *subEntityKeyInfoMap,
                                                                BooleanVector* timelineRedunantKeyedVector)
    {
        SpriterFileElementWrapper *mainlineElement = animationElement->getFirstChildElement("mainline");
        if (mainlineElement->isValid())
        {
            SpriterFileElementWrapper *firstKeyElement = mainlineElement->getFirstChildElement("key");
            SpriterFileElementWrapper *keyElement = firstKeyElement->getCloneElement();
            while (keyElement->isValid())
            {
                SpriterFileElementWrapper *nextKeyElement = keyElement->getNextSiblingElement();
                MainlineKey *mainlineKey = animation->pushBackMainlineKey(getTimeInfoFromElement(keyElement, nextKeyElement, firstKeyElement, animation->getLength(), animation->getIsLooping()));

                getRefsFromMainlineKeyElement(keyElement, animation, mainlineKey, spriteKeyFileInfoMap, subEntityKeyInfoMap, timelineRedunantKeyedVector);
                keyElement->advanceToNextSiblingElementOfSameName();
            }
        }
    }

    void SpriterDocumentLoader::getRefsFromMainlineKeyElement(SpriterFileElementWrapper *keyElement, Animation *animation, MainlineKey *mainlineKey,
                                                              SpriteKeyFileInfoObjectIdMap *spriteKeyFileInfoMap, SubEntityKeyInfoMap *subEntityKeyInfoMap,
                                                              BooleanVector* timelineRedunantKeyedVector)
    {
        SpriterFileElementWrapper *refElement = keyElement->getFirstChildElement();
        std::vector<int> refObjectIds;
        while (refElement->isValid())
        {
            const int NOT_FOUND = -1;
            int keyIndex = NOT_FOUND;
            int adjustedKeyIndex = NOT_FOUND;
            int timelineIndex = NOT_FOUND;
            int objectId = NOT_FOUND;
            int parentObjectId = THIS_ENTITY;

            bool redunantKeyed = false;

            SpriterFileAttributeWrapper *att = refElement->getFirstAttribute();
            while (att->isValid())
            {
                if (att->getName() == "parent")
                {
                    int parentRefIndex = att->getIntValue();
                    if (parentRefIndex < refObjectIds.size())
                    {
                        parentObjectId = refObjectIds.at(parentRefIndex);
                    }
                    else
                    {
                        Settings::error("SpriterDocument::getRefsFromMainlineKeyElement - ref index " + std::to_string(parentRefIndex) + " from \"parent\" attribute in \"" + refElement->getName() + "\" element  out of range");
                        return;
                    }
                }
                else if (att->getName() == "timeline")
                {
                    timelineIndex = att->getIntValue();
                    objectId = animation->getObjectIdFromTimelineIndex(timelineIndex);
                    refObjectIds.push_back(objectId);


                    if (timelineIndex >= 0 && timelineIndex < timelineRedunantKeyedVector->size())
                    {
                        if (timelineRedunantKeyedVector->at(timelineIndex))
                        {
                            {
                                redunantKeyed = true;
                            }
                        }
                    }
                }
                else if (att->getName() == "key")
                {
                    keyIndex = att->getIntValue();
                    adjustedKeyIndex = keyIndex;

                    if (redunantKeyed)
                    {
                        adjustedKeyIndex++;
                    }
                }

                att->advanceToNextAttribute();
            }


            if (timelineIndex == NOT_FOUND || keyIndex == NOT_FOUND || objectId == NOT_FOUND)
            {
                Settings::error("SpriterDocument::getRefsFromMainlineKeyElement - \"" + refElement->getName() + "\" contains invalid or missing timeline or key data");
                return;
            }

            TimelineKey *timelineKey = animation->getObjectTimelineKey(timelineIndex, adjustedKeyIndex);

            if (redunantKeyed && timelineKey->getTime() > mainlineKey->getTime())
            {
                timelineKey = animation->getObjectTimelineKey(timelineIndex, 0);
            }

            if (!timelineKey)
            {
                Settings::error("SpriterDocument::getRefsFromMainlineKeyElement - \"" + refElement->getName() + "\" contains invalid or missing timeline or key data");
                return;
            }

            if (refElement->getName() == "bone_ref")
            {
                mainlineKey->pushBackBoneRef(new BoneRef(objectId, parentObjectId, timelineKey));
            }
            else if (refElement->getName() == "object_ref")
            {
                SpriteKeyFileInfoObjectIdMap::iterator it = spriteKeyFileInfoMap->find(objectId);
                if (it != spriteKeyFileInfoMap->end())
                {
                    SpriteKeyFileInfoVector *spriteKeyInfoVector = &(*it).second;
                    if (keyIndex < spriteKeyInfoVector->size())
                    {
                        SpriteKeyFileInfo *spriteKeyInfo = &spriteKeyInfoVector->at(keyIndex);
                        mainlineKey->pushBackZOrderRef(new SpriteRef(objectId, parentObjectId, timelineKey, spriteKeyInfo->fileIndex, spriteKeyInfo->useDefaultPivot));
                    }
                    else
                    {
                        Settings::error("SpriterDocument::getRefsFromMainlineKeyElement - unable to find sprite key file info for object id " + std::to_string(objectId));
                        return;
                    }
                }
                else
                {
                    SubEntityKeyInfoMap::iterator it = subEntityKeyInfoMap->find(objectId);
                    if (it != subEntityKeyInfoMap->end())
                    {
                        SubEntityKeyInfoVector *subEntityKeyInfoVector = &(*it).second;
                        if (keyIndex < subEntityKeyInfoVector->size())
                        {
                            SubEntityKeyInfo *subEntityKeyInfo = &subEntityKeyInfoVector->at(keyIndex);
                            mainlineKey->pushBackZOrderRef(new EntityRef(objectId, parentObjectId, timelineKey, subEntityKeyInfo->entityId, subEntityKeyInfo->animationIndex));
                        }
                        else
                        {
                            Settings::error("SpriterDocument::getRefsFromMainlineKeyElement - unable to find sub-entity key info for object id " + std::to_string(objectId));
                            return;
                        }
                    }
                    else
                    {
                        mainlineKey->pushBackZOrderRef(new ObjectRef(objectId, parentObjectId, timelineKey));
                    }
                }
            }

            refElement->advanceToNextSiblingElement();
        }
    }

    void SpriterDocumentLoader::getEventlinesFromAnimationElement(SpriterFileElementWrapper *animationElement, Entity *entity, Animation *animation)
    {
        SpriterFileElementWrapper *eventlineElement = animationElement->getFirstChildElement("eventline");
        while (eventlineElement->isValid())
        {
            std::string timelineName;
            SpriterFileAttributeWrapper *att = eventlineElement->getFirstAttribute("name");

            if (att->isValid())
            {
                timelineName = att->getStringValue();
            }
            else
            {
                Settings::error("SpriterDocumentLoader::getEventlinesFromAnimationElement - \"eventline\" element missing \"name\" attribute");
                return;
            }

            Object *object = entity->setObject(timelineName, Object::OBJECTTYPE_TRIGGER);
            Timeline *newTimeline = animation->pushBackTriggerTimeline(object->getId());

            SpriterFileElementWrapper *firstKeyElement = eventlineElement->getFirstChildElement("key");
            SpriterFileElementWrapper *keyElement = firstKeyElement->getCloneElement();
            while (keyElement->isValid())
            {
                SpriterFileElementWrapper *nextKeyElement = keyElement->getNextSiblingElement();
                TimeInfo *timeInfo = getTimeInfoFromElement(keyElement, nextKeyElement, firstKeyElement, animation->getLength(), animation->getIsLooping());
                if (!timeInfo)
                {
                    Settings::error("SpriterDocumentLoader::getEventlinesFromAnimationElement - unable to create time info from invalid \"key\" element");
                    return;
                }

                TimelineKey * newKey = newTimeline->pushBackKey(timeInfo, new TriggerObjectInfo());
                newKey->setNextObjectInfo(newKey->getObjectInfo());

                keyElement->advanceToNextSiblingElementOfSameName();
            }

            createRedundantFirstKeys(animation, newTimeline);
            eventlineElement->advanceToNextSiblingElementOfSameName();
        }
    }

    void SpriterDocumentLoader::getSoundlinesFromAnimationElement(SpriterFileElementWrapper *animationElement, Entity *entity, Animation *animation, FileFlattener *fileFlattener)
    {
        SpriterFileElementWrapper *soundlineElement = animationElement->getFirstChildElement("soundline");
        while (soundlineElement->isValid())
        {
            std::string timelineName;
            SpriterFileAttributeWrapper *att = soundlineElement->getFirstAttribute("name");
            if (att->isValid())
            {
                timelineName = att->getStringValue();
            }
            else
            {
                Settings::error("SpriterDocumentLoader::getSoundlinesFromAnimationElement - \"soundline\" element missing \"name\" attribute");
                return;
            }

            Object *object = entity->setObject(timelineName, Object::OBJECTTYPE_SOUND);
            Timeline *newTimeline = animation->pushBackSoundTimeline(object->getId());

            TimelineKey *firstKey = 0;
            TimelineKey *previousKey = 0;

            bool soundFileFound = false;

            SpriterFileElementWrapper *firstKeyElement = soundlineElement->getFirstChildElement("key");
            SpriterFileElementWrapper *keyElement = firstKeyElement->getCloneElement();
            while (keyElement->isValid())
            {
                SpriterFileElementWrapper *nextKeyElement = keyElement->getNextSiblingElement();
                TimeInfo *timeInfo = getTimeInfoFromElement(keyElement, nextKeyElement, firstKeyElement, animation->getLength(), animation->getIsLooping());

                UniversalObjectInterface *objectInfo = getSoundObjectInfoFromSoundlineKey(keyElement, entity, object, fileFlattener, &soundFileFound);
                if (!objectInfo)
                {
                    Settings::error("SpriterDocumentLoader::getSoundlinesFromAnimationElement - unable to create object info from invalid \"key\" element");
                    return;
                }

                if (timeInfo)
                {
                    if (previousKey)
                    {
                        previousKey->setNextObjectInfo(objectInfo);
                    }

                    previousKey = newTimeline->pushBackKey(timeInfo, objectInfo);

                    if (!firstKey)
                    {
                        firstKey = previousKey;
                    }
                }
                else
                {
                    Settings::error("SpriterDocumentLoader::getSoundlinesFromAnimationElement - unable to create time info from invalid \"key\" element");
                    return;
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
            soundlineElement->advanceToNextSiblingElementOfSameName();
        }
    }

    UniversalObjectInterface *SpriterDocumentLoader::getSoundObjectInfoFromSoundlineKey(SpriterFileElementWrapper *keyElement, Entity *entity, Object *object, FileFlattener *fileFlattener, bool *soundFileFound)
    {
        SpriterFileElementWrapper *objectInfoElement = keyElement->getFirstChildElement();
        if (objectInfoElement->isValid())
        {
            UniversalObjectInterface *objectInfo = object->getNewObjectInfoInstance();

            int trigger = 1;
            real volume = 1;
            real panning = 0;

            SpriterFileAttributeWrapper *att = objectInfoElement->getFirstAttribute();
            while (att->isValid())
            {
                if (att->getName() == "volume")
                {
                    volume = att->getRealValue();
                }
                else if (att->getName() == "panning")
                {
                    panning = att->getRealValue();
                }
                else if (att->getName() == "trigger")
                {
                    trigger = att->getIntValue();
                }
                else if (!*soundFileFound&&att->getName() == "folder")
                {
                    int folder = att->getIntValue();
                    att->advanceToNextAttribute();
                    if (att->getName() == "file")
                    {
                        object->addInitializationId(fileFlattener->getFlattenedIndex(folder, att->getIntValue()));
                    }
                    else
                    {
                        Settings::error("SpriterDocumentLoader::getSoundObjectInfoFromSoundlineKey - \"object\" element contains \"folder\" attribute, but missing \"file\" attribute");
                        return 0;
                    }
                }

                att->advanceToNextAttribute();
            }

            objectInfo->setTriggerCount(trigger);
            objectInfo->setVolume(volume);
            objectInfo->setPanning(panning);

            return objectInfo;
        }
        else
        {
            Settings::error("SpriterDocumentLoader::getSoundObjectInfoFromSoundlineKey - \"key\" missing \"object\" element");
            return 0;
        }
    }

}









