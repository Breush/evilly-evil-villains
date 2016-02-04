-------------------------
-- Facilities | Sensor --
-------------------------
-- by A. Breust --

---- Description:
-- Emits a signal whenever a moving element comes too close.
-- It will therefore signal the linked room.

------------
-- Locals --

local detecting = false -- Are we detecting anybody?

---------------
-- Callbacks --

-- Called on new data
function _reinit()
end

-- Called once on object creation
function _register()
    eev_callbackRegister("cbMovingElementClose", "hero", "distance < 0.3")
    eev_callbackRegister("cbMovingElementClose", "monster", "distance < 0.3")

    eev_setDepth(95)
end

-- Whenever a moving element comes too close
function cbMovingElementClose(UID)
    if not detecting then
        if eev_linkExists() then
            detecting = true
            eev_selectAnimation("detection")
            local linkX = eev_linkGetX()
            local linkY = eev_linkGetY()
            eev_energySendPulseRoom(linkX, linkY)
        end
    end
end

-------------
-- Routine --

-- Regular call
function _update(dt)
    -- Check if detecting
    if detecting then
        -- The animation stopped, we can re-detect
        if eev_isAnimationStopped() then
            eev_selectAnimation("idle")
            detecting = false
        end
    end
end

