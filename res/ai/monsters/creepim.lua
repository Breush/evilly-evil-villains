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

local altitude_ref  -- The reference altitude

local fusing        -- Whether the Creepim is exploding

---------------
-- Callbacks --

-- Called on new data
function _reinit()
    -- Register data
    eev_initEmptyDataFloat("fusingTime", 0)

    -- Was I fusing?
    local fusingTime = eev_getDataFloat("fusingTime")
    if fusingTime ~= 0 then
        startFusing(fusingTime)
    end
end

-- Called once on object creation
function _register()
    -- Register callbacks
    eev_addCallback("cbHeroClose", "hero", "distance < 0.7")
end

-- Whenever a hero comes too close
function cbHeroClose(heroUID)
    if not fusing then
        startFusing(0)
    end
end

-- Fusing mode
-- One can specify an time offset to apply to the animation
-- which is used to get back into fusing if it was paused
function startFusing(timeOffset)
    -- Stop doing anything else
    fusing = true
    eev_stopMoving()

    -- Select correct way to explode
    if eev_isLookingDirection("left") then
        eev_selectAnimation("lexplode")
    else
        eev_selectAnimation("rexplode")
    end

    -- Apply offset
    if timeOffset ~= 0 then
        eev_restartAnimation()
        eev_forwardAnimation(timeOffset)
    end
end

-------------
-- Routine --

-- Regular call
function _update(dt)
    -- Check if in fusing mode
    if fusing then
        -- The animation stopped, here we really explode
        if eev_isAnimationStopped() then
            eev_dungeonExplodeRoom(eev_getCurrentRoomX(), eev_getCurrentRoomY())
            fusingTime = 0
            fusing = false

        -- Else, just saved how far we got into the animation
        else
            local fusingTime = eev_getDataFloat("fusingTime")
            fusingTime = fusingTime + dt
            eev_setDataFloat("fusingTime", fusingTime)
        end
    end
end

----------------------
-- Graph navigation --

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
