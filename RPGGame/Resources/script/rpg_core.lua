--包含着一些枚举体和一些函数
--游戏状态枚举体
GameState = {};
GameState.Normal = 0;
GameState.Fighting = 1;
GameState.Script = 2;

--脚本触发方式
TriggerType = {};
TriggerType.Click = 1;
TriggerType.Touch = 2;
TriggerType.All = 3;

--优先级
Priority = {};
Priority.Same = 0;
Priority.Low = -1;
Priority.High = 1;
--方向
Direction = {};
Direction.Down = 0;
Direction.Left = 1;
Direction.Right = 2;
Direction.Up = 3;
--获取对应数组
function Direction:toArray()
	-- body
	return {self.Down,self.Left,self.Right,self.Up};
end

--物品类型
GoodType = {};
GoodType.Item = 0;
GoodType.Equipment = 1;
GoodType.Skill = 2;
GoodType.Seed = 3;
--装备类型
EquipmentType = {};
EquipmentType.Weapon = 0;
EquipmentType.OffHand = 1;
EquipmentType.Cloth = 2;
EquipmentType.Ornament = 3;
--副手类型
OffHandType = {};
OffHandType.Shield = 0;
OffHandType.Arrow = 1;
--武器类型
WeaponType = {};
WeaponType.BareHands = 0;
WeaponType.Dagger = 1;
WeaponType.Sword = 2;
WeaponType.Flail = 3;
WeaponType.Axe = 4;
WeaponType.Whip = 5;
WeaponType.Cane = 6;
WeaponType.Bow = 7;
WeaponType.Crossbow = 8;
WeaponType.Claw = 9;
WeaponType.Glove = 10;
WeaponType.Spear = 11;
--使用范围
UsageRange = {};
UsageRange.Self = 0;
UsageRange.SingleAlly = 1;
UsageRange.TotalAlly = 2;
UsageRange.SingleEnemy = 3;
UsageRange.TotalEnemy = 4;
--使用场合
UsageOccasion = {};
UsageOccasion.Normal = 0;
UsageOccasion.Fighting = 1;
UsageOccasion.All = 2;
--战斗状态
FightState = {};
FightState.Idle = 0;
FightState.Thrust = 1;
FightState.Escape = 2;
FightState.Wait = 3;
FightState.Swing = 4;
FightState.Victory = 5;
FightState.Chant = 6;
FightState.Missile = 7;
FightState.Dying = 8;
FightState.Guard = 9;
FightState.Skill = 10;
FightState.Abnormal = 11;
FightState.Damage = 12;
FightState.Spell = 13;
FightState.Sleep = 14;
FightState.Evade = 15;
FightState.Item = 16;
FightState.Dead = 17;
--武器类型
WeaponType = {};
WeaponType.BareHands = 0;
WeaponType.Dagger = 1;
WeaponType.Sword = 2;
WeaponType.Flail = 3;
WeaponType.Axe = 4;
WeaponType.Whip = 5;
WeaponType.Cane = 6;
WeaponType.Bow = 7;
WeaponType.Crossbow = 8;
WeaponType.Claw = 9;
WeaponType.Glove = 10;
WeaponType.Spear = 11;
--特效位置类型
AnimPosType = {};
AnimPosType.Head = 0;
AnimPosType.Body = 1;
AnimPosType.Foot = 2;
AnimPosType.Scene = 3;
AnimPosType.Weapon = 4;
--受伤类型
HurtType = {};
HurtType.None = 0;
HurtType.Attack = 1;
HurtType.MagicAttack = 2;
HurtType.CriticalAttack = 3;
--属性值
AttributeType = {};
AttributeType.None = 0;
AttributeType.Fire = 1;
--属性类型
PropertyType = {};
PropertyType.Hp = 0;
PropertyType.Mp = 1;
PropertyType.Attack = 2;
PropertyType.Defense = 3;
PropertyType.MagicAttack = 4;
PropertyType.MagicDefense = 5;
PropertyType.Agility = 6;
PropertyType.Luck = 7;
--出手类型
TurnType = {};
TurnType.Player = 0;
TurnType.Friend = 1;
TurnType.Enemy = 2;
--搜索类型
SearchType = {};
SearchType.HpMax = 0;
SearchType.HpMin = 1;
SearchType.Random = 2;
--Value类型
Value={};
Value.Type = {};
Value.Type.BYTE = 1;
Value.Type.INTEGER = 2;
Value.Type.FLOAT = 3;
Value.Type.DOUBLE = 4;
Value.Type.BOOLEAN = 5;
Value.Type.STRING = 6;
