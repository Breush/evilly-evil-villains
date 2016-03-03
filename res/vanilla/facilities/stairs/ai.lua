-------------------------
-- Facilities | Stairs --
-------------------------
-- by A. Breust --

---- Description:

------------
-- Locals --

---------------
-- Callbacks --

-- Called on new data
function _reinit()
    -- Facility stairsFront will always exist because it is an strong link on the same room
    local frontUID = eev_getSiblingFacility("stairsFront")

    -- Does our explicit link exist?
    if eev_linkExists(0) then
        local linkX, linkY
        linkX, linkY = eev_linkGet(0)

        -- TODO Do std::tuple also for this function
        local roomX = eev_getCurrentRoomX()
        local roomY = eev_getCurrentRoomY()

        -- Select the correct animation
        if linkX < roomX then
            if linkY < roomY then
                eev_selectAnimation("ldown")
                eev_selectAnimationUID(frontUID, "ldown")
            else
                eev_selectAnimation("rdown")
                eev_selectAnimationUID(frontUID, "rdown")
            end
        else
            if linkY < roomY then
                eev_selectAnimation("lup")
                eev_selectAnimationUID(frontUID, "lup")
            else
                eev_selectAnimation("rup")
                eev_selectAnimationUID(frontUID, "rup")
            end
        end

        -- Create tunnel if none
        if not eev_hasTunnel() then
            eev_addTunnel(linkX, linkY, false)
        end
    else
        -- Remove tunnel if any
        eev_selectAnimation("dead_end")
        eev_setVisibleUID(frontUID, false)
        eev_removeTunnels()
    end
end

-- Called once on object creation
function _register()
    eev_callbackClickRightSet("cbClickRight", "Set link")

    eev_setDepth(110)
end

-- Called whenever an entity enters in one of our tunnel
function _onEntityEnterTunnel(UID)
    eev_setDepthUID(UID, 105)
    eev_setDetectActiveUID(UID, false)
    eev_setDetectVisibleUID(UID, false)

    -- Add the both exits clipping
    linkX, linkY = eev_linkGet(0)
    local roomX = eev_getCurrentRoomX()
    local roomY = eev_getCurrentRoomY()

    eev_resetClipAreasUID(UID)
    eev_addClipAreaUID(UID, roomX, roomY, 1, 1)
    eev_addClipAreaUID(UID, linkX, linkY, 1, 1)

    -- FIXME What happens to the entity clipped if this tunnel entrance is destroyed while in it?
end

-- Called whenever an entity leaves in one of our tunnel
function _onEntityLeaveTunnel(UID)
    eev_setDepthUID(UID, 50)
    eev_setDetectActiveUID(UID, true)
    eev_setDetectVisibleUID(UID, true)
    eev_resetClipAreasUID(UID)
end

-- Called whenever a right click occurs
function cbClickRight()
    -- TODO
end

-------------
-- Routine --

-- Regular call
function _update(dt)
end

