----------------------------
-- Monsters | Sand spirit --
----------------------------
-- by A. Breust --

---- Description:

------------
-- Locals --

local attacking = false     -- Are we attacking an entity?
local attackingMark = false -- Mark if some attack occured.
local attackingCooldown = 0 -- Time to wait before next attack is possible

---------------
-- Callbacks --

-- Called on new data
function _reinit()
end

-- Called once on object creation
function _register()
    eev_addCallback("cbHeroClose", "hero", "distance < 0.7")
end

-- Whenever a hero comes too close
function cbHeroClose(heroUID)
    if not attacking then
        eev_setMoving(false)
        eev_damageUID(heroUID, 4)
        attackingMark = true
    end
end

-------------
-- Routine --

-- Regular call
function _update(dt)
    if attackingMark then
        attackingMark = false
        attackingCooldown = 1.1
        attacking = true
    end

    -- Update the damaging cooldown
    if attacking then
        attackingCooldown = attackingCooldown - dt
        if attackingCooldown <= 0 then
            eev_setMoving(true)
            attacking = false
        end
    end
end

----------------------
-- Graph navigation --

-- Called with the current node information
function _evaluateReference()
    -- Just don't stay in the same room
    return -1
end

-- Called with one of the neighbours of the current node
function _evaluate()
    return 0
end
