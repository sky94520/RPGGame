require "rpg_core"
--基础动画 注意 检测的帧数需要是从0开始的 和effect.plist对应
HitPhysical = {};
HitPhysical.localZOrder = 1;
HitPhysical.monitorFrames = {1};
HitPhysical.posType = AnimPosType.Body;
--TODO
function HitPhysical:frameEvent(index)
	--播放音效
	if index == 1 then
		sound.playEffect("sound/se/Blow3.ogg", 0);
	end
end

HitEffect = {};
HitEffect.localZOrder = 1;
HitEffect.monitorFrames = {1,2};
HitEffect.posType = AnimPosType.Body;

function HitEffect:frameEvent(index)
	--播放音效
	if index == 1 then
		sound.playEffect("sound/se/Evasion.ogg", 0);
	elseif index == 2 then
		sound.playEffect("sound/se/Damage4.ogg", 0);
	end
end
--治愈
HealOne1 = {};
HealOne1.localZOrder = 1;
HealOne1.monitorFrames = {1};
HealOne1.posType = AnimPosType.Foot;

function HealOne1:frameEvent(index)
	if index == 1 then
		sound.playEffect("sound/se/Heal3.ogg", 0);
		sound.playEffect("sound/se/Saint2.ogg", 0);
	end
end

--爪子特效
ClashPhysical = {};
ClashPhysical.localZOrder = 1;
ClashPhysical.monitorFrames = {1,2};
ClashPhysical.posType = AnimPosType.Body;

function ClashPhysical:frameEvent(index)
	if index == 1 then
		sound.playEffect("sound/se/Attack2.ogg", 0);
	elseif index == 2 then
		sound.playEffect("sound/se/Slash2.ogg", 0);
	end
end

--Cane
CaneEffect = {};
CaneEffect.localZOrder = -1;
CaneEffect.monitorFrames = {2};
CaneEffect.posType = AnimPosType.Weapon;

function CaneEffect:frameEvent(index)
	-- body
end
--弓
BowEffect = {};
BowEffect.localZOrder = -1;
BowEffect.monitorFrames = {1};
BowEffect.posType = AnimPosType.Weapon;

function BowEffect:frameEvent(index)
	-- body
	if index == 1 then
		sound.playEffect("sound/se/Sword5.ogg", 0);
	end
end
--火箭特效
arrow02_hit = {};
arrow02_hit.localZOrder = -1;
arrow02_hit.monitorFrames = {1};
arrow02_hit.posType = AnimPosType.Weapon;

function arrow02_hit:frameEvent(index)
	-- body
	if index == 1 then
		sound.playEffect("sound/se/Fire2.ogg", 0);
	end
end
--弓箭特效 毒
arrow03_hit = {};
arrow03_hit.localZOrder = -1;
arrow03_hit.monitorFrames = {1};
arrow03_hit.posType = AnimPosType.Weapon;

function arrow03_hit:frameEvent(index)
	-- body
	if index == 1 then
		sound.playEffect("sound/se/Poison.ogg", 0);
	end
end