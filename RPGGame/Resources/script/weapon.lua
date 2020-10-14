require "equipment"
require "rpg_core"

Weapon = Equipment:new();
--设置类型为武器
Weapon:setEquipmentType(EquipmentType.Weapon);
--设置武器具体类型
function Weapon:setWeaponType(weaponType)
	self.weaponType = weaponType;
end
--获取该武器的出手动作
function Weapon:getFightState()
	return FightState.Swing;
end
--是否命中
--userID 使用者ID
--targetID 目标ID
function Weapon.isHit(userID, targetID)
	--获取用户的命中率
	local hitRate = battle.getHitRate(userID);
	local evasionRate = battle.getEvasionRate(targetID);

	local result = (hitRate + 1) / (evasionRate + 1) * 0.5;

	return math.random() <= result;
end
--是否暴击返回 是否暴击 和 暴击倍数
--userId 用户id
function Weapon.isCritical(userID)
	local criticalRate = battle.getCriticalRate(userID);
	--TODO:设定倍数为1.5倍
	return  math.random() <= criticalRate , 1.5;
end
--攻击函数
--userID 使用者ID
--targetID 目标ID
--value 值
--hurtType 伤害类型
--atkEffect 攻击目标特效
--isMelee 是否是近战 默认为true
--beforeCallback 攻击前回调函数 可缺省 返回时间
--endCallback 攻击成功后的回调函数 可缺省
--beforeCallback beforeAtkCallback endAtkCallback endCallback
--以上几个回调函数是在一个callbacks中存在，可缺省

function Weapon:execute(userID, targetID, value, hurtType, atkEffect, isMelee, callbacks)
	--获取几个回调函数
	local beforeCallback = callbacks and callbacks["beforeCallback"];
	local endCallback = callbacks and callbacks["endCallback"];

	local beforeAtkCallback = callbacks and callbacks["beforeAtkCallback"];
	local endAtkCallback = callbacks and callbacks["endAtkCallback"];

	-- 出手提示 TODO
	if isMelee == nil then
		isMelee = true;
	end

	local duration = 1.0;
	message.showTip("出手攻击", TextPosition.Top, duration);

	--如果是近战 则移动
	if isMelee then
		duration = battle.moveToTarget(userID, targetID);
	else
		duration = 0.2;
	end
	timer.delay(duration);
	--攻击前函数回调
	if beforeCallback ~= nil then
		duration = beforeCallback();
	end
	--武器挥舞
	duration = battle.changeFightState(userID, self:getFightState());
	--等待一会
	timer.delay(duration / 3 * 2);
	--有需要，则显示攻击动画
	if atkEffect ~= nil then
		battle.showBattleAnimation(targetID, atkEffect);
	end
	-------------------------------尝试攻击-------------------------------
	--获取武器的属性 默认无属性
	local attributeType = self.attributeType or AttributeType.None;

	local ret = true;
	--判断是否命中
	if hurtType == HurtType.Attack then
		ret = Weapon.isHit(userID, targetID);
	end
	--未能命中，伤害变为0
	if ret == false and value > 0 then
		value = 0;
	elseif ret then --判断是否发生暴击
		local success, scale = Weapon.isCritical(userID);
		--暴击
		if success then
			value = value * scale;
			hurtType = HurtType.CriticalAttack;
		end --设置攻击倍率 TODO
	end
	--攻击生效前回调函数
	if beforeAtkCallback ~= nil then
		duration = beforeAtkCallback();
		timer.delay(duration);
	end
	--受伤
	battle.hurt(targetID, value, attributeType, hurtType);
	--攻击生效后回调函数
	if endAtkCallback ~= nil then
		duration = endAtkCallback();
		timer.delay(duration);
	end
	timer.delay(duration / 3);
	--攻击完成后回调函数
	if afterCallback ~= nil then
		afterCallback();
	end
	--近战则返回
	if isMelee then
		duration = battle.backTo(userID);
		timer.delay(duration);
	end
	--回合结束
	battle.roundOver();
end
------------------------------------空手----------------------------------------------------
BareHands = Weapon:new();
BareHands:setWeaponType(WeaponType.BareHands);
function BareHands:getFightState()
	return FightState.Thrust;
end

function BareHands:execute(userID, targetID)
	--获取角色攻击力
	local  value = battle.getProperty(userID, PropertyType.Attack);

	Weapon.execute(self, userID, targetID, value, HurtType.Attack, "HitPhysical", true);
end
------------------------------------杖----------------------------------------------------
Cane = Weapon:new();
Cane:setWeaponType(WeaponType.Cane);
function Cane:getFightState()
	return FightState.Swing;
end

function Cane:execute(userID,targetID)
	--获取角色攻击力
	local  value = battle.getProperty(userID, PropertyType.Attack);
	--前回调函数 显示武器挥舞动作
	local beforeCallback = function()
		return battle.showBattleAnimation(userID, "CaneEffect");
	end

	local callbacks = {};
	callbacks["beforeCallback"] = beforeCallback;

	Weapon.execute(self, userID, targetID, value, HurtType.Attack, "HitPhysical", true, callbacks);
end
--木棒
WoodenStick = Cane:new();
WoodenStick:setIconID(110);
WoodenStick:setName("木棒");
WoodenStick:setDescription("稍微有点攻击力，还是比赤手空拳要好点");
WoodenStick:setDeleption(false);
WoodenStick:setUsageRange(UsageRange.SingleEnemy);
WoodenStick:setUsageOccasion(UsageOccasion.Fighting);
--设置属性
WoodenStick:setAttack(2);
WoodenStick:setCost(10);
--棍棒
Club = Cane:new();
Club:setIconID(124);
Club:setName("棍棒");
Club:setDescription("精心制作的棍棒 拿在手里让人很安心");
Club:setDeleption(false);
Club:setUsageRange(UsageRange.SingleEnemy);
Club:setUsageOccasion(UsageOccasion.Fighting);
--设置属性
Club:setAttack(7);
Club:setCost(30);
------------------------------------剑----------------------------------------------------
Sword = Weapon:new();
Sword:setWeaponType(WeaponType.Sword);
function Sword:getFightState()
	return FightState.Swing;
end
function Sword:execute(userID,targetID)
	local value = battle.getProperty(userID,PropertyType.Attack);
	--武器动画
	local beforeCallback = function()
		return battle.showBattleAnimation(userID, "SwordEffect");
	end
	local callbacks = {};
	callbacks["beforeCallback"] = beforeCallback;

	Weapon.execute(self, userID, targetID, value, HurtType.Attack, "SlashPhysical", true,callbacks);
end
--铜剑
CopperSword = Sword:new();
CopperSword:setIconID(97);
CopperSword:setName("铜剑");
CopperSword:setDescription("稍微有些锋利的剑,没什么特殊效果，攻击力稍微增加");
CopperSword:setDeleption(false);
CopperSword:setUsageRange(UsageRange.SingleEnemy);
CopperSword:setUsageOccasion(UsageOccasion.Fighting);
CopperSword:setAttack(12);
CopperSword:setCost(100);
------------------------------------匕首----------------------------------------------------
Dagger = Weapon:new();
Dagger:setWeaponType(WeaponType.Dagger);
function Dagger:getFightState()
	return FightState.Swing;
end
function Dagger:execute(userID, targetID)
	local value = battle.getProperty(userID,PropertyType.Attack);
	--武器动画
	local beforeCallback = function()
		return battle.showBattleAnimation(userID, "DaggerEffect");
	end
	local callbacks = {};
	callbacks["beforeCallback"] = beforeCallback;

	Weapon.execute(self, userID, targetID, value, HurtType.Attack, "SlashPhysical", true,callbacks);
end
--神圣小刀
HolyKnife = Dagger:new();
HolyKnife:setIconID(96);
HolyKnife:setName("神圣小刀");
HolyKnife:setDescription("制作精良的小刀 小巧而又锋利");
HolyKnife:setDeleption(false);
HolyKnife:setUsageRange(UsageRange.SingleEnemy);
HolyKnife:setUsageOccasion(UsageOccasion.Fighting);
HolyKnife:setAttack(14);
HolyKnife:setCost(200);
------------------------------------拳套类武器----------------------------------------------------
------------------------------------弓箭类武器----------------------------------------------------
Bow = Weapon:new();
function Bow:getFightState()
	return FightState.Missile;
end
function Bow:execute(userID, targetID)
	--获取副手武器
	local offHandStr = party.getEquipment(userID, EquipmentType.OffHand);
	local good = _G[offHandStr];
	local extraValue = 0;
	--使用默认的箭支
	local bulletName = Arrow:getBulletName();
	--副手是箭头 消耗一个
	if good ~= nil and good.offHandType == OffHandType.Arrow then
		bulletName = good:getBulletName();
		party.subEquipment(userID, offHandStr, 1);
		--增加额外的属性值仅限于攻击力
		extraValue = good.attack * 0.3;

		if extraValue < 1 then
			extraValue = 1;
		end
	end

	local value = battle.getProperty(userID,PropertyType.Attack) + extraValue;
	--武器动画
	local beforeCallback = function()
		return battle.showBattleAnimation(userID, "BowEffect");
	end
	--攻击前弓箭效果
	local beforeAtkCallback = function()
		return battle.showBullet(userID, targetID, bulletName);
	end

	local callbacks = {};
	callbacks["beforeCallback"] = beforeCallback;
	callbacks["beforeAtkCallback"] = beforeAtkCallback;

	Weapon.execute(self, userID, targetID, value, HurtType.Attack, nil, false,callbacks);
end
--木弓
WoodenBow = Bow:new();
WoodenBow:setIconID(102);
WoodenBow:setName("木弓");
WoodenBow:setDescription("木头制作的弓，仅仅能当作玩具");
WoodenBow:setDeleption(false);
WoodenBow:setUsageRange(UsageRange.SingleEnemy);
WoodenBow:setUsageOccasion(UsageOccasion.Fighting);
WoodenBow:setAttack(3);
WoodenBow:setCost(85);

function WoodenBow:execute(userID, targetID)
	Bow.execute(self, userID, targetID);
end