require "Good"
require "rpg_core"

Equipment = Good:new();
--设置为装备类型
Equipment:setGoodType(GoodType.Equipment);

function Equipment:execute(userID,targetID)
end

function Equipment:equip(ownerID)
	self.ownerID = ownerID;
end

function Equipment:unequip()
	self.ownerID = -1;
end
--设置各种属性
function Equipment:setAttack(attack)
	self.attack = attack;
end
function Equipment:setDefense(defense)
	self.defense = defense;
end

function Equipment:setMagicAttack(magicAttack)
	self.magicAttack = magicAttack;
end

function Equipment:setMagicDefense(magicDefense)
	self.magicDefense = magicDefense;
end
function Equipment:setAgility(agility)
	self.agility = agility;
end
function Equipment:setLuck(luck)
	self.luck = luck;
end
function Equipment:setEquipmentType(equipmentType)
	self.equipmentType = equipmentType;
end
----------------------------------------------衣服------------------------------------
--布衣
SimpleCloth = Equipment:new();
SimpleCloth:setIconID(155);
SimpleCloth:setName("布衣");
SimpleCloth:setDescription("简单构造的衣服,如果仔细闻的话还能问到前任主人的味道");
SimpleCloth:setDeleption(false);
SimpleCloth:setEquipmentType(EquipmentType.Cloth);
SimpleCloth:setDefense(4);
SimpleCloth:setCost(10);
--皮甲
LeatherArmor = Equipment:new();
LeatherArmor:setIconID(156);
LeatherArmor:setName("皮甲");
LeatherArmor:setDescription("未知动物的皮做的皮甲，穿着很是舒适");
LeatherArmor:setDeleption(false);
LeatherArmor:setEquipmentType(EquipmentType.Cloth);
LeatherArmor:setDefense(12);
LeatherArmor:setCost(180);
--锁链甲
ChainArmor = Equipment:new();
ChainArmor:setIconID(157);
ChainArmor:setName("锁链甲");
ChainArmor:setDescription("铁做的链甲，只能强壮的人才装备得上");
ChainArmor:setDeleption(false);
ChainArmor:setEquipmentType(EquipmentType.Cloth);
ChainArmor:setDefense(18);
ChainArmor:setCost(350);

----------------------------------------------副手------------------------------------
OffHand = Equipment:new();
function OffHand:setOffHandType(offHandType)
	self.offHandType = offHandType;
end
function OffHand:getBulletName()
	-- body
	return "arrow01";
end
-----------------------------弓箭----------------------------
Arrow = OffHand:new();
Arrow:setDeleption(true);
Arrow:setEquipmentType(EquipmentType.OffHand);
Arrow:setOffHandType(OffHandType.Arrow);
--木箭
WoodenArrow = Arrow:new();
WoodenArrow:setIconID(107);
WoodenArrow:setName("木箭");
WoodenArrow:setDescription("细树枝削成木箭");
WoodenArrow:setAttack(1);
WoodenArrow:setCost(2);
function WoodenArrow:getBulletName()
	-- body
	return "arrow01";
end
-----------------------------盾牌----------------------------
Shield = OffHand:new();
Shield:setDeleption(false);
Shield:setEquipmentType(EquipmentType.OffHand);
Shield:setOffHandType(OffHandType.Shield);
--皮盾
LeatherShield = Shield:new();
LeatherShield:setIconID(158);
LeatherShield:setName("皮盾");
LeatherShield:setDescription("未知动物的皮做的皮盾");
LeatherShield:setDefense(4);
LeatherShield:setCost(90);
--鳞盾
ScaleShield = Shield:new();
ScaleShield:setIconID(159);
ScaleShield:setName("鳞盾");
ScaleShield:setDescription("能稍微增加防御力");
ScaleShield:setDefense(7);
ScaleShield:setCost(180);
--铁盾 历史遗留问题
Ironshield = Shield:new();
--设置基础属性
Ironshield:setIconID(129);
Ironshield:setName("铁盾");
Ironshield:setDescription("强壮的人所用的盾");
Ironshield:setDeleption(false);
Ironshield:setUsageRange(UsageRange.SingleEnemy);
Ironshield:setUsageOccasion(UsageOccasion.Fighting);
Ironshield:setDefense(2);
Ironshield:setMagicDefense(2);
Ironshield:setLuck(-2);
Ironshield:setCost(40);

