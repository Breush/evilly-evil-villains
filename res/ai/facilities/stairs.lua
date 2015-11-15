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
    if eev_hasLink() then
        local linkX = eev_getLinkRoomX()
        local linkY = eev_getLinkRoomY()
        local roomX = eev_getCurrentRoomX()
        local roomY = eev_getCurrentRoomY()

        -- Select the correct animation
        if linkX < roomX then
            if linkY < roomY then
                eev_selectAnimation("ldown")
            else
                eev_selectAnimation("rdown")
            end
        else
            if linkY < roomY then
                eev_selectAnimation("lup")
            else
                eev_selectAnimation("rup")
            end
        end

        -- TODO Create tunnel if none and the linked room is valid
    else
        eev_selectAnimation("dead_end")

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

