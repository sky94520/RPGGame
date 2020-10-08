require "good"
require "rpg_core"

Equipment = Good:new();
--����Ϊװ������
Equipment:setGoodType(GoodType.Equipment);

-- ִ�к���
function Equipment:execute(userID,targetID)
end

--װ��
function Equipment:equip(ownerID)
	self.ownerID = ownerID;
end

--ж��װ��
function Equipment:unequip()
	self.ownerID = -1;
end
--���ø�������
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
----------------------------------------------�·�------------------------------------
--����
SimpleCloth = Equipment:new();
SimpleCloth:setIconID(155);
SimpleCloth:setName("����");
SimpleCloth:setDescription("�򵥹�����·�,�����ϸ�ŵĻ������ʵ�ǰ�����˵�ζ��");
SimpleCloth:setDeleption(false);
SimpleCloth:setEquipmentType(EquipmentType.Cloth);
SimpleCloth:setDefense(4);
SimpleCloth:setCost(10);
--Ƥ��
LeatherArmor = Equipment:new();
LeatherArmor:setIconID(156);
LeatherArmor:setName("Ƥ��");
LeatherArmor:setDescription("δ֪�����Ƥ����Ƥ�ף����ź�������");
LeatherArmor:setDeleption(false);
LeatherArmor:setEquipmentType(EquipmentType.Cloth);
LeatherArmor:setDefense(12);
LeatherArmor:setCost(180);
--������
ChainArmor = Equipment:new();
ChainArmor:setIconID(157);
ChainArmor:setName("������");
ChainArmor:setDescription("���������ף�ֻ��ǿ׳���˲�װ������");
ChainArmor:setDeleption(false);
ChainArmor:setEquipmentType(EquipmentType.Cloth);
ChainArmor:setDefense(18);
ChainArmor:setCost(350);

----------------------------------------------����------------------------------------
OffHand = Equipment:new();
function OffHand:setOffHandType(offHandType)
	self.offHandType = offHandType;
end
function OffHand:getBulletName()
	-- body
	return "arrow01";
end
-----------------------------����----------------------------
Arrow = OffHand:new();
Arrow:setDeleption(true);
Arrow:setEquipmentType(EquipmentType.OffHand);
Arrow:setOffHandType(OffHandType.Arrow);
--ľ��
WoodenArrow = Arrow:new();
WoodenArrow:setIconID(107);
WoodenArrow:setName("ľ��");
WoodenArrow:setDescription("ϸ��֦����ľ��");
WoodenArrow:setAttack(1);
WoodenArrow:setCost(2);
function WoodenArrow:getBulletName()
	-- body
	return "arrow01";
end
-----------------------------����----------------------------
Shield = OffHand:new();
Shield:setDeleption(false);
Shield:setEquipmentType(EquipmentType.OffHand);
Shield:setOffHandType(OffHandType.Shield);
--Ƥ��
LeatherShield = Shield:new();
LeatherShield:setIconID(158);
LeatherShield:setName("Ƥ��");
LeatherShield:setDescription("δ֪�����Ƥ����Ƥ��");
LeatherShield:setDefense(4);
LeatherShield:setCost(90);
--�۶�
ScaleShield = Shield:new();
ScaleShield:setIconID(159);
ScaleShield:setName("�۶�");
ScaleShield:setDescription("����΢���ӷ�����");
ScaleShield:setDefense(7);
ScaleShield:setCost(180);
--���� ��ʷ��������
Ironshield = Shield:new();
--���û�������
Ironshield:setIconID(129);
Ironshield:setName("����");
Ironshield:setDescription("ǿ׳�������õĶ�");
Ironshield:setDeleption(false);
Ironshield:setUsageRange(UsageRange.SingleEnemy);
Ironshield:setUsageOccasion(UsageOccasion.Fighting);
Ironshield:setDefense(2);
Ironshield:setMagicDefense(2);
Ironshield:setLuck(-2);
Ironshield:setCost(40);