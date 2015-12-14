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
    -- No relative tunnel yet
    if not eev_hasTunnel() then
        eev_addTunnel(1, 0, true)
    end

    -- Is this a double entry ladder or a simple one?
    local double = eev_hasSiblingFacility("ladderExit")

    -- Set the correct visual given the graph link
    if double then
        eev_selectAnimation("exit_main")
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
    eev_selectAnimationUID(UID, "climb")
end

-- Called whenever an entity leaves in one of our tunnel
function _onEntityLeaveTunnel(UID)
    eev_setDepthUID(UID, 50)
end

-------------
-- Routine --

-- Regular call
function _update(dt)
end

