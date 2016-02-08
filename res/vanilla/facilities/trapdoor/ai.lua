---------------------------
-- Facilities | Trapdoor --
---------------------------
-- by A. Breust --

---- Description:
-- A trapdoor that can be triggered to make a hole in the floor.

------------
-- Locals --

local pulsing = false           -- Pulsing the door (turn it on, then off)
local pulsingToOff              -- When pulsing, are we going to off?
local pulsingWaiting = false    -- When pulsing, are we waiting?
local pulsingDelay              -- When pulsing, the delay to wait with the door on
local on                        -- Always equal to its data homonym

---------------
-- Callbacks --

-- Called on new data
function _reinit()
    -- Get from data
    on = eev_initEmptyDataBool("on", false)

    refreshDisplay()
    refreshTunnel()
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

-- Called whenever an entity enters in one of our tunnel
function _onEntityEnterTunnel(UID)
end

-- Called whenever an entity leaves in one of our tunnel
function _onEntityLeaveTunnel(UID)
end

-------------
-- Routine --

-- Regular call
function _update(dt)
    if pulsing then
        if pulsingWaiting then
            pulsingDelay = pulsingDelay - dt
            if pulsingDelay <= 0 then
                pulsingWaiting = false
                closeDoor()
            end
        elseif eev_isAnimationStopped() then
            if not pulsingToOff then
                pulsingToOff = true
                pulsingWaiting = true
                pulsingDelay = 0.8
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
    refreshTunnel()
end

-- If the door is on, turn it off
function closeDoor()
    if not on then return end

    on = eev_setDataBool("on", false)
    eev_selectAnimation("close")
    refreshTunnel()
end

---------------
-- Refreshes --

function refreshTunnel()
    eev_removeTunnels()
    if on then
        local roomX = eev_getCurrentRoomX()
        local roomY = eev_getCurrentRoomY()
        eev_addTunnel(-1, 0, true)
    end
end

function refreshDisplay()
    if on then  eev_selectAnimation("opened")
    else        eev_selectAnimation("closed")
    end
end

