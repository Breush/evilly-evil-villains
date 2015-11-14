-------------------------
-- Facilities | Stairs --
-------------------------
-- by A. Breust --

---- Description:

------------
-- Locals --

---------------
-- Callbacks --

-- Called on new data
function _reinit()
    -- Does our explicit link exist?
    if eev_linkExist() then
        -- local linkX = eev_getLinkX()
        -- local linkY = eev_getLinkY()
        -- eev_log("Found existing link in " .. linkX .. "/" .. linkY)
        eev_log("Found existing link")
        -- TODO Create tunnel if none
    else
        -- TODO Remove tunnel if any
        -- eev_removeTunnel()
    end
end

-- Called once on object creation
function _register()
    eev_setDepth(99)
end

-------------
-- Routine --

-- Regular call
function _update(dt)
end

