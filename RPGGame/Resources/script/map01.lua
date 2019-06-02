require "rpg_core"
--创建几个门
object.create("door1", "Door1_0", 6, 7);

object.setTrigger("door1", TriggerType.Click, "change2map01_01");
object.setPriority("door1", Priority.Same);


function change2map01_01( ... )
	-- body
	print("change to map01_01");
end