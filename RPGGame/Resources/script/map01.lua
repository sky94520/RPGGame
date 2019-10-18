require "rpg_core"
--创建一个门
object.create("door1", "Door1_0", 6, 5);

object.setTrigger("door1", TriggerType.Touch, "change2map01_01");
object.setPriority("door1", Priority.Low);


function change2map01_01( ... )
	-- body
	print("change to map01_01");
	base.setGameState(GameState.Script);
	--fadeIn
	screen.fadeIn(1.5);
	--跳转地图
	movement.changeMap("map/Map01_01.tmx", 8, 12);
	screen.fadeOut(1.5);
	base.setGameState(GameState.Normal);
end