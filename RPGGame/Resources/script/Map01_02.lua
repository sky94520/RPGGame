require "NonPlayerCharacter"
require "rpg_core"
require "Mother"
---------------------------------------剧情-----------------------------------------------
Map01_02 = NonPlayerCharacter:new();

function Map01_02:update(dt)
	--是否是第一次进入
	local first = database.getValueOfKey("Map01_02First");
	--第一次 开始剧情
	if first == nil or first == true then
		database.setValueOfKey("Map01_02First", Value.Type.BOOLEAN, false);
		--剧情
		self:firstIn();
	end

end
-- 第一次运行，执行脚本
function Map01_02:firstIn()
	
	base.setGameState(GameState.Script);
	--获取主角名字
	local playerName = party.getName(0);
	--旁白
	message.showText("***","客厅-饭桌上", TextPosition.Middle, true);
	--对话
	message.showText("妈妈","你要是学习有一半玩游戏的劲头就好了",TextPosition.Bottom,true,"img/faces/People1.png",1,FacePosition.Right);
	message.showText(playerName,"我不喜欢学习嘛",TextPosition.Bottom,true,"img/faces/Actor1.png",0,FacePosition.Left);

	message.showText("妈妈","混账，不学习以后怎么有出息，吃完饭后就回屋里学习去吧(打了男孩一下)",TextPosition.Bottom,true,"img/faces/People1.png",0,FacePosition.Right);
	--打击声音
	sound.playEffect("sound/se/Blow3.ogg", 0);
	timer.delay(0.5);
	--男主气泡
	duration = character.showBalloonIcon(0, "Cobweb");
	timer.delay(duration);
	message.showText(playerName,"以后的发展那么多，学习也不是唯一的出路啊，没准我以后玩游戏、做游戏也能搞出名堂呢！？",TextPosition.Bottom,true,"img/faces/Actor1.png",0,FacePosition.Left);

	message.showText("妈妈","不可能，游戏能有什么发展。我走过的桥比你走过的路还多，我还能不知道！",TextPosition.Bottom,true,"img/faces/People1.png", 0, FacePosition.Right);
	message.showText(playerName, "时代不一样了嘛", TextPosition.Bottom, true, "img/faces/Actor1.png", 0, FacePosition.Left);

	message.showText("妈妈","你听我的没错，好了，快点吃饭吧",TextPosition.Bottom,true,"img/faces/People1.png", 0, FacePosition.Right);
	message.showText(playerName, "哼~~，我吃完了(-^-)", TextPosition.Bottom, true, "img/faces/Actor1.png", 0, FacePosition.Left);

	--寻路
	local ret, duration = character.moveToward(0, 3, 14);
	timer.delay(duration);
	--主动调用传送门1的execute函数
	Map01_02Portal01:execute(0);

	base.setGameState(GameState.Normal);
end
---------------------------------------传送门1-----------------------------------------------
Map01_02Portal01 = NonPlayerCharacter:new();

function Map01_02Portal01:execute(playerID)
	base.setGameState(GameState.Script);
	-- body
	sound.playEffect("sound/se/Move1.ogg", 0);
	--淡入场景
	screen.fadeInScreen(0.8);
	--切换场景
	movement.changeMap("map/Map01_03.tmx", 8, 11);

	screen.fadeOutScreen(0.8);

	base.setGameState(GameState.Normal);
end
