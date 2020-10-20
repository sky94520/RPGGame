# RPG游戏开发

>旨在开发2D回合制游戏。
>本项目依赖于[SDL_Engine](https://github.com/sky94520/SDL_Engine)、lua5.4

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
>MapLayer中的一个TiledMap对象内的collision layer为本项目的主要层，
>大部分精灵都依附于碰撞层。
## 管理器
>1. PlayerManager  
>主角层管理 只是负责管理，其角色对象存在MapLayer的碰撞层中
>主要有两个函数需要碰撞层的参与：
>>1. initializePlayers() 从存档中读取数据并生成玩家对象时，需要把对象添加到对象层中
>>2. changeLayerOfPlayers() 改变玩家所在的层 这个主要在GameScene::changeMap内调用来改变所属层
>
>2. ScriptManager  
> 负责管理脚本对象，其脚本对象存在MapLayer的碰撞层中
> 在addLuaObject中需要碰撞层
>
>5. SpritePool
>精灵池，负责创建精灵和回收精灵对象
>6. Text
>可用于显示对话
## 脚本对象 Good[该对象由c++的Good类和lua的Good类组成]
>c++的Good类提供了一个接口，真正的操作是在lua脚本中  
以下为公共函数
>>1. Good:execute(userId, targetId)
>>- 物品的执行函数
>>- @param userId 使用者的id
>>- @param targetId 目标id
>>2. Good:clean()
>>- 析构函数，c++的Good对象在调用析构函数的时候会调用该函数
>>3. Good:setIconID(iconId) 图标id
>>4. Good:setName(name) 设置物品名称
>>5. Good:setDescription(desc) 设置描述
>>6. Good:setDeleption(ret) 设置是否是消耗品
>>7. Good:setNumber(num) 设置本物品个数
>>8. Good:setCost(cost) 设置价格（不包括技能）
>>9. Good:setGoodType(type) 设置物品类型(Item、Equipment、Skill、Seed)
>>10. Good:setUsageRange(ur) 设置使用范围(Self、SingleAlly、TotalAlly、SingleEnemy、TotalEnemy)
>  
>以下为装备特有函数
>>1. equip
>>2. unequip
>>3. 等等
## 脚本
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
>>1. object.create(name, chartlet, tileX, tileY)
>>创建脚本对象
>>- @param name 脚本对象的名称 唯一，可以通过这个name来操作该脚本对象
>>- @param chartlet 动画名称
>>- @param tileX 所在的tileX位置
>>- @param tileY 所在的tileY位置
>>- @return 是否创建成功
>>
>>2. object.delete(name)
>>- @param name 要删除的脚本对象名称
>>
>>3. object.set_trigger(name, type)
>>设置脚本对象的触发方式，以及触发后的回调函数
>>- @param name 脚本对象名称
>>- @param type 触发类型 int=>TriggerType
>>- @param funcName lua函数名
>>
>>4. object.set_priority(name, type)
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

## 待更新
>1. character.xml => json格式
>XML格式不适用于当前的应用场景，目前计划使用jsoncpp代替SDL_Engine ValueMap的使用（使用python amalgamate.py）
>需要解决的问题有：
>>1. 对数组进行遍历
>>2. key在dict中是否存在
>2. 脚本对象的动画显示（依然采用v1版本）
>~~3. 脚本对象和玩家的遮挡问题~~
>该问题已经解决，不过目前需要考虑碰撞层的问题。碰撞层是存在于MapLayer::TiledMap下的一个TMXLayer，
>目前的所有精灵都是依附于这个层的，EffectLayer、PlayerManager、ScriptManager、GameScene
>4. 玩家触碰到脚本时的暂停问题 在触发脚本时，如果玩家在行走，则先暂停；也可以在脚本中停止运动
>5. 切换场景时，changeMap设置中心存在问题（初步认为，该算法在跳跃时存在问题，主角移动时不存在这个问题）
>6. 确定函数，要么都使用像素点，要么都使用tile pos

背包及装备功能 完成
购买/出售功能
战斗功能
对话功能
struct Properties绑定到lua中
