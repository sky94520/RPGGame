require "rpg_core"
require "NonPlayerCharacter"

moon = NonPlayerCharacter:new();

function moon:initialize(id, mapName)
	-- body
	NonPlayerCharacter.initialize(self, id);
end

function moon:execute(playerID)
	-- body
end