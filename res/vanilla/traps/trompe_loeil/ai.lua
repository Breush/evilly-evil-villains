--------------------------
-- Traps | Trompe l'œil --
--------------------------
-- by A. Breust --

---- Description:
-- A non-trap that looks deadly but is just a painting.

------------
-- Locals --

---------------
-- Callbacks --

-- Called on new data
function _reinit()
end

-- Called once on object creation
function _register()
    -- In front of wall (100) and floor (75)
    -- but behind entities (50)
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
