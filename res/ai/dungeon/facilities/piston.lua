-------------------------
-- Facilities | Piston --
-------------------------
-- by A. Breust --

---- Description:

------------
-- Locals --

local on    -- Always equal to its data homonym

---------------
-- Callbacks --

-- Called on new data
function _reinit()
    -- No blocked yet, block it then
    if not eev_hasBarrier() then
        eev_setBarrier(true)
    end

    -- Is the piston extended?
    on = eev_initEmptyDataBool("on", false)
    if on then
        eev_selectAnimation("extend")
        eev_forwardAnimation(0.5)
    else
        eev_selectAnimation("idle")
    end
end

-- Called once on object creation
function _register()
    eev_setLeftClickAction("cbLeftClick", "Activate")
    eev_setRightClickAction("cbRightClick", "Orient")

    -- In front of almost everything
    eev_setDepth(10)
end

-- Called whenever a left click occurs
function cbLeftClick()
    eev_log("Left click")
    activatePiston()
end

-- Called whenever a right click occurs
function cbRightClick()
    eev_log("Right click")
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
    if not on then
        local x = eev_getCurrentRoomX()
        local y = eev_getCurrentRoomY()

        local success = eev_dungeonPushRoom(x + 1, y, "north", 500)
        if success then
            on = true
            eev_setDataBool("on", on)
            eev_selectAnimation("extend")
        end
    end
end

-- Turn the piston counter-clockwise
function orientPiston()
    -- TODO
end
