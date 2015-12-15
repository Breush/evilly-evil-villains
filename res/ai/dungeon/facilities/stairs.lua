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
end

-- Called whenever an entity leaves in one of our tunnel
function _onEntityLeaveTunnel(UID)
    eev_setDepthUID(UID, 50)
    eev_setDetectActiveUID(UID, true)
    eev_setDetectVisibleUID(UID, true)
end

-------------
-- Routine --

-- Regular call
function _update(dt)
end

