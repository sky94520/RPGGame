require "NonPlayerCharacter"
require "rpg_core"

--怪物生成器
Map02_03_EnemySpawner01 = EnemySpawner:new();

function Map02_03_EnemySpawner01:initialize(id)
	--敌人
	local enemies = {
		{troop="Goblin1" ,weight=1,value=2},
		--{troop="StoneMonster1" ,weight=1,value=2}
	};
	EnemySpawner.initialize(self, id, 10, "all", 2, enemies);

end

function Map02_03_EnemySpawner01:execute(playerID)
	-- body
	local enemies = EnemySpawner.execute(self, playerID);

	if enemies == nil then
		return ;
	end
	--进入战斗
	battle.specifyBattleback("battleback1/DemonCastle2.png","battleback2/DemonCastle3.png");
	battle.startBattle(enemies);

	--return true;
end

--死亡骑士，本作的BOSS
Map02_03_BOSS = NonPlayerCharacter:new();

function Map02_03_BOSS:initialize(id)
	-- body
	NonPlayerCharacter.initialize(self, id);
	--设置方向
	character.setDirection(self.id, Direction.Down);
end

function Map02_03_BOSS:execute(playerID)
	-- 进入战斗
	local enemies = {
	{name="DeathKnight", number=1},
	{name="Goblin1", number=1}};
	battle.specifyBattleback("battleback1/DemonCastle2.png","battleback2/DemonCastle3.png");
	battle.startBattle(enemies);
end

--骷髅战士1
Map02_03_Warrior01= NonPlayerCharacter:new();

function Map02_03_Warrior01:initialize(id)
	-- body
	NonPlayerCharacter.initialize(self, id);
	--设置方向
	character.setDirection(self.id, Direction.Down);
end
--骷髅战士2
Map02_03_Warrior02= NonPlayerCharacter:new();

function Map02_03_Warrior02:initialize(id)
	-- body
	NonPlayerCharacter.initialize(self, id);
	--设置方向
	character.setDirection(self.id, Direction.Down);
end
--剧情
Map02_03 = NonPlayerCharacter:new();

function Map02_03:update(dt)
	--是否是第一次进入
	local first = database.getValueOfKey("Map02_03First");
	--第一次 开始剧情
	if first == nil or first == true then
		database.setValueOfKey("Map02_03First", Value.Type.BOOLEAN, false);
		--剧情
		self:firstIn();
	end

end
-- 第一次运行，执行脚本
function Map02_03:firstIn()
	base.setGameState(GameState.Script);
	--获取主角名字
	local playerName = party.getName(0);
	--对话
	message.showText("死亡骑士","你们所说的有着自然之语的天赋的就是这个少年吗？",TextPosition.Bottom,true,"img/faces/People1.png",2,FacePosition.Right);
	
	message.showText("骷髅战士1","报告大人，就是这个男孩",TextPosition.Bottom,true);
	message.showText("骷髅战士2","有了自然之语，我们就能和兽族进行沟通，来让兽族和我们亡灵族联盟，进攻人族领地",TextPosition.Bottom,true);
	message.showText("死亡骑士","嗯，你们办事效率不错。事不宜迟，你们马上准备亡灵转化仪式。我要让他成为我座下的四大战士之一",TextPosition.Bottom,true,"img/faces/People1.png",2,FacePosition.Right);
	message.showText("骷髅战士1","是，大人",TextPosition.Bottom,true);
	--一声巨响
	sound.playEffect("sound/se/Battle3.ogg", 0);
	timer.delay(2.0);
	--旁白
	message.showText("***","只听附近传来一阵巨响", TextPosition.Middle, true);
	--气泡
	character.showBalloonIcon(Map02_03_BOSS.id, "Exclamation");
	character.showBalloonIcon(Map02_03_Warrior01.id, "Exclamation");
	local duration = character.showBalloonIcon(Map02_03_Warrior02.id, "Exclamation");
	timer.delay(duration);
	--对话
	message.showText("死亡骑士","谁？？？！！！",TextPosition.Bottom,true,"img/faces/People1.png",2,FacePosition.Right);
	--主角消失
	character.setVisible(0, false);
	--对话
	message.showText("人族大法师","亡灵族的小崽子们，这个小男孩我就带走了", TextPosition.Bottom, true);
	--传出切换场景音效
	sound.playEffect("sound/se/Move1.ogg", 0);
	timer.delay(0.5);

	message.showText("死亡骑士","大法师，你敢坏我好事！！！",TextPosition.Bottom,true,"img/faces/People1.png",2,FacePosition.Right);
	screen.fadeInScreen(0.8);
	movement.changeMap("map/Map03_01.tmx", 12, 12);
	screen.fadeOutScreen(0.8);
	--主角消失
	character.setVisible(0, true);

	base.setGameState(GameState.Normal);
end