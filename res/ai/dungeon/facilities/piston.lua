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
    -- No blocked yet, block it then
    if not eev_hasBarrier() then
        eev_setBarrier(true)
    end

    -- Is the piston extended?
    on = eev_initEmptyDataBool("on", false)
    if on then
        eev_selectAnimation("extend")
        eev_forwardAnimation(0.5)
    else
        eev_selectAnimation("idle")
    end
end

-- Called once on object creation
function _register()
    -- FIXME This is clearly a temporary thingy,
    -- the activate state should be controlled by mouse and mechanism's signals
    -- and not that kind of callback
    eev_addCallback("cbHeroClose", "hero", "distance < 1.1")

    -- In front of almost everything
    eev_setDepth(10)
end

-- Whenever an hero comes close
function cbHeroClose()
    if not on then
        on = true
        eev_setDataBool("on", on)

        local x = eev_getCurrentRoomX()
        local y = eev_getCurrentRoomY()
        local success = eev_dungeonPushRoom(x + 1, y, "north", 500)
        if success then
            eev_selectAnimation("extend")
        end
    end
end

-------------
-- Routine --

-- Regular call
function _update(dt)
end

