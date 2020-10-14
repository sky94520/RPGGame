require "good"
require "rpg_core"
require "item"
require "weapon"

Skill = Good:new();
Skill:setGoodType(GoodType.Skill);
Skill:setDeleption(false);
--消耗mp
--userID 使用者ID
--delta 消耗点数
function Skill.consumeMP(userID, delta)
	local value = battle.getProperty(userID, PropertyType.Mp) - delta;
	--使用失败 提示
	if value < 0 then
		message.showTip("MP不足，技能使用失败", 1.5);
		return false;
	end
	--消耗
	battle.setProperty(userID, PropertyType.Mp, value);
	return true;
end
--治疗术
HealOne = Skill:new();
--设置基础属性
HealOne:setIconID(72);
HealOne:setName("治疗术");
HealOne:setDescription("恢复少量HP 消耗2MP");
HealOne:setUsageRange(UsageRange.SingleAlly);
HealOne:setUsageOccasion(UsageOccasion.All);
HealOne:setCost(0);

function HealOne:execute(userID,targetID)
	--消耗mp
	local ret = Skill.consumeMP(userID, 2);

	if ret == true then
		local value = math.random(30, 40);
		HitPointItem.execute(self, userID, targetID, value, "HealOne1");
	end
end
--Sword攻击
SwordAttack = Skill:new();
SwordAttack:setIconID(76);
SwordAttack:setName("强力攻击");
SwordAttack:setDescription("对敌方单体进行强力攻击,消耗2MP");
SwordAttack:setDeleption(false);
SwordAttack:setUsageRange(UsageRange.SingleEnemy);
SwordAttack:setUsageOccasion(UsageOccasion.Fighting);--仅能在战斗状态下使用
SwordAttack:setCost(0);

function SwordAttack:getFightState()
	return FightState.Thrust;
end

function SwordAttack:execute(userID, targetID)
	-- 消耗mp
	local ret = Skill.consumeMP(userID, 2);
	--开始攻击
	if ret then
		--获取角色攻击力
		local  value = battle.getProperty(userID, PropertyType.Attack) + 7;
		--前回调函数 显示武器挥舞动作 TODO
		local beforeCallback = function()
			return battle.showBattleAnimation( userID, "CaneEffect");
		end
		local callbacks = {};
		callbacks["beforeCallback"] = beforeCallback;
		--尝试攻击
		Weapon.execute(self, userID, targetID, value, HurtType.Attack, "HitEffect", true,Callbacks);
	end
end