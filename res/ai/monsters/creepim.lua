------------------------
-- Monsters | Creepim --
------------------------
-- by A. Breust --

---- Description:
-- A creepim is a easy monster,
-- who will just explode himself and the room he's in
-- if a hero comes too close. 

---------------
-- Callbacks --

-- Called once on object creation
function register()
    eev_addCallback("heroClose", "entity.type == hero && entity.distance <= 0.5");
end

-- Whenever a hero comes to close
function heroClose()
end

----------------------
-- Graph navigation --

-- The reference altitude
local altitude_ref

-- Called on initialization, dungeon invasion restarts
function init()
end

-- Called with the current node information
function evaluate_reference()
    -- Save current altitude
    altitude_ref = weight.altitude()

    -- Evaluate the current room
    -- Its possible to stay in the same room
    return 0
end

-- Called with one of the neighbours of the current node
-- Just stay in the same floor
function evaluate()
    return - (weight.altitude() - altitude_ref)^2
end

