-----------------------
-- Traps | Pick-pock --
-----------------------
-- by A. Breust --

---- Description:
-- A Pick-pock steals dosh from nearby heroes.

------------
-- Locals --

local currentDosh       -- How much money this trap currently hold
local maxDosh = 30      -- How much money this trap can hold
local stealing = false  -- Whether the Pick-pock is already stealing

---------------
-- Callbacks --

-- Called on new data
function _reinit()
    currentDosh = eev_initEmptyDataU32("dosh", 0)
end

-- Called once on object creation
function _register()
    eev_addCallback("cbHeroClose", "hero", "distance < 0.3")

    eev_setDepth(60)
end

-- Whenever a hero comes too close
function cbHeroClose(heroUID)
    if not stealing then
        local heroDosh = eev_getUIDDataU32(heroUID, "dosh")

        -- Steal money if not full and hero has some on him
        if currentDosh < maxDosh and heroDosh ~= 0 then
            -- Stop doing anything else
            stealing = true
            eev_selectAnimation("grab")

            -- TODO How to animate the hero grabbed?

            -- Steal
            local doshStolen = math.min(maxDosh - currentDosh, heroDosh)
            eev_setUIDDataU32(heroUID, "dosh", heroDosh - doshStolen)
            currentDosh = currentDosh + doshStolen
            eev_setDataU32("dosh", currentDosh)
            eev_warnHarvestableDosh();
        end
    end
end

-------------
-- Routine --

-- Regular call
function _update(dt)
    if stealing and eev_isAnimationStopped() then
        stealing = false
        eev_selectAnimation("idle")
    end
end

-------------
-- Harvest --

-- Called for harvesting money
function _harvestDosh()
    local dosh = currentDosh
    currentDosh = eev_setDataU32("dosh", 0)
    return dosh
end

-- Called for finding out how muh money the trap holds
function _harvestableDosh()
    return currentDosh
end
