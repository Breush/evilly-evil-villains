-------------------
-- Traps | Razor --
-------------------
-- by A. Breust --

---- Description:
-- A trap that deals damage to very close heroes.
-- It looses some durability each time.
-- It will boost heroes with razor_boost attribute (basically Birbilon).
-- DPS: 2

------------
-- Locals --

local damaging = false      -- Are we damaging an entity?
local damagingMark = false  -- Mark if some damage occured.
local damagingCooldown = 0  -- Time to wait before next damage is possible

---------------
-- Callbacks --

-- Called on new data
function _reinit()
end

-- Called once on object creation
function _register()
    eev_callbackRegister("cbEntityClose", "hero", "distance < 0.4")
    eev_callbackRegister("cbEntityClose", "monster", "distance < 0.4")

    eev_setDepth(70)
end

-- Whenever a hero or a monster comes too close
function cbEntityClose(UID)
    if not damaging then
        eev_selectAnimation("attack")
        eev_restartAnimation()
        eev_damageUID(UID, 0.2)
        damagingMark = true
    end
end

-------------
-- Routine --

-- Regular call
function _update(dt)
    if damagingMark then
        damagingMark = false
        damagingCooldown = 0.5
        damaging = true
    end

    -- Update the damaging cooldown
    if damaging then
        damagingCooldown = damagingCooldown - dt
        if damagingCooldown <= 0 then
            damaging = false
        end
    end
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
