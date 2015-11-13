-----------------------------
-- Facilities | LadderExit --
-----------------------------
-- by A. Breust --

---- Description:

-----------
-- Const --

-- TODO Should they be in another file
-- containing all const?
local NORTH = 33
local SOUTH = 01
local WEST  = 16
local EAST  = 18

------------
-- Locals --

---------------
-- Callbacks --

-- Called on new data
function _reinit()
    -- No relative tunnel yet
    if eev_getRtunnel(0) == 0 then
        eev_addRtunnel(SOUTH)
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
