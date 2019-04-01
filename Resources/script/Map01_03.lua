require "rpg_core"
require "NonPlayerCharacter"

Map01_03Portal01 = NonPlayerCharacter:new();

function Map01_03Portal01:execute(playerID)
	-- body
	base.setGameState(GameState.Script);

	sound.playEffect("sound/se/Move1.ogg", 0);
	--淡入场景
	screen.fadeInScreen(0.8);
	--切换场景
	movement.changeMap("map/Map01_02.tmx", 3, 15);

	screen.fadeOutScreen(0.8);
	base.setGameState(GameState.Normal);
end
---------------------------------------剧情-----------------------------------------------
Map01_03 = NonPlayerCharacter:new();

function Map01_03:update(dt)
	--是否是第一次进入
	local first = database.getValueOfKey("Map01_03First");
	--第一次 开始剧情
	if first == nil or first == true then
		database.setValueOfKey("Map01_03First", Value.Type.BOOLEAN, false);
		--剧情
		self:firstIn();
	end

end
-- 第一次运行，执行脚本
function Map01_03:firstIn()
	base.setGameState(GameState.Script);
	--获取主角名字
	local playerName = party.getName(0);
	--行走至书桌旁边
	local ret, duration = character.moveToward(0, 8, 5);
	timer.delay(duration);
	--气泡
	duration = character.showBalloonIcon(0, "Cobweb");
	
	message.showText(playerName,"学习吧",TextPosition.Bottom,true,"img/faces/Actor1.png",0,FacePosition.Left);

	screen.fadeInScreen(0.8);
	screen.fadeOutScreen(0.8);
	--旁白
	message.showText("***","几个小时过后", TextPosition.Middle, true);
	--行走
	ret, duration = character.moveToward(0, 8, 5);
	timer.delay(duration);

	message.showText(playerName,"哎~~好累啊，该睡觉了",TextPosition.Bottom,true,"img/faces/Actor1.png",0,FacePosition.Left);
	--走到床上，睡觉
	ret, duration = character.moveToward(0, 2, 5);
	timer.delay(duration);
	--转向
	character.setDirection(0, Direction.Down);
	--睡觉气泡
	duration = character.showBalloonIcon(0, "Zzz");
	--画面转黑
	screen.fadeInScreen(0.8);
	--切换地图
	movement.changeMap("map/Map02_03.tmx", 19, 14);

	screen.fadeOutScreen(0.8);

	base.setGameState(GameState.Normal);
end