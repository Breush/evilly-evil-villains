----------------------------
-- Monsters | Sand spirit --
----------------------------
-- by A. Breust --

---- Description:

------------
-- Locals --

---------------
-- Callbacks --

-- Called once on object creation
function _register()
end

-------------
-- Routine --

-- Regular call
function _update(dt)
end

----------------------
-- Graph navigation --

-- Called on initialization, dungeon invasion restarts
function _init()
end

-- Called with the current node information
function _evaluateReference()
    return 0
end

-- Called with one of the neighbours of the current node
function _evaluate()
    return -1
end