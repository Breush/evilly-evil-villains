-- This file is a test to develop a reference for dynamic AI.

-- The reference altitude
local altitude_ref

-- Called with the current room information 
function evaluate_reference()
    altitude_ref = weight.altitude()
    return - 3 * weight.visited()
end

-- Called with one of the neighbours of the current room
-- We try to get a node that has been least visited, and the higher one
function evaluate()
    return (weight.altitude() - altitude_ref) - 2 * weight.visited()
end

