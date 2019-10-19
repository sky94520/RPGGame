--包含着一些枚举体和一些函数
--游戏状态枚举体
GameState = {};
GameState.Normal = 0;
GameState.Fighting = 1;
GameState.Script = 2;

--脚本触发方式
TriggerType = {};
TriggerType.Click = 1;
TriggerType.Touch = 2;
TriggerType.All = 3;

--优先级
Priority = {};
Priority.Same = 0;
Priority.Low = -1;
Priority.High = 1;
--方向
Direction = {};
Direction.Down = 0;
Direction.Left = 1;
Direction.Right = 2;
Direction.Up = 3;
--获取对应数组
function Direction:toArray()
	-- body
	return {self.Down,self.Left,self.Right,self.Up};
end
