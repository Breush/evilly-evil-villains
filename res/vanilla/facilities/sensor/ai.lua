-------------------------
-- Facilities | Sensor --
-------------------------
-- by A. Breust --

---- Description:
-- Emits a signal whenever a moving element comes too close.
-- It will therefore signal the linked room.

------------
-- Locals --

---------------
-- Callbacks --

-- Called on new data
function _reinit()
end

-- Called once on object creation
function _register()
    eev_setDepth(95)
end

-------------
-- Routine --

-- Regular call
function _update(dt)
end

