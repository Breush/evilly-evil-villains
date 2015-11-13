-------------------------
-- Facilities | Ladder --
-------------------------
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
        eev_addRtunnel(NORTH)
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

-------------
-- Routine --

-- Regular call
function _update(dt)
end

