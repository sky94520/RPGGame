require "rpg_core"

--物品table 其子类包括
--Item 道具
--Equipment 装备
--Skill 技能
--Seed 种子
Good = {};
--构造函数
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
------------------------------用于展示------------------------------
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

function Good:getFightState()
	return FightState.Item;
end