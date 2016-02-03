-------------------------
-- Facilities | Piston --
-------------------------
-- by A. Breust --

---- Description:
-- A piston can push nearby rooms into the direction it's pointing.

-----------
-- Const --

local NORTH = 0
local SOUTH = 1
local EAST  = 2
local WEST  = 3

------------
-- Locals --

local on        -- Always equal to its data homonym
local direction -- The direction the piston is pointing

---------------
-- Callbacks --

-- Called on new data
function _reinit()
    -- No blocked yet, block it then
    if not eev_hasBarrier() then
        eev_setBarrier(true)
    end

    -- Get from data
    on = eev_initEmptyDataBool("on", false)
    direction = eev_initEmptyDataU32("direction", NORTH)

    refreshDisplay(true)
end

-- Called once on object creation
function _register()
    -- Click actions
    eev_callbackClickLeftSet("cbLeftClick", "Activate")
    eev_callbackClickRightSet("cbRightClick", "Orient")

    -- In front of almost everything
    eev_setDepth(10)
end

-- Called whenever a left click occurs
function cbLeftClick()
    if on then
        deactivatePiston()
    else
        activatePiston()
    end
end

-- Called whenever a right click occurs
function cbRightClick()
    orientPiston()
end

-------------
-- Routine --

-- Regular call
function _update(dt)
end

--------------------
-- Piston control --

-- If the piston is off, turn it on
function activatePiston()
    if on then return end

    local success = false
    local x = eev_getCurrentRoomX()
    local y = eev_getCurrentRoomY()

    if direction == NORTH then      success = eev_dungeonPushRoom(x + 1, y, "north", 500)
    elseif direction == SOUTH then  success = eev_dungeonPushRoom(x - 1, y, "south", 500)
    elseif direction == WEST then   success = eev_dungeonPushRoom(x, y - 1, "west", 500)
    elseif direction == EAST then   success = eev_dungeonPushRoom(x, y + 1, "east", 500)
    end

    if not success then return end
    on = eev_setDataBool("on", true)
    refreshDisplay(false)
end

-- If the piston is on, turn it off
function deactivatePiston()
    if not on then return end

    local x = eev_getCurrentRoomX()
    local y = eev_getCurrentRoomY()

    on = eev_setDataBool("on", false)
    refreshDisplay(false)
end

-- Turn the piston clockwise
function orientPiston()
    -- Change direction
    if direction == NORTH then      direction = EAST
    elseif direction == SOUTH then  direction = WEST
    elseif direction == WEST then   direction = NORTH
    elseif direction == EAST then   direction = SOUTH
    end

    eev_setDataU32("direction", direction)

    -- Deactivate the piston
    on = eev_setDataBool("on", false)
    refreshDisplay(true)
end

-------------
-- Display --

function refreshDisplay(forward)
    if direction == NORTH then
        if on then  eev_selectAnimation("north_extend")
        else        eev_selectAnimation("north_retract")
        end
    elseif direction == SOUTH then
        if on then  eev_selectAnimation("south_extend")
        else        eev_selectAnimation("south_retract")
        end
    elseif direction == WEST then
        if on then  eev_selectAnimation("west_extend")
        else        eev_selectAnimation("west_retract")
        end
    elseif direction == EAST then
        if on then  eev_selectAnimation("east_extend")
        else        eev_selectAnimation("east_retract")
        end
    end

    if forward then
        eev_forwardAnimation(0.5)
    end
end
