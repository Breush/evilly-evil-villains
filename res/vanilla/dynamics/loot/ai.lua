---------------------
-- Dynamics | Loot --
---------------------
-- by A. Breust --

---- Description:
-- A simple harvestable object usually dropped by heroes on their death.
-- It can contain some resources.

---- Example of extern usage:
-- local lootUID = eev_spawnDynamic("loot", rx, ry)
-- eev_setUIDDataU32(lootUID, "dosh", 100)

------------
-- Locals --

---------------
-- Callbacks --

-- Called on new data
function _reinit()
    eev_initEmptyDataU32("dosh", 0)
    eev_initEmptyDataU32("soul", 0)
    eev_initEmptyDataU32("fame", 0)
end

-- Called once on object creation
function _register()
    -- Click actions
    eev_callbackClickLeftSet("cbLeftClick", "Harvest")

    -- In front of everything
    eev_setDepth(-50)
end

-- Called whenever a right click occurs
function cbLeftClick()
    -- Give resources to the villain
    eev_giveDosh(eev_getDataU32("dosh"))
    eev_giveSoul(eev_getDataU32("soul"))
    eev_giveFame(eev_getDataU32("fame"))

    -- Play some sound
    eev_soundPlay("core/resources/dosh/gain")

    -- Destroy itself
    eev_dynamicRemoveSelf()
end

-------------
-- Routine --

-- Regular call
function _update(dt)
end
