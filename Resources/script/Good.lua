require "rpg_core"

--物品table 其子类包括
--Item 道具
--Equipment 装备
--Skill 技能
--Seed 种子
Good = {};
--新建函数
function Good:new(o)
	-- body
	o = o or {};

	setmetatable(o,self);
	self.__index = self;

	return o;
end
--物品的执行函数
--userId 使用的人的id
--targetId 目标人的id
function Good:execute(userID,targetID)
	-- body
end
--清除函数
function Good:clean()
	-- body
	self = nil;
end
------------------------------用于显示
--设置图标
function Good:setIconID(iconID)
	-- body
	self.iconID = iconID;
end
--设置名称
function Good:setName(name)
	-- body
	self.name = name;
end
--设置描述
function Good:setDescription(descrition)
	-- body
	self.descrition = descrition;
end
--是否是消耗品
function Good:setDeleption(deleption)
	-- body
	self.deleption = deleption;
end
--设置个数
function Good:setNumber(number)
	self.number = number;
end
--设置价钱      不包括技能 价钱为0表示不可出售
function Good:setCost(cost)
	self.cost = cost;
end

function Good:setGoodType(goodType)
	self.goodType = goodType;
end
--设置使用范围
function Good:setUsageRange(usageRange)
	self.usageRange = usageRange;
end
--设置使用场合
function Good:setUsageOccasion(usageOccasion)
	self.usageOccasion = usageOccasion;
end
--获取唯一id
function Good:getUniqueID(bAdding)
	local var = 1;

	if self.uniqueID == nil then
		self.uniqueID = 1;
	elseif bAdding ~= nil and bAdding == true then
		self.uniqueID = self.uniqueID + 1;
		var = self.uniqueID;
	end

	return var;
end

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