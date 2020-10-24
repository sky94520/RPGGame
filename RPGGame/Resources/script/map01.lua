require "rpg_core"
--创建一个门
object.create("door1", "Door1_0", 6, 5);

object.setTrigger("door1", TriggerType.Click, "change2map01_01");
object.setPriority("door1", Priority.Same);

--base.setViewpointCenter(12*48, 6*48, 2000);


function change2map01_01( ... )
	-- body
	print("change to map01_01");
	base.setGameState(GameState.Script);
	--显示开门动画
	for k, dir in pairs(Direction:toArray()) do
		local duration = object.showWalkingAnimation("door1", dir);
		timer.delay(duration);
	end
	--fadeIn
	screen.fadeIn(1.0);
	--跳转地图
	movement.changeMap("map/Map01_01.tmx", 8, 12);
	screen.fadeOut(1.0);
	base.setGameState(GameState.Normal);
end

--创建一个怪物
object.create("monster", "Skeleton01", 8, 8);
object.setTrigger("monster", TriggerType.Click, "generate_monster");
object.setPriority("monster", Priority.Same);

function generate_monster()
	local enemies = {}
	table.insert(enemies, {name="Goblin1", number=2})

	--设置背景
	battle.specifyBattleback("battleback1/Grassland.png", "battleback2/Grassland.png");
	battle.startBattle(enemies);
end