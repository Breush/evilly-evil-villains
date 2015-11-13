------------------------------
-- Facilities | Small chest --
------------------------------
-- by A. Breust --

---- Description:

------------
-- Locals --

---------------
-- Callbacks --

-- TODO Have a callback on treasure value changed
-- so that we can adapt the display (empty/full/etc.)

-- Called on new data
function _reinit()
end

-- Called once on object creation
function _register()
    eev_setDepth(70)
end

-------------
-- Routine --

-- Regular call
function _update(dt)
end

