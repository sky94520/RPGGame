require "rpg_core"
--����һ����
object.create("door1", "Door1_0", 6, 5);

object.setTrigger("door1", TriggerType.Click, "change2map01_01");
object.setPriority("door1", Priority.Same);


function change2map01_01( ... )
	-- body
	print("change to map01_01");
	base.setGameState(GameState.Script);
	--fadeIn
	screen.fadeIn(1.5);
	--��ת��ͼ
	movement.changeMap("map/Map01_01.tmx", 8, 12);
	screen.fadeOut(1.5);
	base.setGameState(GameState.Normal);
end