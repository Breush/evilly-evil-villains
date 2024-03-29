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

local pulsing = false   -- Pulsing the piston (turn it on, then off)
local pulsingToOff      -- When pulsing, are we going to off?
local on                -- Always equal to its data homonym
local direction         -- The direction the piston is pointing

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
    refreshRoomLocking()
end

-- Called once on object creation
function _register()
    -- Click actions
    eev_callbackClickLeftSet("cbLeftClick", "Activate")
    eev_callbackClickRightSet("cbRightClick", "Orient")

    -- In front of almost everything
    eev_setDepth(10)
end

-- Called whenever an energy pulse occurs in the room we're in
function _energyOnPulse()
    if not pulsing then
        pulsePiston()
    end
end

-- Called whenever a left click occurs
function cbLeftClick()
    switchPistonActivation()
end

-- Called whenever a right click occurs
function cbRightClick()
    orientPiston()
end

-------------
-- Routine --

-- Regular call
function _update(dt)
    if pulsing then
        if eev_isAnimationStopped() then
            if not pulsingToOff then
                pulsingToOff = true
                deactivatePiston()
            else
                pulsing = false
            end
        end
    end
end

--------------------
-- Piston control --

-- Pulse piston (turn it on, then off
function pulsePiston()
    pulsing = true

    if on then
        deactivatePiston()
        pulsingToOff = true
    else
        activatePiston()
        pulsingToOff = false
    end
end

-- Switch piston state
function switchPistonActivation()
    if on then
        deactivatePiston()
    else
        activatePiston()
    end
end

-- If the piston is off, turn it on
function activatePiston()
    if on then return end

    local x, y
    local success = false
    x, y = roomFront()
    success = eev_dungeonPushRoom(x, y, directionString(), 500)

    if not success then return end
    on = eev_setDataBool("on", true)
    refreshDisplay(false)
    refreshRoomLocking()
end

-- If the piston is on, turn it off
function deactivatePiston()
    if not on then return end

    local x = eev_getCurrentRoomX()
    local y = eev_getCurrentRoomY()

    on = eev_setDataBool("on", false)
    refreshDisplay(false)
    refreshRoomLocking()
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
    refreshRoomLocking()
end

-- Turn the piston into a specific direction
function turnPiston(turnDirection)
    -- Change direction
    if turnDirection == "north" then      direction = NORTH
    elseif turnDirection == "south" then  direction = SOUTH
    elseif turnDirection == "west" then   direction = WEST
    elseif turnDirection == "east" then   direction = EAST
    end

    eev_setDataU32("direction", direction)

    -- Deactivate the piston
    on = eev_setDataBool("on", false)
    refreshDisplay(true)
    refreshRoomLocking()
end

-- Find the coordinates of the room in front of the piston
function roomFront()
    local x = eev_getCurrentRoomX()
    local y = eev_getCurrentRoomY()

    if direction == NORTH then      x = x + 1
    elseif direction == SOUTH then  x = x - 1
    elseif direction == WEST then   y = y - 1
    elseif direction == EAST then   y = y + 1
    end

    return x, y
end

-- Convert the current direction to a string
function directionString()
    if direction == NORTH then      return "north"
    elseif direction == SOUTH then  return "south"
    elseif direction == WEST then   return "west"
    elseif direction == EAST then   return "east"
    end
end

---------------
-- Refreshes --

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

function refreshRoomLocking()
    eev_roomLocksClear()

    if on then
        local x, y
        x, y = roomFront()
        eev_roomLocksAdd(x, y)
    end
end
