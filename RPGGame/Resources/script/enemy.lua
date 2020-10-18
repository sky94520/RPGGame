require "rpg_core"
require "weapon"
-----------------------------------------------敌人-----------------------------------------------------
Enemy = {};
--敌人类型 1龙 2走兽 3僵尸 4水系  5怪人 6恶魔 7机器 8物质 9虫子 10鸟 11植物 12元素 13史莱姆 14魔王
Enemy.Type = {Dragon = 1,Beast = 2,Zombie = 3,Water = 4,Anxiety = 5,
Crowned = 6,Machine = 7,Creeping = 8,Insect = 9,Bird = 10,Plant = 11,
Element = 12,Slime = 13,Devil = 14};

function Enemy:new(o)
	o = o or {};

	setmetatable(o,self);
	self.__index = self;

	return o;
end
--初始化函数
function Enemy:initialize(id)
	self.id = id;
end
--执行函数
function Enemy:execute(userID,targetID)
	-- body
end
--受伤函数
function Enemy:hurt(afterDamage)
	-- body
end

function Enemy:clean()
	local drop_goods = {};

	if self.drop_goods ~= nil then
		--进行遍历
		for i,v in ipairs(self.drop_goods) do
			local rate = math.random();
			--添加掉落物品
			--print("rate",rate);
			if rate <= v.probability then
				drop_goods[v.name] = 1;
			end
		end
	end
	battle.addReward(drop_goods, self.exp, self.gold);
	-- 清空自己
	self = nil;
end

function Enemy:setName(name)
	self.name = name;
end

function Enemy:setDescription(description)
	self.description = description;
end

function Enemy:setHitPoint(hp)
	--设置最大hp
	if self.hp == nil then
		self.maxHp = hp;
	end
	self.hp = hp;
end

function Enemy:setManaPoint(mp)
	if self.mp == nil then
		self.maxMp = mp;
	end
	self.mp = mp;
end

function Enemy:setGold(gold)
	self.gold = gold;
end

function Enemy:setExp(exp)
	self.exp = exp;
end

function Enemy:setAttack(attack)
	self.attack = attack;
end

function Enemy:setDefense(defense)
	self.defense = defense;
end

function Enemy:setMagicAttack(magicAttack)
	self.magicAttack = magicAttack;
end

function Enemy:setMagicDefense(magiDefense)
	self.magiDefense = magicDefense;
end

function Enemy:setAgility(agility)
	self.agility = agility;
end

function Enemy:setLuck(luck)
	self.luck = luck;
end

function Enemy:setHitRate(hitRate)
	self.hitRate = hitRate;
end

function Enemy:setEvasionRate(evasionRate)
	self.evasionRate = evasionRate;
end

function Enemy:setCriticalRate(criticalRate)
	self.criticalRate = criticalRate;
end

--获取唯一id
function Enemy:getUniqueID(bAdding)
	local var = 1;

	if self.uniqueID == nil then
		self.uniqueID = 1;
	elseif bAdding ~= nil and bAdding == true then
		self.uniqueID = self.uniqueID + 1;
		var = self.uniqueID;
	end
	return var;
end

function Enemy:setUniqueID(uniqueID)
	self.uniqueID = uniqueID;
end

function Enemy:setTurnName(turnName)
	self.turnName = turnName;
end
--设置敌人类型
function Enemy:setType(type)
	self.type = type;
end
--设置挥舞类型
function Enemy:getFightState()
	-- body
	return FightState.Swing;
end
--searchType 搜索类型
--hurtType 攻击类型
--atkEffect 攻击特效
--isMelee 是否是近战 默认为true
--beforeCallback 为beforeCallback 返回值为attackCallback的时间
--endCallback
function Enemy:execute(searchType,hurtType,atkEffect, isMelee,callbacks)
	--获取一个id
	local targetID = battle.searchFighter(TurnType.Player, searchType);
	
	if targetID == nil then
		print("Enemy:execute error:player id = -1");
		return;
	end
	--攻击
	Weapon.execute(self, self.id, targetID, self.attack,hurtType, atkEffect, isMelee, callbacks);
end

--哥布林1
Goblin1 = Enemy:new();
Goblin1:setName("普通哥布林");
Goblin1:setDescription("奇怪的怪物");
Goblin1:setTurnName("Goblin1_turn.png");
Goblin1:setHitRate(0.95);
Goblin1:setEvasionRate(0.04);
Goblin1:setCriticalRate(0);
Goblin1:setType(Enemy.Type.Beast);

--掉落物品
Goblin1.drop_goods = {
		{name = "Herbs",probability = 0.125},
		{name = "WoodenStick",probability = 0.125}
};

function Goblin1:initialize(id)
	--调用父类方法
	Enemy.initialize(self, id);
	--设置基础数据
	self:setHitPoint(math.random(7,9));
	self:setAttack(math.random(8,9));
	self:setDefense(math.random(4,5));
	self:setAgility(3);
	self:setLuck(2);

	self:setGold(math.random(1,2));
	self:setExp(1);
	--绑定动画
	battle.setChartletName(self.id, "Goblin1");
	battle.changeFightState(self.id, FightState.Walk);
end

function Goblin1:hurt(afterDamage)
	--
end

function Goblin1:execute()
	--调用攻击
	Enemy.execute(self, SearchType.Random, HurtType.Attack, "HitPhysical", true);
end

--石头怪1
StoneMonster1 = Enemy:new();
StoneMonster1:setName("怪物");
StoneMonster1:setDescription("巨大的石头怪兽");
StoneMonster1:setTurnName("StoneMonster1_turn.png");
StoneMonster1:setHitRate(0.95);
StoneMonster1:setEvasionRate(0.04);
StoneMonster1:setCriticalRate(0);
StoneMonster1:setType(Enemy.Type.Creeping);

--掉落物品
StoneMonster1.drop_goods = {
		{name = "Herbs",probability = 0.125}};

function StoneMonster1:initialize(id)
	--调用父类方法
	Enemy.initialize(self, id);
	--设置基础数据
	self:setHitPoint(16);
	self:setAttack(19);
	self:setDefense(15);
	self:setAgility(10);

	self:setGold(10);
	self:setExp(9);
	--绑定动画
	battle.setChartletName(self.id, "StoneMonster1");
	battle.changeFightState(self.id, FightState.Walk);
end

function StoneMonster1:execute()
	--调用攻击
	Enemy.execute(self, SearchType.Random, HurtType.Attack, "ClashPhysical", true);
end

--死亡骑士
DeathKnight = Enemy:new();
DeathKnight:setName("死亡骑士");
DeathKnight:setDescription("试图毁灭世界，让世界陷于黑暗的邪恶领头者");
DeathKnight:setTurnName("BowKnight_turn.png");
DeathKnight:setHitRate(1.0);
DeathKnight:setEvasionRate(0.2);
DeathKnight:setCriticalRate(0.1);
DeathKnight:setType(Enemy.Type.Devil);

DeathKnight.drop_goods = {
	{name = "Herbs", probability=0.5}
};

function DeathKnight:initialize(id)
	--调用父类方法
	Enemy.initialize(self,id);
	--设置基础数据
	self:setHitPoint(2700);
	self:setAttack(210);
	self:setDefense(140);
	self:setAgility(63);

	self:setGold(400);
	self:setExp(15000);
	--绑定动画
	battle.setChartletName(self.id, "BowKnight");
	battle.changeFightState(id, FightState.Walk);
	battle.setFlipX(id, true);
end

function DeathKnight:execute()
	--调用攻击
	Enemy.execute(self, SearchType.Random, HurtType.Attack, "HitPhysical", false);
end