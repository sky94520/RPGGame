require "rpg_core"
require "NonPlayerCharacter"
require "GreatMaster"
require "moon"

Map03_02Portal01 = NonPlayerCharacter:new();

function Map03_02Portal01:execute(playerID)
	-- body
	base.setGameState(GameState.Script);
	sound.playEffect("sound/se/Move1.ogg", 0);
	--淡入场景
	screen.fadeInScreen(0.8);
	--切换场景
	movement.changeMap("map/Map03_01.tmx", 5, 11);

	screen.fadeOutScreen(0.8);

	base.setGameState(GameState.Normal);
end
--剧情
Map03_02 = NonPlayerCharacter:new();

function Map03_02:update(dt)
	--是否是第一次进入
	local first = database.getValueOfKey("Map03_02First");
	--第一次 开始剧情
	if first == nil or first == true then
		database.setValueOfKey("Map03_02First", Value.Type.BOOLEAN, false);
		--剧情
		self:firstIn();
	end

end
-- 第一次运行，执行脚本
function Map03_02:firstIn()
	base.setGameState(GameState.Script);

	local playerName = party.getName(0);
	--moon移动到GreatMaster中
	local tileX, tileY = character.getTilePosition(GreatMaster.id);
	local ret, duration = character.moveToward(moon.id, tileX + 1, tileY);
	timer.delay(duration);
	--改变朝向
	character.setDirection(moon.id, Direction.Left);
	character.setDirection(GreatMaster.id, Direction.Right);
	--对话
	message.showText("moon","父亲,你回来了，怎么累成这样了？", TextPosition.Bottom, true, "img/faces/Actor1.png", 1, FacePosition.Right);
	message.showText("人族大法师","刚才魔力消耗太大了，有点体力不支，歇会就好了。", TextPosition.Bottom, true, "img/faces/People1.png", 3, FacePosition.Right);
	--气泡
	duration = character.showBalloonIcon(GreatMaster.id, "Exclamation");
	timer.delay(duration);
	character.setDirection(GreatMaster.id, Direction.Down);

	local text = string.format("%s,进来坐吧", playerName);
	message.showText("人族大法师",text, TextPosition.Bottom, true, "img/faces/People1.png", 3, FacePosition.Right);
	--主角移动
	ret, duration = character.moveToward(0, tileX, tileY + 1);
	timer.delay(duration);
	--开始对话
	message.showText(playerName,"请问一下这是哪啊？你们是谁啊？我记得我是在家里的床上睡觉的。",TextPosition.Bottom,true,"img/faces/Actor1.png",0,FacePosition.Left);

	message.showText("moon","我来介绍下，我是moon，这位是大法师，即是我的师傅，也是我的父亲。", TextPosition.Bottom, true, "img/faces/Actor1.png", 1, FacePosition.Right);
	message.showText("人族大法师","你叫我大法师就行。我预知到亡灵族会对一个人族少年有大动作，所以是专程来救你的", TextPosition.Bottom, true, "img/faces/People1.png", 3, FacePosition.Right);
	
	duration = character.showBalloonIcon(0, "Question");
	timer.delay(duration);
	message.showText(playerName,"绑票？勒索？亡灵族？我还没睡醒吧？对！这一定是做梦",TextPosition.Bottom,true,"img/faces/Actor1.png",0,FacePosition.Left);
	
	duration = character.showBalloonIcon(moon.id, "Silence");
	timer.delay(duration);
	message.showText("moon","首先，这并不是梦；其次，让我来逐个回答你提出的问题吧。", TextPosition.Bottom, true, "img/faces/Actor1.png", 1, FacePosition.Right);
	message.showText("moon","亡灵族，就是和人族对立的种族，他们一直计划着侵略人族，称霸世界。", TextPosition.Bottom, true, "img/faces/Actor1.png", 1, FacePosition.Right);
	message.showText("moon","而当前的世界并不是你原来所在的世界，它是你所在的那个世界对应的平行世界。和你所在的世界的科技时代不同，这里是一个魔法时代。", TextPosition.Bottom, true, "img/faces/Actor1.png", 1, FacePosition.Right);

	message.showText(playerName,"我穿越了？？？可亡灵族为什么要跨越位面来抓我呢？",TextPosition.Bottom,true,"img/faces/Actor1.png",0,FacePosition.Left);
	message.showText("人族大法师","这个就不清楚了。", TextPosition.Bottom, true, "img/faces/People1.png", 3, FacePosition.Right);

	message.showText(playerName,"那我要怎么回去啊？",TextPosition.Bottom,true,"img/faces/Actor1.png",0,FacePosition.Left);
	message.showText("人族大法师","其实你回去不难。不过想来死亡骑士他是不会罢手的。如果你在这个世界的话，我的魔力还能隐藏住你的踪迹。而在你回到原来的世界后，我的魔法会被削弱，踪迹的暴露只会让你更加的危险。", TextPosition.Bottom, true, "img/faces/People1.png", 3, FacePosition.Right);

	message.showText(playerName,"可我必须要回去啊，那里有我的家人、朋友和我未完成的梦想",TextPosition.Bottom,true,"img/faces/Actor1.png",0,FacePosition.Left);
	message.showText("人族大法师","想要回去有两个办法。1.打败亡灵势力 2.你先在这修炼，等你修成魔法《位面跨越》后你就可以回家，不过要注意，你必须要在魔力消耗完之前回来。第一个办法是长久之计，不过比较难实现罢了。", TextPosition.Bottom, true, "img/faces/People1.png", 3, FacePosition.Right);

	duration = character.showBalloonIcon(GreatMaster.id, "Silence");
	timer.delay(duration);
	message.showText(playerName,"好吧，我就在这里好好修炼魔法。另外，谢谢你的救命之恩。",TextPosition.Bottom,true,"img/faces/Actor1.png",0,FacePosition.Left);

	message.showText("人族大法师","嗯~。当你找到有意思的东西后，可以回来找我。", TextPosition.Bottom, true, "img/faces/People1.png", 3, FacePosition.Right);

	base.setGameState(GameState.Normal);
end