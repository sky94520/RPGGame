require "rpg_core"
--创建一个门
object.create("door1", "Door1_0", 6, 5);

object.setTrigger("door1", TriggerType.Click, "change2map01_01");
object.setPriority("door1", Priority.Same);


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