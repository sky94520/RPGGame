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
>2. LuaStack
>对lua的类化
>3. ObjectScript
>注册跟脚本对象相关的函数。
## 实体
>1. Entity 实体类，是角色类的基类
>2. Character
>3. LuaObject

>4. Controller
>5. FollowController
>6. AStarController
>7. ControllerListener
