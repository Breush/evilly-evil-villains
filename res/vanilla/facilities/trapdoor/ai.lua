---------------------------
-- Facilities | Trapdoor --
---------------------------
-- by A. Breust --

---- Description:
-- A trapdoor that can be triggered to make a hole in the floor.

------------
-- Locals --

local pulsing = false   -- Pulsing the door (turn it on, then off)
local pulsingToOff      -- When pulsing, are we going to off?
local on                -- Always equal to its data homonym

---------------
-- Callbacks --

-- Called on new data
function _reinit()
    -- Get from data
    on = eev_initEmptyDataBool("on", false)

    refreshDisplay()
end

-- Called once on object creation
function _register()
    eev_setDepth(60)
end

-- Called whenever an energy pulse occurs in the room we're in
function _energyOnPulse()
    if not pulsing then
        pulseDoor()
    end
end

-------------
-- Routine --

-- Regular call
function _update(dt)
    if pulsing then
        if eev_isAnimationStopped() then
            if not pulsingToOff then
                pulsingToOff = true
                closeDoor()
            else
                pulsing = false
            end
        end
    end
end

------------------
-- Door control --

-- Pulse door (turn it on, then off
function pulseDoor()
    pulsing = true

    if on then
        closeDoor()
        pulsingToOff = true
    else
        openDoor()
        pulsingToOff = false
    end
end

-- If the door is off, turn it on
function openDoor()
    if on then return end

    on = eev_setDataBool("on", true)
    eev_selectAnimation("open")
end

-- If the door is on, turn it off
function closeDoor()
    if not on then return end

    on = eev_setDataBool("on", false)
    eev_selectAnimation("close")
end

-------------
-- Display --

function refreshDisplay()
    if on then  eev_selectAnimation("opened")
    else        eev_selectAnimation("closed")
    end
end

