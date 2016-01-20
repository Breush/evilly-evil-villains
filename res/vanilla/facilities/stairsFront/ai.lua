-------------------------------
-- Facilities | Stairs front --
-------------------------------
-- by A. Breust --

---- Description:
-- The part of the stairs that are slightly in front of real stairs.
-- This makes us able to display pillar and some stairs before the background wall.

------------
-- Locals --

---------------
-- Callbacks --

-- Called on new data
function _reinit()
end

-- Called once on object creation
function _register()
    eev_setDepth(100)
end

-------------
-- Routine --

-- Regular call
function _update(dt)
end

