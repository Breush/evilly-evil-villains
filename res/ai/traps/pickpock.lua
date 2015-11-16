-----------------------
-- Traps | Pick-pock --
-----------------------
-- by A. Breust --

---- Description:
-- A Pick-pock steals dosh from nearby heroes.

------------
-- Locals --

local maxDosh = 30      -- How much money this trap can hold
local stealing = false  -- Whether the Pick-pock is already stealing

---------------
-- Callbacks --

-- Called once on object creation
function _register()
    eev_addCallback("cbHeroClose", "hero", "distance < 0.3")

    eev_initEmptyDataU32("dosh", 0)
end

-- Whenever a hero comes too close
function cbHeroClose(heroUID)
    if not stealing then
        local dosh = eev_getDataU32("dosh")
        local heroDosh = eev_getUIDDataU32(heroUID, "dosh")

        -- Steal money if not full and hero has some on him
        if dosh < maxDosh and heroDosh ~= 0 then
            -- Stop doing anything else
            stealing = true
            eev_selectAnimation("grab")

            -- TODO How to animate the hero grabbed?

            -- Steal
            local doshStolen = math.min(maxDosh - dosh, heroDosh)
            eev_setUIDDataU32(heroUID, "dosh", heroDosh - doshStolen)
            eev_setDataU32("dosh", doshStolen)
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
    local dosh = eev_getDataU32("dosh")
    eev_setDataU32("dosh", 0)
    return dosh
end

-- Called for finding out how muh money the trap holds
function _harvestableDosh()
    local dosh = eev_getDataU32("dosh")
    return dosh
end
