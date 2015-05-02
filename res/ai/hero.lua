-- This file is a test to develop a reference for dynamic AI.

-- The reference altitude
local altitude_ref
local visited_ref
local lastVisit_ref
local treasure_found

-- Called on initialization, dungeon invasion starts
function init()
    treasure_found = false
end

-- Called with the current node information
function evaluate_reference()
    -- If it is the first time we met a treasure, change state
    if (weight.treasure() > 0) then
        AIStealTreasure()
        treasure_found = true
    end

    -- Save current altitude
    altitude_ref = weight.altitude()
    visited_ref = weight.visited()
    lastVisit_ref = weight.lastVisit()

    -- Get out if treasure found and exit
    if (treasure_found and weight.exit()) then
        AIGetOut()
    end

    -- Evaluate the current room
    -- Never stay in the same room
    return - 42 * weight.visited()
end

-- Called with one of the neighbours of the current node
function evaluate()
    if (treasure_found) then
        -- Treasure is found, we follow our steps back
        return lastVisit_ref - weight.lastVisit()
    else
        -- We try to get a node that has been least visited, and the higher one
        return (weight.altitude() - altitude_ref) - 2 * (weight.visited() - visited_ref)
    end
end

