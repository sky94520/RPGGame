require "rpg_core"
require "NonPlayerCharacter"
require "GreatMaster"
--充当显示
Map03_03Merchant01 = NonPlayerCharacter:new();

function Map03_03Merchant01:initialize(id)
	-- body
	NonPlayerCharacter.initialize(self, id);
	character.setDirection(self.id, Direction.Right);
end

function Map03_03Merchant01:execute(playerID)
	-- body
end
--充当显示
Map03_03Merchant02 = NonPlayerCharacter:new();

function Map03_03Merchant02:initialize(id)
	-- body
	NonPlayerCharacter.initialize(self, id);
end

function Map03_03Merchant02:execute(playerID)
	-- body
	base.setGameState(GameState.Script);
	message.showText("旅店老板", "你好啊，冒险者", TextPosition.Bottom, true, "img/faces/People1.png", 5, FacePosition.Right);

	local exit = false;
	repeat
		local result = message.showChoices({"对话","休息（地铺）", "休息（床）","退出"});
		--判断选择
		if result == "对话" then
			message.showText("旅店老板", "大法师可是个伟大的领袖，多亏他，我们才能过上安定和谐的生活。", TextPosition.Bottom, true, "img/faces/People1.png", 5, FacePosition.Right);
		elseif result == "休息（地铺）" or result == "休息（床）" then
			--获取需要的金钱
			local gold = 20;
			local percent = 1.0;

			if result == "休息（地铺）" then
				percent = 0.6;
			end
			--先减少钱
			local ret = party.subGold(gold * percent);
			--有足够的钱
			if ret then
				message.showText("旅店老板", "一路旅途辛苦了，祝你做个好梦。", TextPosition.Bottom, true, "img/faces/People1.png", 5, FacePosition.Right);
				screen.fadeInScreen(0.8);
				--音效
				sound.playEffect("sound/se/Recovery.ogg", 0);
				party.recover(-1, 40 * percent, 40 * percent);
				screen.fadeOutScreen(0.8);

				exit = true;
			else
				local text = string.format("需要%d金币，你没有足够的钱", gold * percent);
				message.showText("旅店老板", text, TextPosition.Bottom, true, "img/faces/People1.png", 5, FacePosition.Right);
			end
			 
		elseif result == "退出" then
			exit = true;
		end
	until exit;
	base.setGameState(GameState.Normal);
end