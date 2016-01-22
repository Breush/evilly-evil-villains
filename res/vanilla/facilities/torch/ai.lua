------------------------
-- Facilities | Torch --
------------------------
-- by A. Breust --

---- Description:
-- A simple way to light the dungeon.

------------
-- Locals --

---------------
-- Callbacks --

-- Called on new data
function _reinit()
end

-- Called once on object creation
function _register()
    eev_setDepth(90)

    -- TODO Better use a point name giving the position in the Spriter file,
    -- otherwise, it might be hard to really get the coordinates working
    local lightID = eev_lightAddPoint(0, -40, 3)
    eev_lightSetColor(lightID, 255, 230, 200)
end

-------------
-- Routine --

-- Regular call
function _update(dt)
end

