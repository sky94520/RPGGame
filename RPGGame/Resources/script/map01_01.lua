require "rpg_core"
--创建一个门
object.create("portal1", "Portal_1", 8, 13);

object.setTrigger("portal1", TriggerType.Touch, "change2map01");
object.setPriority("portal1", Priority.Low);


function change2map01( ... )
	-- body
	print("change to map01");
	base.setGameState(GameState.Script);
	--fadeIn
	screen.fadeIn(1);
	--跳转地图
	movement.changeMap("map/Map01.tmx", 6, 6);
	screen.fadeOut(1);
	base.setGameState(GameState.Normal);
end