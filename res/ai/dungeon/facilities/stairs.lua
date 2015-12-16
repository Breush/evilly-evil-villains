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
    -- Does our explicit link exist?
    if eev_hasLink() then
        local linkX = eev_getLinkRoomX()
        local linkY = eev_getLinkRoomY()
        local roomX = eev_getCurrentRoomX()
        local roomY = eev_getCurrentRoomY()

        -- Select the correct animation
        if linkX < roomX then
            if linkY < roomY then
                eev_selectAnimation("ldown")
            else
                eev_selectAnimation("rdown")
            end
        else
            if linkY < roomY then
                eev_selectAnimation("lup")
            else
                eev_selectAnimation("rup")
            end
        end

        -- Create tunnel if none
        if not eev_hasTunnel() then
            eev_addTunnel(linkX, linkY, false)
        end
    else
        -- Remove tunnel if any
        eev_selectAnimation("dead_end")
        eev_removeTunnels()
    end
end

-- Called once on object creation
function _register()
    eev_setDepth(110)
end

-- Called whenever an entity enters in one of our tunnel
function _onEntityEnterTunnel(UID)
    eev_setDepthUID(UID, 105)
    eev_setDetectActiveUID(UID, false)
    eev_setDetectVisibleUID(UID, false)

    -- Add the both exits clipping
    local linkX = eev_getLinkRoomX()
    local linkY = eev_getLinkRoomY()
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

-------------
-- Routine --

-- Regular call
function _update(dt)
end

