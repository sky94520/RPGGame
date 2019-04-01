require "rpg_core"
require "NonPlayerCharacter"

GreatMaster = NonPlayerCharacter:new();

function GreatMaster:initialize(id, mapName)
	-- body
	NonPlayerCharacter.initialize(self, id);
	--是否第一次进入地图Map03_01
	local first = database.getValueOfKey("Map03_01First");
	if first == nil then
		first = true;
	end
	local direction = Direction.Down;
	--在地图Map03_01进行初始化
	if mapName == "Map03_01" and first == false then
		character.setVisible(self.id, false);
	--在地图Map03_02设置朝向
	elseif mapName == "Map03_02" then
		direction = Direction.Down;
	end
	character.setDirection(self.id, direction);
	--修改行走速度
	character.setDurationPerGrid(self.id, 0.5);
end

function GreatMaster:execute(playerID)
	-- body
end
