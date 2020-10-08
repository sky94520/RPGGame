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

--���ۿ� ֻ�������֪���̵�Ĺ����
SellCard = Good:new();
--���û�������
SellCard:setIconID(188);
SellCard:setName("���ۿ�");
SellCard:setDescription("�̵������Ʒ50%�ļ�Ǯ �չ���Ʒ,������");
SellCard:setDeleption(false);
SellCard:setUsageRange(UsageRange.Self);
SellCard:setUsageOccasion(UsageOccasion.Normal);
SellCard:setCost(0);
--���
Sulphur = Good:new();
Sulphur:setName("���");
Sulphur:setDescription("��ҪС�ı��棬��;�ǳ��㷺");
Sulphur:setDeleption(false);
Sulphur:setUsageRange(UsageRange.SingleAlly);
Sulphur:setUsageOccasion(UsageOccasion.Fighting);
Sulphur:setCost(10);
--��
Nitrate = Good:new();
Nitrate:setName("��ʯ");
Nitrate:setDescription("������ҩ��ԭ��֮һ");
Nitrate:setDeleption(false);
Nitrate:setUsageRange(UsageRange.SingleAlly);
Nitrate:setUsageOccasion(UsageOccasion.Fighting);
Nitrate:setCost(10);
--ľ̿
Charcoal = Good:new();
Charcoal:setName("ľ̿");
Charcoal:setDescription("ľ�ľ�������ȫȼ�ջ�����������Ƚ�����ɣ�������������Ʒ");
Charcoal:setDeleption(false);
Charcoal:setUsageRange(UsageRange.SingleAlly);
Charcoal:setUsageOccasion(UsageOccasion.Fighting);
Charcoal:setCost(10);
--ըҩ ������Ʒ�����ɳ���
TNT = Good:new();
--���û�������
TNT:setIconID(188);
TNT:setName("ըҩ");
TNT:setDescription("�����޴��ըҩ����ը��ɽʯ");
TNT:setDeleption(true);
TNT:setUsageRange(UsageRange.SingleAlly);
TNT:setUsageOccasion(UsageOccasion.Fighting);
TNT:setCost(0);

function TNT:execute(userID, targetID)
	-- body
end