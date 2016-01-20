-----------------------------
-- Facilities | LadderExit --
-----------------------------
-- by A. Breust --

---- Description:

------------
-- Locals --

---------------
-- Callbacks --

-- Called on new data
function _reinit()
    -- No relative tunnel yet
    if not eev_hasTunnel(0) then
        eev_addTunnel(-1, 0, true)
    end

    -- Should be hidden if the main ladder handle the graphics
    local double = eev_hasSiblingFacility("ladder")
    eev_setVisible(not double)
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

