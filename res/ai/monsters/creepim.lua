------------------------
-- Monsters | Creepim --
------------------------
-- by A. Breust --

---- Description:
-- A creepim is an simple monster,
-- who will just explode himself and the room he's in
-- if a hero comes too close.

------------
-- Locals --

local altitude_ref      -- The reference altitude

local fusing = false    -- Whether the Creepim is exploding

---------------
-- Callbacks --

-- Called once on object creation
function _register()
    eev_addCallback("cbHeroClose", "hero", "distance < 0.7")
end

-- Whenever a hero comes too close
function cbHeroClose()
    if not fusing then
        -- Stop doing anything else
        fusing = true
        eev_stopMoving()

        -- Select correct way to explode
        eev_setAnimationLooping(false)
        if eev_isLookingDirection("left") then
            eev_selectAnimation("lexplode")
        else
            eev_selectAnimation("rexplode")
        end
    end
end

-------------
-- Routine --

-- Regular call
function _update(dt)
    if fusing and eev_isAnimationStopped() then
        eev_dungeonExplodeRoom(eev_getCurrentRoomX(), eev_getCurrentRoomY())
        fusing = false;
    end
end

----------------------
-- Graph navigation --

-- Called on initialization, dungeon invasion restarts
function _init()
end

-- Called with the current node information
function _evaluateReference()
    -- Save current altitude
    altitude_ref = eev_weight.altitude()

    -- Evaluate the current room
    -- It's possible to stay in the same room
    return 0
end

-- Called with one of the neighbours of the current node
-- Just stay in the same floor
function _evaluate()
    return - (eev_weight.altitude() - altitude_ref)^2
end
