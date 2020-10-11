require "rpg_core"
require "good"

--销售卡 只是让玩家知道商店的购买比
SellCard = Good:new();
--设置基础属性
SellCard:setIconID(188);
SellCard:setName("销售卡");
SellCard:setDescription("商店会以物品50%的价钱 收购物品,可升级");
SellCard:setDeleption(false);
SellCard:setUsageRange(UsageRange.Self);
SellCard:setUsageOccasion(UsageOccasion.Normal);
SellCard:setCost(0);
--硫磺
Sulphur = Good:new();
Sulphur:setName("硫磺");
Sulphur:setDescription("需要小心保存，用途非常广泛");
Sulphur:setDeleption(false);
Sulphur:setUsageRange(UsageRange.SingleAlly);
Sulphur:setUsageOccasion(UsageOccasion.Fighting);
Sulphur:setCost(10);
--硝
Nitrate = Good:new();
Nitrate:setName("硝石");
Nitrate:setDescription("制作火药的原料之一");
Nitrate:setDeleption(false);
Nitrate:setUsageRange(UsageRange.SingleAlly);
Nitrate:setUsageOccasion(UsageOccasion.Fighting);
Nitrate:setCost(10);
--木炭
Charcoal = Good:new();
Charcoal:setName("木炭");
Charcoal:setDescription("木材经过不完全燃烧或隔绝空气下热解而生成，能用来加热物品");
Charcoal:setDeleption(false);
Charcoal:setUsageRange(UsageRange.SingleAlly);
Charcoal:setUsageOccasion(UsageOccasion.Fighting);
Charcoal:setCost(10);
--炸药 任务物品，不可出售
TNT = Good:new();
--设置基础属性
TNT:setIconID(188);
TNT:setName("炸药");
TNT:setDescription("威力巨大的炸药，能炸毁山石");
TNT:setDeleption(true);
TNT:setUsageRange(UsageRange.SingleAlly);
TNT:setUsageOccasion(UsageOccasion.Fighting);
TNT:setCost(0);

function TNT:execute(userID, targetID)
	-- body
end

-----------------------------------------药草-------------------------------
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
