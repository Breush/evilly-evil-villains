---------------------
-- Heroes | Luxian --
---------------------
-- by A. Breust --

---- Description:

------------
-- Locals --

---------------
-- Callbacks --

-- Called on new data
function _reinit()
end

-- Called once on object creation
function _register()
end

-- Called when this hero dies
function _onDeath()
    local rx = eev_getDataFloat("rx")
    local ry = eev_getDataFloat("ry")
    local lootUID = eev_spawnDynamic("loot", rx, ry)
    eev_setUIDDataU32(lootUID, "dosh", 1000)
    eev_setUIDDataU32(lootUID, "soul", 5)
end

-------------
-- Routine --

-- Regular call
function _update(dt)
end

----------------------
-- Graph navigation --

-- Called with the current node information
function _evaluateReference()
    return 0
end

-- Called with one of the neighbours of the current node
function _evaluate()
    return 0
end

