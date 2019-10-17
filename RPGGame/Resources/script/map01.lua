require "rpg_core"
--创建几个门
object.create("door1", "Door1_0", 6, 7);

object.setTrigger("door1", TriggerType.Touch, "change2map01_01");
object.setPriority("door1", Priority.Low);


function change2map01_01( ... )
	-- body
	print("change to map01_01");
	base.setGameState(GameState.Script);
	--fadeIn
	screen.fadeIn(1.5);
	screen.fadeOut(1.5);
	base.setGameState(GameState.Normal);
	print("script end");
end