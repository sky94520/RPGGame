require "NonPlayerCharacter"
require "rpg_core"
require "Mother"
---------------------------------------剧情-----------------------------------------------
Map01_01 = NonPlayerCharacter:new();

function Map01_01:update(dt)
	--是否是第一次进入
	local first = database.getValueOfKey("Map01_01First");
	--第一次 开始剧情
	if first == nil or first == true then
		database.setValueOfKey("Map01_01First", Value.Type.BOOLEAN, false);
		--剧情
		self:firstIn();
	end

end

function Map01_01:firstIn()
	-- 第一次运行，执行脚本
	base.setGameState(GameState.Script);
	--获取主角名字
	local playerName = party.getName(0);
	--旁白
	message.showText("***","一群小鬼(初三的模样)在网吧坐在一块玩游戏", TextPosition.Middle,true);
	--开始对话
	message.showText("男孩1","对面的人怎么装备这么好啊，打不过啊，烦死了！！！", TextPosition.Bottom, true);
	message.showText("男孩2","我先溜了，骚不过，根本骚不过", TextPosition.Bottom, true);
	message.showText("男孩3","跑什么啊，继续打啊！人在塔在", TextPosition.Bottom, true);
	message.showText("男孩4","我来加血", TextPosition.Bottom, true);
	--中二少年登场
	message.showText(playerName,"我来了，看我大杀特杀！！！",TextPosition.Bottom,true,"img/faces/Actor1.png",0,FacePosition.Left);
	--音乐5杀
	--TODO
	local text = string.format("好厉害哦，不愧是%s", playerName);
	message.showText("众男",text, TextPosition.Bottom, true);
	message.showText(playerName,"嘿嘿，legendary！！！",TextPosition.Bottom,true,"img/faces/Actor1.png",0,FacePosition.Left);
	--淡入又淡出
	screen.fadeInScreen(0.8);
	screen.fadeOutScreen(0.8);
	--进门音效
	sound.playEffect("sound/se/Open1.ogg", 0);
	--切换视角
	character.setDirection(Mother.id, Direction.Up);
	character.setVisible(Mother.id, true);
	base.setViewpointFollow(Mother.id);
	--进行行走
	local ret, duration = character.moveToward(Mother.id, 9, 8);
	timer.delay(duration);
	--左看看
	character.setDirection(Mother.id, Direction.Left);
	timer.delay(0.5);
	--右看看
	character.setDirection(Mother.id, Direction.Right);
	timer.delay(0.5);
	--上看看
	character.setDirection(Mother.id, Direction.Up);
	timer.delay(0.5);
	--向左行走
	ret,duration = character.moveToward(Mother.id, 5, 9);
	timer.delay(duration);
	--等待 切换方向 行走
	for i = 1, 4 do
		--等待
		character.setDirection(Mother.id, Direction.Up);
		timer.delay(0.5);
		--行走
		ret,duration = character.moveToward(Mother.id, 5 - i, 9);
		timer.delay(duration)
	end
	--等待
	character.setDirection(Mother.id, Direction.Up);
	timer.delay(0.5);
	--人物气泡 。。。
	duration = character.showBalloonIcon(Mother.id, "Exclamation");
	timer.delay(duration);
	--对话
	message.showText("妈妈","又在打游戏，就不能好好学习吗，走！跟我回家(妈妈用手捏住了你的耳朵)",TextPosition.Bottom,true,"img/faces/People1.png",0,FacePosition.Right);
	--主角转向
	character.setDirection(0, Direction.Down);

	duration = character.showBalloonIcon(0, "Exclamation");
	timer.delay(duration);	
	message.showText(playerName,"疼，疼，疼。把这局打完行吗？眼看着这局就要赢了",TextPosition.Bottom,true,"img/faces/Actor1.png",0,FacePosition.Left);


	duration = character.showBalloonIcon(Mother.id, "Anger");
	timer.delay(duration);	
	message.showText("妈妈","少废话，快走(再次捏住了你的耳朵)",TextPosition.Bottom,true,"img/faces/People1.png",0,FacePosition.Right);

	message.showText(playerName,"哎哎哎~饶命啊",TextPosition.Bottom,true,"img/faces/Actor1.png",0,FacePosition.Left);
	--转向
	character.setDirection(0, Direction.Right);
	message.showText(playerName,"兄弟们，我有事先走了啊",TextPosition.Bottom,true,"img/faces/Actor1.png",0,FacePosition.Left);
	--众男
	message.showText("众男","哈哈，真惨", TextPosition.Bottom, true);
	--恢复默认并设置主角的朝向
	base.setViewpointFollow(0);
	character.setDirection(0, Direction.Right);
	--淡入场景
	screen.fadeInScreen(0.8);
	--切换场景
	movement.changeMap("map/Map01_02.tmx", 13, 10);
	screen.fadeOutScreen(0.8);
	timer.delay(1);
	
	base.setGameState(GameState.Normal);
end
---------------------------------------路人-----------------------------------------------
--路人1
Passerby1 = NonPlayerCharacter:new();

function Passerby1:initialize(id)
	-- body
	NonPlayerCharacter.initialize(self, id);
	character.setDirection(self.id, Direction.Up);
end

function Passerby1:execute(playerID)
	-- body

end
--路人2
Passerby2 = NonPlayerCharacter:new();

function Passerby2:initialize(id)
	-- body
	NonPlayerCharacter.initialize(self, id);
	character.setDirection(self.id, Direction.Up);
end

function Passerby2:execute(playerID)
	-- body

end
--路人3
Passerby3 = NonPlayerCharacter:new();

function Passerby3:initialize(id)
	-- body
	NonPlayerCharacter.initialize(self, id);
	character.setDirection(self.id, Direction.Up);
end

function Passerby3:execute(playerID)
	-- body

end
--路人4
Passerby4 = NonPlayerCharacter:new();

function Passerby4:initialize(id)
	-- body
	NonPlayerCharacter.initialize(self, id);
	character.setDirection(self.id, Direction.Up);
end

function Passerby4:execute(playerID)
	-- body

end