-------------------------
-- Facilities | Ladder --
-------------------------
-- by A. Breust --

---- Description:

------------
-- Locals --

---------------
-- Callbacks --

-- Called on new data
function _reinit()
    -- Add a tunnel only if we have a ladderExit facility in the northern room
    eev_removeTunnels()
    if eev_facilityExistsRelative(1, 0, "ladderExit") or eev_facilityExistsRelative(1, 0, "ladder") then
        eev_addTunnel(1, 0, true)
    end

    -- Set the correct visual given the graph link
    local double = eev_facilityExistsRelative(-1, 0, "ladder")
    if double then
        eev_selectAnimation("exit_main")
        eev_addTunnel(-1, 0, true)
    else
        eev_selectAnimation("main")
    end
end

-- Called once on object creation
function _register()
    -- Behind almost everything, but still in front of back wall
    eev_setDepth(99)
end

-- Called whenever an entity enters in one of our tunnel
function _onEntityEnterTunnel(UID)
    eev_setDepthUID(UID, 90)
    eev_setDetectActiveUID(UID, false)
    eev_setDetectVisibleUID(UID, false)
    eev_selectAnimationUID(UID, "climb")
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

