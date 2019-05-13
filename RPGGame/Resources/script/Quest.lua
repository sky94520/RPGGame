Quest = {};

function Quest:new(o)
	o = o or {};

	setmetatable(o,self);
	self.__index = self;

	return o;
end

--初始化函数
function Quest:initialize()
	
end

function Quest:getTitle()
	return "";
end
function Quest:getDescription()
	return "";
end
function Quest:getStep()
	return "";
end
function Quest:getReward()
	return "";
end
--完成任务时调用
function Quest:complete()
	-- body
end
--任务1

Map01_Task01 = Quest:new();

function Map01_Task01:initialize()
	--初始化当前进度
	self.step = database.getValueOfKey("Map01_Task01") or 1;
end
--返回该步骤
function Map01_Task01:getStep()
	-- body
	return self.step;
end

function Map01_Task01:getTitle()
	local title = "";

	if self.step == 1 then
		title = "问候村长";
	elseif self.step == 2 then
		title = "调查响声";
	elseif self.step == 3 then
		title = "回去复命";
	elseif self.step == 4 then
		title = "收集黑火药的原料";
	end

	return title;
end

function Map01_Task01:getDescription()
	local desc = "";

	if self.step == 1 then
		desc = "村长好像有事情找我，赶紧去他家吧，他家就在出门左手边";
	elseif self.step == 2 then
		desc = "昨天夜晚外面轰隆一声巨响,村长让你去外面看看发生了什么";
	elseif self.step == 3 then
		desc = "村子外面的响声应该是大石头造成的,但是这个石头单凭人力的话是无法破坏的,还是先回去向村长说明情况吧";
	elseif self.step == 4 then
		desc = "村子里的没有黑火药，需要去采集原料,恰巧村子右侧的废弃矿洞中有原料 不过要小心矿洞中的怪物";
	end

	return desc;
end

function Map01_Task01:getStep()
	local steps = "";

	if self.step == 1 then
		steps = "-去村长家问候村长";
	elseif self.step == 2 then
		steps = "-调查一下外面轰隆声的来源";
	elseif self.step == 3 then
		steps = "-回去向村长复命";
	elseif self.step == 4 then--TODO
		local num1 = party.getGoodNumber("Sulphur");
		local num2 = party.getGoodNumber("Nitrate");
		local num3 = party.getGoodNumber("Charcoal");

		steps = string.format("-硫磺:%d/1 \n -硝石:%d/2 \n -木头:%d/3", num1, num2, num3);
	end

	return steps;
end

function Map01_Task01:getReward()
	local reward = "";

	if self.step == 1 then
		reward = string.format("\\I[%d]%s*1",_G["WoodenStick"].iconID,_G["WoodenStick"].name);--"木棒*1";
	elseif self.step == 2 then
		reward = "布衣*1";
	elseif self.step == 3 then
		reward = "20G";
	elseif self.step == 4 then
		reward = "炸药*1";
	end

	return reward;
end

function Map01_Task01:complete()
	-- body
	if self.step == 1 then
		party.addGood("WoodStick", 1);
	elseif self.step == 2 then--TODO
		party.addGood("SimpleCloth", 1);
	elseif self.step == 3 then
		party.addGold(20);
	elseif self.step == 4 then
		party.addGood("TNT", 1);
	end
	self.step = self.step + 1;
	--回写到数据库
	database.setValueOfKey("Map01_Task01", Value.Type.INTEGER, self.step);

	return self.step > 5;
end