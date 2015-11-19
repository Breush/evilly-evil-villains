--------------------
-- Heroes | Gr'oo --
--------------------
-- by A. Breust --

---- Description:

------------
-- Locals --

local altitude_ref
local visited_ref
local lastVisit_ref
local treasure_found

---------------
-- Callbacks --

-- Called on new data
function _reinit()
    -- FIXME Save this state in the eData
    treasure_found = false
end

-- Called once on object creation
function _register()
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
    -- If it is the first time we met a treasure, change state
    if (eev_weight.treasure() > 0) then
        eev_stealTreasure()
        treasure_found = true
    end

    -- Save current references
    altitude_ref = eev_weight.altitude()
    visited_ref = eev_weight.visited()
    lastVisit_ref = eev_weight.lastVisit()

    -- Try to get out if treasure found or if there is no other nodes to visit
    if eev_weight.exit() and (treasure_found or eev_weight.visited() >= 5) then
        eev_getOut()
    end

    -- Evaluate the current room
    -- Never stay in the same room
    return - 42 * eev_weight.visited()
end

-- Called with one of the neighbours of the current node
function _evaluate()
    if (treasure_found) then
        -- Treasure is found, we follow our steps back
        return lastVisit_ref - eev_weight.lastVisit()
    else
        -- We try to get a node that has been least visited, and the higher one
        return (eev_weight.altitude() - altitude_ref) - 2 * (eev_weight.visited() - visited_ref)
    end
end

