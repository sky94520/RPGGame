require "rpg_core"
require "NonPlayerCharacter"
require "GreatMaster"
--传送门1
Map03_01Portal01 = NonPlayerCharacter:new();

function Map03_01Portal01:execute(playerID)
	-- body
	base.setGameState(GameState.Script);
	--进行一个判断TODO
	--获取主角名字
	local playerName = party.getName(0);
	message.showText(playerName,"还是先去找大法师去吧",TextPosition.Bottom,true,"img/faces/Actor1.png",0,FacePosition.Left);

	local tileX,tileY = character.getTilePosition(playerID);
	tileY = tileY - 1;

	local ret,duration = character.moveToward(playerID, tileX, tileY);
	timer.delay(duration);

	base.setGameState(GameState.Normal);
end
--传送门2
Map03_01Portal02 = NonPlayerCharacter:new();

function Map03_01Portal02:execute(playerID)
	-- body
	base.setGameState(GameState.Script);
	--进行一个判断TODO
	--获取主角名字
	local playerName = party.getName(0);
	message.showText(playerName,"还是先去找大法师去吧",TextPosition.Bottom,true,"img/faces/Actor1.png",0,FacePosition.Left);

	local tileX,tileY = character.getTilePosition(playerID);
	tileY = tileY - 1;

	local ret,duration = character.moveToward(playerID, tileX, tileY);
	timer.delay(duration);

	base.setGameState(GameState.Normal);
end
--传送门3 旅店
Map03_01Portal03 = NonPlayerCharacter:new();

function Map03_01Portal03:execute(playerID)
	-- body
	base.setGameState(GameState.Script);
	sound.playEffect("sound/se/Move1.ogg", 0);
	--淡入场景
	screen.fadeInScreen(0.8);
	movement.changeMap("map/Map03_03.tmx",10, 12);
	screen.fadeOutScreen(0.8);
	base.setGameState(GameState.Normal);
end
--传送门4 武器&盾牌
Map03_01Portal04 = NonPlayerCharacter:new();

function Map03_01Portal04:execute(playerID)
	-- body
	base.setGameState(GameState.Script);
	sound.playEffect("sound/se/Move1.ogg", 0);
	--淡入场景
	screen.fadeInScreen(0.8);
	movement.changeMap("map/Map03_04.tmx",10, 12);
	screen.fadeOutScreen(0.8);
	base.setGameState(GameState.Normal);
end
--门01
Map03_01Door01 = Door:new();

function Map03_01Door01:initialize(id)
	-- body
	Door.initialize(self, id, "Door1_4");
end

function Map03_01Door01:execute(playerID)
	-- body
	base.setGameState(GameState.Script);
	Door.execute(self, playerID, "map/Map03_02.tmx", 9, 15);
	base.setGameState(GameState.Normal);
end
--剧情
Map03_01 = NonPlayerCharacter:new();

function Map03_01:update(dt)
	--是否是第一次进入
	local first = database.getValueOfKey("Map03_01First");
	--第一次 开始剧情
	if first == nil or first == true then
		database.setValueOfKey("Map03_01First", Value.Type.BOOLEAN, false);
		--剧情
		self:firstIn();
	end

end
-- 第一次运行，执行脚本
function Map03_01:firstIn()
	base.setGameState(GameState.Script);
	--转移中心点从主角到大法师
	local x,y = character.getPosition(GreatMaster.id);
	base.setViewpointFollow(GreatMaster.id);
	base.setViewpointCenter(x, y, 0.5);
	--获取主角名字
	local playerName = party.getName(0);
	--对话
	message.showText("人族大法师","（很累的样子）太好了，刚好赶上了", TextPosition.Bottom, true, "img/faces/People1.png", 3, FacePosition.Right);
	--人物气泡
	timer.delay(character.showBalloonIcon(0, "Exclamation"));
	--主角转向
	character.setDirection(0, Direction.Left);
	message.showText(playerName,"（吓坐在地上）你是谁啊？我怎么会在这里",TextPosition.Bottom,true,"img/faces/Actor1.png",0,FacePosition.Left);

	character.setDirection(GreatMaster.id, Direction.Right);
	message.showText("人族大法师","等等再说，我在那边的房子等你。", TextPosition.Bottom, true, "img/faces/People1.png", 3, FacePosition.Right);
	--行走至房门口
	local ret,duration = character.moveToward(GreatMaster.id, 5, 11);
	timer.delay(duration);
	--开门声
	sound.playEffect("sound/se/Open1.ogg", 0);
	--消失
	character.setVisible(GreatMaster.id, false);
	--转移目标
	--镜头转移
	x,y = character.getPosition(0);
	base.setViewpointCenter(x, y, 0.6);
	base.setViewpointFollow(0);

	base.setGameState(GameState.Normal);
end