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

-------------
-- Routine --

-- Regular call
function _update(dt)
end

