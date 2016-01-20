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
    -- If no treasure yet, initialize it
    if not eev_hasTreasure() then
        local dosh = eev_borrowVillainDosh(100)
        eev_setTreasure(dosh)
    end
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

