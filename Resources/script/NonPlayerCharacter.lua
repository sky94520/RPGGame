require "rpg_core"

NonPlayerCharacter = {};

function NonPlayerCharacter:new(o)
	o = o or {};

	setmetatable(o,self);
	self.__index = self;

	return o;
end
--初始化
function NonPlayerCharacter:initialize(id)
	self.id = id;
end
--playerID 触发的主角id
--返回值 为true则吞并事件
function NonPlayerCharacter:execute(playerID)
	return false;
end

function NonPlayerCharacter:update()
	-- body
end

function NonPlayerCharacter:clean()
	self = nil;
end
--朝向某角色，并保存原来的方向
function NonPlayerCharacter:toward(id)
	-- body
	local dir = character.getDirection(id);
	--获取对立的方向
	local oppositeDir;

	if dir == Direction.Left then 
		oppositeDir = Direction.Right;
	elseif dir == Direction.Right then 
		oppositeDir = Direction.Left;
	elseif dir == Direction.Up then 
		oppositeDir = Direction.Down;
	elseif dir == Direction.Down then 
		oppositeDir = Direction.Up;
	end
	--保留当前朝向,并改变方向
	self.oldDir = character.setDirection(self.id,oppositeDir);
end
--门对象
Door = NonPlayerCharacter:new();
--初始化函数
function Door:initialize(id,chartletName)
	-- body
	NonPlayerCharacter.initialize(self,id);
	--保存贴图
	self.chartletName = chartletName;
end
--执行函数
function Door:execute(playerID,mapName,x,y)
	-- 开门音效
	sound.playEffect("sound/se/Open1.ogg", 0);
	--门 打开动画
	for k,v in pairs(Direction:toArray()) do
		local duration = character.showAnimation(self.id,self.chartletName,v);
		timer.delay(duration);
	end
	sound.playEffect("sound/se/Move1.ogg", 0);
	--淡入场景
	screen.fadeInScreen(0.8);
	--切换场景
	movement.changeMap(mapName,x,y);

	screen.fadeOutScreen(0.8);

	return true;
end
--宝箱
Chest = NonPlayerCharacter:new();
--id 对应的c++对象的id
--chartletName 贴图名
--key 对应的数据库的key
function Chest:initialize(id,chartletName,key)
	-- body
	NonPlayerCharacter.initialize(self,id);
	self.chartletName = chartletName;
	self.key = key;
end

function Chest:execute(playerID)
	-- body
end
--商人，应在这之前填充物品
Merchant = NonPlayerCharacter:new();

function Merchant:execute(playerID,goods)
	-- body
	local exit = false;
	repeat
		local result = message.showChoices({"购买物品","出售物品","退出"});
		--判断选择
		if result == "购买物品" then
			scene.shopProcessing(goods);
		elseif result == "出售物品" then
			scene.shopProcessing();
		elseif result == "退出" then
			exit = true;
		end
	until exit;
end

--敌人生成器
EnemySpawner = NonPlayerCharacter:new();
--初始化函数
--enc_step 多少步出现怪物
--range 范围 有 part(对应的对象大小) all 全地图
--value 怪物的总价值不能超过该value
--enemies 对应的怪物 table
function EnemySpawner:initialize(id,enc_step,range,value,enemies)
	-- body
	self.id = id;
	self.step = 0;
	self.enc_step = enc_step;
	self.range = range;
	self.value = value;
	self.enemies = enemies;
	self.weight = 0;
	--获取总权重
	for k,v in pairs(self.enemies) do
		self.weight = self.weight + v.weight;
	end
end
--对step进行更新，如果到达步数，则返回对应的怪物 table
function EnemySpawner:execute(playerID)
	-- body
	self.step = self.step + 1;
	--不到出怪时间，直接返回
	if self.step < self.enc_step then
		return ;
	end
	--出现怪物
	self.step = self.step - self.enc_step;
	--保存将要出现的怪物列表
	local enemies = {};
	local value = 0;
	local index = 0;

	while value < self.value do
		index = math.random(1,self.weight);
		local sum = 0;		
		--获取落在的区间
		for i,v in ipairs(self.enemies) do
			sum = sum + v.weight;

			if index <= sum then
				local bInserted = false;
				--检测是否已经存在该怪物
				for i,iv in ipairs(enemies) do
					if iv.name == v.troop then
						iv.number = iv.number + 1;
						bInserted = true;
						break;
					end
				end
				if bInserted == false then 
					table.insert(enemies,{name = v.troop , number = 1});
				end
				value = value + v.value;
				break;
			end--end if
		end--end for
	end--end while

	return enemies;
end