require "rpg_core"
require "NonPlayerCharacter"

Mother = NonPlayerCharacter:new();

function Mother:initialize(id, mapName)
	-- body
	NonPlayerCharacter.initialize(self, id);
	--在地图Map01_01进行初始化时，设置不可见
	if mapName == "Map01_01" then
		character.setVisible(self.id, false);
	--在地图Map01_02设置朝向
	elseif mapName == "Map01_02" then
		character.setDirection(self.id, Direction.Left);
	end
	--修改行走速度
	character.setDurationPerGrid(self.id, 0.5);
end

function Mother:execute(playerID)
	-- body
end
