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
    -- No relative link yet, create it to the north
    local rlink = eev_getRlink(0)
    if rlink == 0 then
        eev_addRlink(NORTH)
    end

    -- Is this a double entry ladder or a simple one?
    local simple = (eev_getRlink(1) == 0)

    -- Set the correct visual given the graph link
    if not simple then
        eev_selectAnimation("exit_main")
    elseif rlink == SOUTH then
        eev_selectAnimation("exit_end")
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
