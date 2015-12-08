-------------------
-- Traps | Razor --
-------------------
-- by A. Breust --

---- Description:
-- A trap that deals damage to very close heroes.
-- It looses some durability each time.
-- It will boost heroes with razor_boost attribute (basically Birbilon).

------------
-- Locals --

---------------
-- Callbacks --

-- Called on new data
function _reinit()
end

-- Called once on object creation
function _register()
    eev_addCallback("cbEntityClose", "hero", "distance < 0.1")
    eev_addCallback("cbEntityClose", "monster", "distance < 0.1")

    eev_setDepth(70)
end

-- Whenever a hero or a monster comes too close
function cbEntityClose(UID)
    eev_damageUID(UID, 0.1);
end

-------------
-- Routine --

-- Regular call
function _update(dt)
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
