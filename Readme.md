# RPG游戏开发

>旨在开发2D回合制游戏。
## 数据
>### 静态数据
>1. StaticData 
>主要用于处理"data/static_data.plist"，并向游戏中提供键值对
>2. CharacterData
>处理角色相关的数据，比如角色的行走动画、立绘、战斗动画以及升级属性值。
>### 动态数据
>1. UserRecord
> 用户记录，用于读取/写入XML文件。比如金币数量、当前所处的地图和位置以及玩家数据等。
>2. DynamicData
>内含有UserRecord对象，负责管理存档对象。
## 显示层
>1. EffectLayer
>特效层 显示各种特效。
>2. MapLayer
>地图层，整个游戏的载体，负责显示地图，以及各种事件。
>3. PlayerLayer
>主角层管理 在效果上是PlayerManager的功能
>4. ScriptLayer
> 负责管理脚本对象
>5. SpritePool
>精灵池，负责创建精灵和回收精灵对象
>6. Text
>可用于显示对话
## 脚本
>1. BaseScript
>注册基本的脚本函数
>
>>1. base.getGameState()
>>- 获取当前的游戏状态
>>- @return 返回游戏状态
>>
>>2. base.setGameState(gameState)
>>- 设置当前的游戏状态
>>- @param gameState 游戏状态GameState{Normal, Fighting, Script} 设置在rpg_core.lua中
>>- @return
>>
>>3. base.setViewpointCenter(x, y, duration)
>>- 改变中心点
>>- @param x x坐标
>>- @param y y坐标
>>- @param duration 持续时间 以毫秒为单位
>>- @return
>>
>>4. base.setViewpointFollow()
>>- 设置视角跟随TODO
>>- @return
>>
>>5. movement.changeMap(mapName, tileX, tileY)
>>- 切换地图，(tileX, tileY)为中心目标
>>- @param mapName 地图名称
>>- @param tileX 横轴图块坐标
>>- @param tileY 纵轴图块坐标
>>- @return
>>
>>6. screen.fadeInScreen(duration)
>>- 淡入场景
>>- @param duration 持续时间
>>- @return
>>
>>7. screen.fadeOutScreen(duration)
>>- 淡出场景
>>- @param duration 持续时间
>>- @return
>>
>>8. timer.delay(duration)
>>- 脚本等待一段时间
>>- @param duration 持续时间
>>- @return
>
>2. LuaStack
>对lua的类化
>
>3. ObjectScript
>注册跟脚本对象相关的函数。
>
>>1. Object.create_object(name, chartlet, tileX, tileY)
>>创建脚本对象
>>- @param name 脚本对象的名称 唯一，可以通过这个name来操作该脚本对象
>>- @param chartlet 动画名称
>>- @param tileX 所在的tileX位置
>>- @param tileY 所在的tileY位置
>>- @return 是否创建成功
>>
>>2. Object.delete_object(name)
>>- @param name 要删除的脚本对象名称
>>
>>3. Object.set_trigger(name, type)
>>设置脚本对象的触发方式，以及触发后的回调函数
>>- @param name 脚本对象名称
>>- @param type 触发类型 int=>TriggerType
>>- @param funcName lua函数名
>>
>>4. Object.set_priority(name, type)
>>设置优先级
>>- @param name 脚本对象名称
>>- @param type 优先级 int
## 实体
>1. Entity 实体类，是角色类的基类
>2. Character
>3. LuaObject
>
>4. Controller
>5. FollowController
>6. AStarController
>7. ControllerListener
