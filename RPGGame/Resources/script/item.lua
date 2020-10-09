require "rpg_core"
require "good"

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

-----------------------------------------ҩ��-------------------------------
Item = Good:new();
Item:setGoodType(GoodType.Item);

--Ѫƿ
HitPointItem = Item:new();
--����
--self ��Ҫ��name���� ����Ҫ������Ʒ����
--userID ʹ���ߵ�id
--targetID Ŀ���id
--value �ָ�����ֵ
--anim ��ʾ�Ķ���
function HitPointItem:execute(userID, targetID, value, anim, afterCallback)
	-- ����1��
	self.number = self.number - 1;
	--������Ч
	sound.playEffect("sound/se/Recovery.ogg", 0);
	--��ȡ��Ϸ״̬
	local gameState = base.getGameState();
	--����/�ű�״̬�� ֱ������
	if gameState == GameState.Normal or gameState == GameState.Script then
		battle.heal(targetID, value);
	else--ս��״̬�£�����һ�غϣ�������
		local duration = battle.showBattleAnimation(targetID, anim);
		--��ʾ�ı�
		message.showTip("����", TextPosition.Top, duration);
		--�ȴ�
		timer.delay(duration / 2);
		--����
		battle.heal(targetID, value);
		--���ý�������
		if afterCallback ~= nil then
			afterCallback();
		end
		timer.delay(duration / 2);
		--�غϽ���
		battle.roundOver();
	end
end
-----------------------------------------ҩ��-------------------------------
Herbs = HitPointItem:new();
Herbs:setIconID(183);
Herbs:setName("ҩ��");
Herbs:setDescription("�ָ�����HP");
Herbs:setDeleption(true);
Herbs:setUsageRange(UsageRange.SingleAlly);
Herbs:setUsageOccasion(UsageOccasion.All);
Herbs:setCost(8);

function Herbs:execute(userID, targetID)
	local value = math.random(30,40);

	HitPointItem.execute(self, userID, targetID, value, "HealOne1");
end
