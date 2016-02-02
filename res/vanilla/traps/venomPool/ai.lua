------------------------
-- Traps | Venom pool --
------------------------
-- by A. Breust --

---- Description:
-- The bathtub for the venom pool.
-- Heroes won't go inside by themself.
-- One could use a trapdoor upstairs to make a hero go down.

------------
-- Locals --

---------------
-- Callbacks --

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

-------------
-- Harvest --

-- Called for harvesting money
function _harvestDosh()
    return 0
end

-- Called for finding out how muh money the trap holds
function _harvestableDosh()
    return 0
end
