-- This file is a test to develop a reference for dynamic AI.

-- Called with the current room information 
function evaluate_reference()
   return 3 * weight.altitude() - weight.visited()
end

-- Called with one of the neighbours of the current room
-- We try to get a node that has been least visited, and the higher one
function evaluate()
   return 2 * weight.altitude() - weight.visited()
end

