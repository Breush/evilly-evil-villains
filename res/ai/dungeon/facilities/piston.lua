-------------------------
-- Facilities | Piston --
-------------------------
-- by A. Breust --

---- Description:

------------
-- Locals --

local on    -- Always equal to its data homonym

---------------
-- Callbacks --

-- Called on new data
function _reinit()
    on = eev_initEmptyDataBool("on", false)
end

-- Called once on object creation
function _register()
    -- FIXME This is clearly a temporary thingy,
    -- the activate state should be controlled by mouse and mechanism's signals
    -- and not that kind of callback
    eev_addCallback("cbHeroClose", "hero", "distance < 0.3")

    -- In front of almost everything
    eev_setDepth(10)
end

-- Whenever an hero comes close
function cbHeroClose()
    if not on then
        on = true
        eev_setDataBool("on", on)

        -- TODO Specify the correct coordinates!
        local x = eev_getCurrentRoomX()
        local y = eev_getCurrentRoomY()
        local success = eev_dungeonPushRoom(x + 1, y, "north", 500)
        if success then
            eev_log("PISTON Pushing successful")
        else
            eev_log("PISTON Pushing failed")
        end
    end
end

-------------
-- Routine --

-- Regular call
function _update(dt)
end

