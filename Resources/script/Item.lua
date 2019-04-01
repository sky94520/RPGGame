require "Good"
require "rpg_core"

Item = Good:new();
Item:setGoodType(GoodType.Item);

--血瓶
HitPointItem = Item:new();
--治愈
--self 需要有name属性 且需要有消耗品属性
--userID 使用者的id
--targetID 目标的id
--value 恢复的数值
--anim 显示的动画
function HitPointItem:execute(userID, targetID, value, anim, afterCallback)
	-- 消耗1个
	self.number = self.number - 1;
	--播放音效
	sound.playEffect("sound/se/Recovery.ogg", 0);
	--获取游戏状态
	local gameState = base.getGameState();
	--正常/脚本状态下 直接治愈
	if gameState == GameState.Normal or gameState == GameState.Script then
		battle.heal(targetID, value);
	else--战斗状态下，消耗一回合，并治愈
		local duration = battle.showBattleAnimation(targetID, anim);
		--显示文本
		message.showTip("治疗", TextPosition.Top, duration);
		--等待
		timer.delay(duration / 2);
		--治疗
		battle.heal(targetID, value);
		--调用结束函数
		if afterCallback ~= nil then
			afterCallback();
		end
		timer.delay(duration / 2);
		--回合结束
		battle.roundOver();
	end
end
-----------------------------------------药草-------------------------------
Herbs = HitPointItem:new();
Herbs:setIconID(183);
Herbs:setName("药草");
Herbs:setDescription("恢复少量HP");
Herbs:setDeleption(true);
Herbs:setUsageRange(UsageRange.SingleAlly);
Herbs:setUsageOccasion(UsageOccasion.All);
Herbs:setCost(8);

function Herbs:execute(userID, targetID)
	local value = math.random(30,40);

	HitPointItem.execute(self, userID, targetID, value, "HealOne1");
end