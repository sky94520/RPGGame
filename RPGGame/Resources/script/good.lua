require "rpg_core"

--��Ʒtable ���������
--Item ����
--Equipment װ��
--Skill ����
--Seed ����
Good = {};
--���캯��
function Good:new(o)
	-- body
	o = o or {};

	setmetatable(o,self);
	self.__index = self;

	return o;
end
--��Ʒ��ִ�к���
--userId ʹ�õ��˵�id
--targetId Ŀ���˵�id
function Good:execute(userID,targetID)
	-- body
end
--�������
function Good:clean()
	-- body
	self = nil;
end
------------------------------����չʾ------------------------------
--����ͼ��
function Good:setIconID(iconID)
	-- body
	self.iconID = iconID;
end
--��������
function Good:setName(name)
	-- body
	self.name = name;
end
--��������
function Good:setDescription(descrition)
	-- body
	self.descrition = descrition;
end
--�Ƿ�������Ʒ
function Good:setDeleption(deleption)
	-- body
	self.deleption = deleption;
end
--���ø���
function Good:setNumber(number)
	self.number = number;
end
--���ü�Ǯ      ���������� ��ǮΪ0��ʾ���ɳ���
function Good:setCost(cost)
	self.cost = cost;
end

function Good:setGoodType(goodType)
	self.goodType = goodType;
end
--����ʹ�÷�Χ
function Good:setUsageRange(usageRange)
	self.usageRange = usageRange;
end
--����ʹ�ó���
function Good:setUsageOccasion(usageOccasion)
	self.usageOccasion = usageOccasion;
end
--��ȡΨһid
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
