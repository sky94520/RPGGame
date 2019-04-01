#include "GameScene.h"
#include "EventLayer.h"
#include "MapLayer.h"
#include "PlayerLayer.h"
#include "ScriptLayer.h"
#include "GoodLayer.h"
#include "DisplayLayer.h"
#include "EffectLayer.h"
#include "MessageLayer.h"
#include "OperationLayer.h"
#include "BattleScene.h"
#include "StaticData.h"
#include "DynamicData.h"
#include "Character.h"
#include "AStar.h"
#include "ScriptFunc.h"
#include "NonPlayerCharacter.h"
#include "Text.h"
#include "Good.h"
//static
GameScene* GameScene::s_pInstance = nullptr;
string const GameScene::CHARACTER_MOVE_TO_TILE = "character move to tile";

GameScene* GameScene::getInstance()
{
	//仅在场景在运行时才会分配新的对象
	if (s_pInstance == nullptr && Director::getInstance()->isRunning())
	{
		s_pInstance = new GameScene();
		s_pInstance->init();
	}

	return s_pInstance;
}

void GameScene::purge()
{
	SDL_SAFE_RELEASE_NULL(s_pInstance);
}

GameScene::GameScene()
	:m_pEventLayer(nullptr)
	,m_pMapLayer(nullptr)
	,m_pPlayerLayer(nullptr)
	,m_pScriptLayer(nullptr)
	,m_pOperationLayer(nullptr)
	,m_pBattleScene(nullptr)
	,m_pGoodLayer(nullptr)
	,m_pDisplayLayer(nullptr)
	,m_pEffectLayer(nullptr)
	,m_pMessageLayer(nullptr)
	,m_pViewpointCharacter(nullptr)
	,m_pLuaState(nullptr)
	,m_gameState(GameState::Normal)
	,m_nCoroutineRet(LUA_OK)
{
}

GameScene::~GameScene()
{
	lua_close(m_pLuaState);
}

bool GameScene::init()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();

	this->preloadResources();
	//初始化脚本
	this->initializeScript();
	//事件层
	m_pEventLayer = EventLayer::create();
	this->addChild(m_pEventLayer);
	//地图层
	m_pMapLayer = MapLayer::create();
	this->addChild(m_pMapLayer);
	//角色层
	m_pPlayerLayer = PlayerLayer::create();
	this->addChild(m_pPlayerLayer);
	//脚本层
	m_pScriptLayer = ScriptLayer::create();
	this->addChild(m_pScriptLayer);
	//操作层
	m_pOperationLayer = OperationLayer::create();
	this->addChild(m_pOperationLayer);
	//战斗层
	m_pBattleScene = BattleScene::create();
	//隐藏按钮
	m_pBattleScene->setVisible(false);
	m_pBattleScene->setVisibilityOfActionBtns(false);
	m_pBattleScene->setVisibilityOfUndoBtn(false);
	this->addChild(m_pBattleScene);
	//物品层
	m_pGoodLayer = GoodLayer::create();
	m_pGoodLayer->setPositionY(-visibleSize.height);
	this->addChild(m_pGoodLayer);
	//任务层
	m_pDisplayLayer = DisplayLayer::create();
	m_pDisplayLayer->setPositionY(-visibleSize.height);
	this->addChild(m_pDisplayLayer);
	//文本层
	m_pMessageLayer = MessageLayer::create();
	this->addChild(m_pMessageLayer);
	//特效层
	m_pEffectLayer = EffectLayer::create();
	this->addChild(m_pEffectLayer);
	//初始化地图和角色
	this->initializeMapAndPlayers();

	//m_pGoodLayer->setOnlyShowingBag(true);

	this->scheduleUpdate();

	return true;
}

void GameScene::preloadResources()
{
	StaticData::getInstance()->loadCharacterFile("data/character.plist");
	//加载图片
	auto frameCache = Director::getInstance()->getSpriteFrameCache();
	auto& framePathArray = STATIC_DATA_ARRAY("sprite_frame_array");
	
	for (auto& value : framePathArray)
	{
		auto filepath = value.asString();
		frameCache->addSpriteFramesWithFile(filepath);
	}
	//加载动画文件
	auto animationCache = AnimationCache::getInstance();
	auto& aniPathArray = STATIC_DATA_ARRAY("animation_array");

	for (auto& value : aniPathArray)
	{
		auto filepath = value.asString();
		animationCache->addAnimationsWithFile(filepath);
	}
	//加载图片
	Director::getInstance()->getTextureCache()->addImage("img/system/IconSet.png");
}

bool GameScene::initializeScript()
{
	m_pLuaState = luaL_newstate();
	luaL_openlibs(m_pLuaState);
	//添加自定义的lua加载器
	this->addLuaLoader(sdl_lua_loader);
	//添加路径
	auto platform = Director::getInstance()->getPlatform();
	if (platform == "Windows" || platform == "Linux")
		this->addLuaSearchPath("Resources/script");
	else
		this->addLuaSearchPath("script");
	//注册c函数给lua
	register_funcs(m_pLuaState);
	//加载必要的脚本文件
	this->executeScriptFile("Item.lua");
	this->executeScriptFile("Weapon.lua");
	this->executeScriptFile("Skill.lua");
	this->executeScriptFile("Enemy.lua");
	this->executeScriptFile("Animation.lua");
	this->executeScriptFile("Quest.lua");
	//对已经接收的任务进行初始化
	//调用任务的初始化函数
	auto& list = DynamicData::getInstance()->getAcceptedTaskList();

	for (auto& value : list)
	{
		auto taskName = value.asString();
		//初始化函数
		this->getLuaGlobal(taskName.c_str());
		this->getLuaField(-1, "initialize");
		this->removeLuaIndex(-2);
		//放入参数
		this->getLuaGlobal(taskName.c_str());
		//执行函数
		this->executeFunction(1, 0);
	}
	return true;
}

bool GameScene::initializeMapAndPlayers()
{
	//设置A星算法
	AStar* pAStar = StaticData::getInstance()->getAStar();
	pAStar->isPassing = SDL_CALLBACK_1(GameScene::isPassing, this);
	pAStar->isPassing4 = SDL_CALLBACK_2(GameScene::isPassing4, this);
	
	auto dynamicData = DynamicData::getInstance();
	//获得存档玩家控制的主角队伍的数据
	auto& valueMap = dynamicData->getTotalValueMapOfCharacter();
	//获取方向
	auto nDir = DynamicData::getInstance()->getValueOfKey("direction")->asInt();
	Direction direction = static_cast<Direction>(nDir);
	Character* last = nullptr;
	//解析数据并生成角色
	for (auto itMap = valueMap.begin();itMap != valueMap.end();itMap++)
	{
		auto chartletName = itMap->first;
		auto& propertyMap = itMap->second.asValueMap();
		//创建角色
		Character* player = Character::create(chartletName, direction);
		player->setDurationPerGrid(0.25f);
		//传递给主角层
		m_pPlayerLayer->addCharacter(player);
		//设置属性
		DynamicData::getInstance()->setPlayerStruct(player, propertyMap);
		//设置跟随
		if (last != nullptr)
			player->follow(last);
		else//设置视角跟随
		{
			this->setViewpointFollow(player);
		}

		last = player;
	}

	auto mapFilePath = dynamicData->getMapFilePath();
	auto tileCooridinate = dynamicData->getTileCoordinateOfPlayer();
	//改变地图
	this->changeMap(mapFilePath, tileCooridinate);
	//添加角色移动结束触发器
	_eventDispatcher->addEventCustomListener(CHARACTER_MOVE_TO_TILE,
		SDL_CALLBACK_1(GameScene::moveToTile, this), this);
	//更新金币显示
	m_pGoodLayer->updateShownOfGold(dynamicData->getGoldNumber());

	return true;
}

void GameScene::moveToTile(EventCustom* eventCustom)
{
	auto character = (Character*) eventCustom->getUserData();
	//获取主角
	auto player = m_pPlayerLayer->getPlayer();
	auto playerID = player->getUniqueID();
	//该角色不是主角，直接返回
	if (playerID != character->getUniqueID())
	{
		character->popStepAndAnimate();
		return;
	}
	bool ret = false;
	//TODO:只有处于正常状态下才会触发脚本对象
	if (m_gameState == GameState::Normal)
	{
		auto rect = Rect(player->getPosition(),Size(1.f,1.f));
		//触发高低优先级的脚本事件
		ret = m_pScriptLayer->checkAndTriggerScriptEvent(rect, playerID, TriggerType::Touch,
			PRIORITY_HIGH | PRIORITY_LOW);
	}
	//在不触发脚本时才进行回调
	if (!ret)
		player->popStepAndAnimate();
	else//主角停止走动
	{
		player->sit();
	}
}

int GameScene::getAreaIndexAt(const Point& tilePos)
{
	//默认为1，表示为区域1
	int index = 1;

	if (this->isPassing(tilePos))
	{
		auto tiledMap = m_pMapLayer->getTiledMap();
		//尝试获取area layer层
		auto areaLayer = static_cast<TMXLayer*>(tiledMap->getLayer("area layer"));

		if (areaLayer != nullptr)
		{
			int gid = areaLayer->getTileGIDAt(tilePos);
			//获取对应属性
			ValueMap* properties = nullptr;
			//获取失败
			if ( tiledMap->getTilePropertiesForGID(gid, &properties))
			{
				//获取图块属性
				ValueMap::iterator it = properties->find("index");

				if (it != properties->end())
				{
					int value = it->second.asInt();

					index = value;
				}
			}//end if
		}//end areaLayer
	}
	else
	{
		index = -1;
	}
	return index;
}

bool GameScene::avoidDeadPath(Point& toTile, const Point& fromTile, int fromTileIndex)
{
	auto tiledMap = m_pMapLayer->getTiledMap();
	auto mapSize = tiledMap->getMapSize();

	vector<Point> deltas;
	deltas.push_back(Point(1.f, 0.f));//右
	deltas.push_back(Point(-1.f, 0.f));//左
	deltas.push_back(Point(0.f, 1.f));//下
	deltas.push_back(Point(0.f, -1.f));//上
	
	vector<Point> tiles;
	Point tilePos;
	//四方向遍历
	for (auto& delta : deltas)
	{
		tilePos = toTile;
		//不能超出地图大小
		while (tilePos.x >= 0 && tilePos.y >= 0
			&& tilePos.x < mapSize.width && tilePos.y < mapSize.height)
		{
			tilePos += delta;
			//找到同区域的图块
			if (this->getAreaIndexAt(tilePos) == fromTileIndex)
			{
				tiles.push_back(tilePos);
				break;
			}
		}
	}
	//未能找到替代图块
	if (tiles.empty())
		return false;
	//找出代价最小的点
	toTile = tiles.front();

	for (int i = 1; i < tiles.size(); i++)
	{
		int hScore1 = AStar::computeHScoreFromCoord(fromTile, toTile);
		int hScore2 = AStar::computeHScoreFromCoord(fromTile, tiles[i]);

		if (hScore1 > hScore2)
			toTile = tiles[i];
	}
	return true;
}

void GameScene::update(float dt)
{
	auto n = lua_gettop(m_pLuaState);
	if (n > 0)
	{
		printf("%d ", n);
	}
	//视角跟随
	if (m_pViewpointCharacter != nullptr 
		&& m_pViewpointCharacter->isMoving())
	{
		this->setViewpointCenter(m_pViewpointCharacter->getPosition());
	}
	//更新等待时间
	if (m_gameState != GameState::Normal)
	{
		m_pScriptLayer->updateWaitTime(dt);
	}
	else
	{
		m_pScriptLayer->update(dt);
	}
	//打字机效果显示
	if (m_pMessageLayer->isShowingText()
		&& !m_pMessageLayer->isWindOver())
	{
		m_pMessageLayer->windRun(dt);
		//保证动画只是显示一次
		if (m_pMessageLayer->isWindOver() 
			&& m_pMessageLayer->isClickable())
		{
			auto pos = m_pMessageLayer->getWaitClickPosition();
			m_pEffectLayer->showWaitClick(pos);
		}
	}
	if (m_gameState == GameState::Fighting)
		m_pBattleScene->update(dt);

	if (m_pGoodLayer->isOpened())
	{
		m_pGoodLayer->update(dt);
	}
}

bool GameScene::isPassing(const Point& tilePos)
{
	auto tiledMap = m_pMapLayer->getTiledMap();
	auto mapSize = tiledMap->getMapSize();
	//不可超出地图
	if (tilePos.x < 0 || tilePos.x > (mapSize.width - 1)
		|| tilePos.y > (mapSize.height - 1) || tilePos.y < 0)
	{
		return false;
	}
	auto layer = m_pMapLayer->getCollisionLayer();
	auto gid = layer->getTileGIDAt(tilePos);

	bool ret = m_pMapLayer->isPassing(gid);
	//再次检查meta层
	if (ret)
	{
		layer = m_pMapLayer->getMetaLayer();
		gid = layer->getTileGIDAt(tilePos);
		ret = m_pMapLayer->isPassing(gid);
	}
	//可通过则检查是否存在NPC
	if (ret)
	{
		auto tileSize = tiledMap->getTileSize();
		//获取矩形
		Rect r;
		r.origin = Point(tileSize.width * (tilePos.x + 0.5f), tileSize.height * (tilePos.y + 0.5f));
		r.size = Size(1.f, 1.f);

		auto npc = m_pScriptLayer->getClickedNPC(r, PRIORITY_SAME);

		ret = ( npc == nullptr ? true : false);
	}

	return ret;
}

bool GameScene::isPassing4(const Point& tilePos, Direction dir)
{
	auto layer = m_pMapLayer->getCollisionLayer();
	auto gid = layer->getTileGIDAt(tilePos);
	bool ret = m_pMapLayer->isPassing(gid, dir);
	//再次检查meta层
	if (ret)
	{
		layer = m_pMapLayer->getMetaLayer();
		gid = layer->getTileGIDAt(tilePos);
		ret = m_pMapLayer->isPassing(gid, dir);
	}

	return ret;
}

void GameScene::changeMap(const string& mapFileName, const Point& tileCoodinate)
{
	//获取主角列表 并清除
	auto &characterList = m_pPlayerLayer->getCharacterList();

	for (auto character : characterList)
	{
		character->retain();

		character->sit();
		character->removeFromParent();
	}
	//删除脚本事件
	m_pScriptLayer->clear();
	//清除地图
	m_pMapLayer->clear();
	//改变当前地图
	m_pMapLayer->init(mapFileName);
	//显示地图名称
	auto text = m_pMapLayer->getMapName();
	m_pMessageLayer->showTip(text, TextPosition::Middle, 1.f);
	//重新设置A星算法的大小
	StaticData::getInstance()->getAStar()->setMapSize(m_pMapLayer->getTiledMap()->getMapSize());
	//获取碰撞层
	auto collisionLayer = m_pMapLayer->getCollisionLayer();
	//设置主角位置
	auto tileSize = m_pMapLayer->getTiledMap()->getTileSize();
	auto pos = Point(tileSize.width * (tileCoodinate.x + 0.5f)
		,tileSize.height * (tileCoodinate.y + 0.5f));
	//添加主角
	for (unsigned int i = 0; i < characterList.size();i++)
	{
		auto character = characterList.at(i);
		//添加主角，并设置localZOrder
		collisionLayer->addChild(character,CHARACTER_LOCAL_Z_ORDER - i);

		character->setPosition(pos);
		character->release();
	}
	//改变当前中心点
	this->setViewpointCenter(pos);
	//获取地图名称
	auto startIndex = mapFileName.find_last_of('/');
	auto endIndex = mapFileName.find_last_of('.');
	auto mapName = mapFileName.substr(startIndex + 1, endIndex - startIndex - 1);
	//获取脚本对象
	auto scriptObjects = m_pMapLayer->getScriptObjectGroup();

	if (scriptObjects != nullptr)
	{
		ValueVector& vec = scriptObjects->getObjects();
		//生成npc
		for (const auto& object : vec)
		{
			const auto& valueMap = object.asValueMap();
			//添加到脚本事件层
			auto npc = m_pScriptLayer->addNPC(valueMap);
			//调用脚本初始化函数
			npc->initialize(mapName);
			//遮挡处理
			int localZOrder = CHARACTER_LOCAL_Z_ORDER;

			if (npc->getPriority() == PRIORITY_LOW) localZOrder -= 1;
			else if (npc->getPriority() == PRIORITY_HIGH) localZOrder += 1;
			//添加到碰撞层
			collisionLayer->addChild(npc,localZOrder);
		}
	}
	//尝试播放bgm
	auto bgm = m_pMapLayer->getBGMFilename();
	if (!bgm.empty())
	{
		SoundManager::getInstance()->playBackgroundMusic(bgm, -1);
	}
}

void GameScene::setViewpointCenter(const Point& position, float duration)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	const int tag = 10;
	//地图跟随点移动
	float x = (float)MAX(position.x, visibleSize.width / 2);
	float y = (float)MAX(position.y, visibleSize.height / 2);
	//获取地图层的地图
	auto tiledMap = m_pMapLayer->getTiledMap();

	auto tileSize = tiledMap->getTileSize();
	auto mapSize = tiledMap->getMapSize();
	auto mapSizePixel = Size(tileSize.width * mapSize.width, tileSize.height * mapSize.height);
	//不让显示区域超过地图的边界
	x = (float)MIN(x, (mapSizePixel.width - visibleSize.width / 2.f));
	y = (float)MIN(y, (mapSizePixel.height - visibleSize.height / 2.f));
	//实际移动的位置
	Point actualPosition = Point(x, y);
	//屏幕中心位置坐标
	Point centerOfView = Point(visibleSize.width / 2, visibleSize.height / 2);

	Point delta = centerOfView - actualPosition;

	FiniteTimeAction* action = nullptr;

	if (duration < FLT_EPSILON)
	{
		action = Place::create(delta);
	}
	else
	{
		action = MoveTo::create(duration, delta);
	}

	action->setTag(tag);

	if (tiledMap->getActionByTag(tag) != nullptr)
	{
		tiledMap->stopActionByTag(tag);
	}
	tiledMap->runAction(action);
}

void GameScene::setViewpointFollow(Character* character)
{
	m_pViewpointCharacter = character;
}

GameState GameScene::getGameState() const
{
	return m_gameState;
}

void GameScene::setGameState(GameState state)
{
	if (m_gameState == state)
		return;

	m_gameState = state;
	//操作层可点击
	bool enable = m_gameState == GameState::Normal ? true : false;

	m_pOperationLayer->setTouchEnabled(enable);
	//从脚本中结束，尝试恢复行走
	if (m_gameState == GameState::Normal)
	{
		m_pPlayerLayer->getPlayer()->popStepAndAnimate();
	}
}

Good* GameScene::addGood(const string& goodName, int number)
{
	auto good = DynamicData::getInstance()->addGood(goodName,number);
	//更新背包层
	m_pGoodLayer->updateShownOfGoods(8);

	return good;
}

bool GameScene::subGood(const string& goodName, int number)
{
	bool ret = DynamicData::getInstance()->subGood(goodName, number);
	m_pGoodLayer->updateShownOfGoods(8);

	return ret;
}

bool GameScene::subEquipment(const string& playerName, const string& goodName, int number)
{
	bool ret = DynamicData::getInstance()->subEquipment(playerName, goodName, number);
	m_pGoodLayer->updateShownOfGoods(8);

	return ret;
}

void GameScene::addGold(int num)
{
	auto dynamicData = DynamicData::getInstance();

	int number = dynamicData->getGoldNumber();
	int afterGold = num + number;
	//最大金币为99999
	if (afterGold > 99999)
		afterGold = 99999;
	//更改金币
	dynamicData->setGoldNumber(afterGold);
	//更新显示
	m_pGoodLayer->updateShownOfGold(afterGold);
}

bool GameScene::subGold(int num)
{
	auto dynamicData = DynamicData::getInstance();

	int number = dynamicData->getGoldNumber();
	int afterGold = number - num;
	//现有金钱足够 减少成功
	if (afterGold >= 0)
	{
		dynamicData->setGoldNumber(afterGold);
		m_pGoodLayer->updateShownOfGold(afterGold);

		return true;
	}//减少失败
	return false;
}

void GameScene::addExp(int exp)
{
	//获取角色列表
	auto& list = m_pPlayerLayer->getCharacterList();
	//数据
	auto dynamicData = DynamicData::getInstance();
	auto staticData = StaticData::getInstance();
	//TODO:有的可能无法获得经验值
	for (auto player : list)
	{
		auto chartletName = player->getChartletName();

		auto curExp = dynamicData->getExp(chartletName);
		auto curLevel = dynamicData->getLevel(chartletName);
		//获取本级的应有属性
		auto& lvStruct = staticData->getDataByLevel(chartletName, curLevel);
		//获取下一等级的应有属性
		auto& nextLvStruct = staticData->getDataByLevel(chartletName,curLevel + 1);
		//增加经验
		curExp += exp;
		//升级
		if (curExp >= nextLvStruct.exp)
		{
			curExp -= nextLvStruct.exp;
			curLevel++;

			auto deltaProperties = nextLvStruct.properties - lvStruct.properties;
			auto maxHp = deltaProperties.hp + dynamicData->getMaxHitPoint(chartletName);
			auto maxMp = deltaProperties.mp + dynamicData->getMaxManaPoint(chartletName);
			//修改角色属性
			auto properties = dynamicData->getProperties(chartletName);
			dynamicData->setProperties(chartletName,properties + deltaProperties);
			dynamicData->setMaxHitPoint(chartletName,maxHp);
			dynamicData->setMaxManaPoint(chartletName,maxMp);
			//是否有技能学习 TODO
			if (!nextLvStruct.skill.empty())
			{
				dynamicData->studySkill(chartletName, nextLvStruct.skill);
			}
			printf("%s:level up\n",chartletName.c_str());
		}
		dynamicData->setLevel(chartletName,curLevel);
		dynamicData->setExp(chartletName,curExp);
	}
	m_pGoodLayer->updateShownOfExp();
	m_pGoodLayer->updateShownOfProps();
}

void GameScene::clickPath(const Point& location)
{
	auto tiledMap = m_pMapLayer->getTiledMap();
	auto nodePos = tiledMap->convertToNodeSpace(location);
	auto player = m_pPlayerLayer->getPlayer();

	auto fromTile = m_pMapLayer->convertToTileCoordinate(player->getPosition());
	auto toTile = m_pMapLayer->convertToTileCoordinate(nodePos);
	//是否点击了相同优先级的NPC
	auto npc = m_pScriptLayer->getClickedNPC(Rect(nodePos, Size(1.f, 1.f)), PRIORITY_SAME);

	player->setTriggerNPC(npc);
	bool bPassing = true;
	//显示点击特效
	auto collisionLayer = m_pMapLayer->getCollisionLayer();
	auto tileSize = tiledMap->getTileSize();
	Point pos((toTile.x + 0.5f) * tileSize.width, (toTile.y + 0.3f) * tileSize.height);

	m_pEffectLayer->showClickAnimation(pos, collisionLayer);
	//死路避免算法
	if (npc == nullptr)
	{
		int fromTileIndex = this->getAreaIndexAt(fromTile);
		int toTileIndex = this->getAreaIndexAt(toTile);
		//不在同一个区域，尝试获取另一个点
		if (fromTileIndex != toTileIndex)
		{
			bPassing = this->avoidDeadPath(toTile, fromTile, fromTileIndex);
		}
	}
	//不可通过，无奈退出
	if (!bPassing)
	{
		//提示文本
		m_pMessageLayer->showTip(STATIC_DATA_STRING("not_passing_dialog"), TextPosition::Middle, 1.f);
		return ;
	}
	//主角运动
	player->moveToward(toTile);
}

void GameScene::clickEvent(const Point& location)
{
	if (!m_pMessageLayer->isClickable())
		return;
	//正在显示文本，且还没有结束，"吸收"一次点击事件
	if (m_pMessageLayer->isShowingText() 
		&& !m_pMessageLayer->isWindOver())
	{
		m_pMessageLayer->windOver();
		//显示等待点击动画
		auto pos = m_pMessageLayer->getWaitClickPosition();
		m_pEffectLayer->showWaitClick(pos);
	}
	else//发送点击事件
	{
		//隐藏等待点击动画
		m_pEffectLayer->hideWaitClick();
		m_pMessageLayer->hideText();

		this->resumeLuaScript(WaitType::Click, 0);
	}
}

Character* GameScene::getCharacterByID(int uniqueID)
{
	Character* character = nullptr;
	//TODO:0 1 2 3表示主角
	auto& list = m_pPlayerLayer->getCharacterList();
	if (uniqueID < list.size())
	{
		character = list[uniqueID];
	}
	else
	{
		character = m_pScriptLayer->getNPCByID(uniqueID);

		if (character == nullptr)
		{
			character = m_pPlayerLayer->getPlayerOfID(uniqueID);
		}
	}
	return character;
}

void GameScene::saveData()
{
	//当前所在地图
	auto mapFilename = m_pMapLayer->getFilepath();
	//获取主角位置和主角方向
	auto player = m_pPlayerLayer->getPlayer();

	auto tilePos = m_pMapLayer->convertToTileCoordinate(player->getPosition());
	auto nDir = static_cast<int>(player->getDirection());
	//进行保存
	bool ret = DynamicData::getInstance()->save(mapFilename, tilePos, nDir);
	//提示是否保存成功
	auto &array = STATIC_DATA_ARRAY("save_result_array");
	auto text = array[ret].asString();
	m_pMessageLayer->showTip(text,TextPosition::Middle,1.5f);

	if (ret)
	{
		SoundManager::getInstance()->playEffect(STATIC_DATA_STRING("save_data_se"), 0);
	}
}

void GameScene::openBag()
{
	m_pGoodLayer->show();

	if (m_gameState == GameState::Normal)
	{
		m_pOperationLayer->setTouchEnabled(false);
	}
}

void GameScene::closeBag()
{
	m_pGoodLayer->hide();
	auto type = m_pGoodLayer->getType();

	if (m_gameState == GameState::Normal)
	{
		m_pOperationLayer->setTouchEnabled(true);
	}
	switch (type)
	{
	case GoodLayer::Type::Bag:
	case GoodLayer::Type::Skill:
		//在战斗状态下且未点击使用按钮，才会呼出行动菜单
		if (m_gameState == GameState::Fighting 
			&& m_pBattleScene->getGood() == nullptr)
		{
			m_pBattleScene->setVisibilityOfActionBtns(true);
		}
		break;
	case GoodLayer::Type::ShopBuy:
	case GoodLayer::Type::ShopSale://TODO:恢复协程
		this->resumeLuaScript(WaitType::Button, 0);
		break;
	case GoodLayer::Type::Seed:
		break;
	default:
		break;
	}
}

void GameScene::useGood(Good* good)
{
	//获取可用场合
	auto usageOccasion = good->getUsageOccasion();
	auto usageRange = good->getUsageRange();

	if (usageOccasion == UsageOccasion::All 
		|| (usageOccasion == UsageOccasion::Fighting && m_gameState == GameState::Fighting)
		|| (usageOccasion == UsageOccasion::Normal && m_gameState == GameState::Normal))
	{
		auto player = m_pGoodLayer->getSelectedPlayer();
		auto id = player->getUniqueID();

		if (m_gameState == GameState::Normal)
		{
			good->execute(id,id);
			//更新状态
			DynamicData::getInstance()->updateGood(good);
			m_pGoodLayer->updateShownOfGoods();
		}
		else if (m_gameState == GameState::Fighting)
		{
			m_pBattleScene->setGood(good);
			//关闭背包
			this->closeBag();
			//显示撤销按钮
			m_pBattleScene->setVisibilityOfUndoBtn(true);
		}
	}
	else
	{
		m_pMessageLayer->showTip(STATIC_DATA_STRING("use_default_text"), TextPosition::Middle, 1.f);
	}
}

bool GameScene::buyGood(Good* good)
{
	//获取价钱并扣除
	auto cost = good->getCost();
	bool ret = this->subGold(cost);

	auto goodName = good->getParentTableName();
	auto& array = STATIC_DATA_ARRAY("buy_text");
	string text = array.at(ret).asString();
	//购买成功
	if (ret)
	{
		this->addGood(goodName, 1);
		//对应物品减少一
		DynamicData::getInstance()->subShopGood(good, 1);
		m_pGoodLayer->updateShownOfGoods();
	}
	//提示文本
	m_pMessageLayer->showTip(text, TextPosition::Middle, 1.f);

	return ret;
}

bool GameScene::sellGood(Good* good)
{
	auto dynamicData = DynamicData::getInstance();
	auto sellRatio = dynamicData->getSellRatio();
	int cost = int(good->getCost() * sellRatio);
	//为0则不可出售
	bool ret = (cost != 0);
	auto& array = STATIC_DATA_ARRAY("sell_text");
	auto text = array.at(ret).asString();
	//出售成功 减少物品 增加金钱
	if (ret)
	{
		//是装备着的装备 先卸下
		if (good->getGoodType() == GoodType::Equipment
			&& good->isEquiped())
		{
			auto player = m_pGoodLayer->getSelectedPlayer();
			auto chartletName = player->getChartletName();
			auto equipType = good->getEquipmentType();

			dynamicData->unequip(chartletName,equipType);
		}
		else
		{
			dynamicData->subGood(good, 1);
		}
		this->addGold(cost);
		m_pGoodLayer->updateShownOfGoods(8);
	}
	//提示文本
	m_pMessageLayer->showTip(text,TextPosition::Middle,1.f);

	return ret;
}

void GameScene::endBattle()
{
	//恢复 TODO
	this->setGameState(GameState::Normal);
	//战斗场景消失
	m_pBattleScene->setVisible(false);
	m_pBattleScene->clear();
	m_pBattleScene->setBattleOver(true);
	//显示地图
	m_pMapLayer->setVisible(true);
	//操作层
	m_pOperationLayer->setVisible(true);
	m_pOperationLayer->setPosition(Point::ZERO);
	//解锁
	m_pGoodLayer->unlockPlayer();
	//播放原来的bgm
	auto bgm = m_pMapLayer->getBGMFilename();

	if (!bgm.empty())
	{
		SoundManager::getInstance()->playBackgroundMusic(bgm,-1);
	}
}

void GameScene::openQuest()
{
	m_pDisplayLayer->show(DisplayLayer::Type::Quest);
	//设置操作层状态不可用
	m_pOperationLayer->setTouchEnabled(false);
}

void GameScene::closeQuest()
{
	m_pDisplayLayer->hide();
	//设置操作层状态可用
	m_pOperationLayer->setTouchEnabled(true);
}

bool GameScene::isPathClickable() const
{
	return (!m_pGoodLayer->isOpened()) 
		&& (!m_pDisplayLayer->isOpened());
}
//----------------------------------------------脚本相关---------------------------------
void GameScene::addLuaLoader(lua_CFunction func)
{
	//空指针，直接返回
	if (func == nullptr)
		return ;
	//获取对应的表
	lua_getglobal(m_pLuaState, "package");
	lua_getfield(m_pLuaState, -1 ,"searchers");

	lua_pushcfunction(m_pLuaState, func);
	//把searchers表原来的2以后的项往后移1位
	for (int i = (int)lua_rawlen(m_pLuaState, -2) + 1; i > 2; i--)
	{
		//获取项到栈顶
		lua_rawgeti(m_pLuaState, -2, i - 1);
		//转移
		lua_rawseti(m_pLuaState, -3, i);
	}
	//searchers[2] = func
	lua_rawseti(m_pLuaState, -2, 2);

	lua_setfield(m_pLuaState, -2, "searchers");

	//清除栈顶，即清除 package表
	lua_pop(m_pLuaState, 1);
}

void GameScene::addLuaSearchPath(const char* path)
{
	//获取path
	lua_getglobal(m_pLuaState, "package");
	lua_getfield(m_pLuaState, -1,"path");

	const char* curPath = luaL_checkstring(m_pLuaState, -1);
	//添加
	lua_pushfstring(m_pLuaState, "%s;%s/?.lua", curPath, path);
	lua_setfield(m_pLuaState, -3, "path");

	lua_pop(m_pLuaState, 2);
}

int GameScene::executeScriptFile(const string& filename, bool force)
{
	bool bShouldLoad = false;

	FileUtils* utils = FileUtils::getInstance();
	string fullpath = utils->fullPathForFilename(filename);
	//仅保留文件名
	auto begin = filename.find_last_of('/');
	auto end = filename.find_last_of('.');

	auto name = filename.substr(begin + 1, end - begin - 1);
	//是否已经加载
	bool loaded = this->isLoadedScriptFile(name.c_str());

	if (!bShouldLoad && loaded)
		return LUA_OK;
	//获取lua的LOADED
	lua_getfield(m_pLuaState, LUA_REGISTRYINDEX, "_LOADED");

	int ret = LUA_ERRRUN;
	size_t size = 0;
	unique_ptr<char> chunk = utils->getUniqueDataFromFile(fullpath, &size);
	//加载并执行
	if (chunk != nullptr)
	{
		const char* data = chunk.get();

		ret = luaL_loadbuffer(m_pLuaState, data, size, nullptr);
		if (ret == LUA_OK)
		{
			ret = lua_pcall(m_pLuaState, 0, 0, 0);
			//赋值
			if (ret == LUA_OK)
			{
				lua_pushboolean(m_pLuaState, 1);
				lua_setfield(m_pLuaState, -2, name.c_str());
			}
		}
		chunk.reset();
	}
	//TODO:打印错误信息
	if (ret != LUA_OK)
	{
		printf("%s", luaL_checkstring(m_pLuaState, -1));
	}

	lua_pop(m_pLuaState, 1);

	return ret;
}

bool GameScene::isLoadedScriptFile(const char* filename) const
{
	//获取lua的LOADED
	lua_getfield(m_pLuaState, LUA_REGISTRYINDEX, "_LOADED");
	//是否存在对应的键值对
	int type = lua_getfield(m_pLuaState, -1, filename);

	lua_pop(m_pLuaState, 2);

	return type != LUA_TNIL;
}

int GameScene::executeString(const char* codes)
{
	luaL_loadstring(m_pLuaState, codes);
	return this->executeFunction(0, 0);
}

int GameScene::executeFunction(int nargs, int nresults)
{
	int ret = LUA_ERRRUN;
	//检测-(nargs + 1)是否是函数
	if (lua_isfunction(m_pLuaState, -(nargs + 1)))
	{
		ret = lua_pcall(m_pLuaState, nargs, nresults,NULL);
	}
	else
	{
		lua_pushfstring(m_pLuaState, "the index isn't function %d", -(nargs + 1));
	}
	return ret;
}

int GameScene::resumeFunction(int nargs)
{
	int ret = LUA_ERRRUN;
	//执行
	ret = lua_resume(m_pLuaState, nullptr, nargs);
	m_nCoroutineRet = ret;

	if (ret != LUA_OK && ret != LUA_YIELD)
	{
		printf("error:%s",luaL_checkstring(m_pLuaState, -1));
	}
	//当执行无误时应该返回LUA_OK或者是LUA_YIELD
	return ret;
}

int GameScene::yield(int nresult)
{
	return lua_yield(m_pLuaState, nresult);
}

int GameScene::resumeLuaScript(WaitType waitType, int nargs)
{
	int ret = LUA_ERRRUN;
	//当前等待类型为空 或者不同，唤醒失败
	if (waitType == WaitType::None ||
		waitType != m_pScriptLayer->getWaitType())
		return ret;
	//初始化等待类型
	m_pScriptLayer->setWaitType(WaitType::None);
	//恢复协程
	ret = this->resumeFunction(nargs);

	return ret;
}

int GameScene::getLuaGlobal(const char* name)
{
	return lua_getglobal(m_pLuaState, name);
}

int GameScene::getLuaField(int index, const char* key)
{
	return lua_getfield(m_pLuaState, index, key);
}

void GameScene::pushInt(int intValue)
{
	lua_pushinteger(m_pLuaState, (lua_Integer)intValue);
}

void GameScene::pushFloat(float floatValue)
{
	lua_pushnumber(m_pLuaState, (lua_Number)floatValue);
}

void GameScene::pushBoolean(bool boolValue)
{
	int value = boolValue ? 1 : 0;
	lua_pushboolean(m_pLuaState, value);
}

void GameScene::pushString(const char* stringValue)
{
	lua_pushstring(m_pLuaState, stringValue);
}

void GameScene::pushNil()
{
	lua_pushnil(m_pLuaState);
}

void GameScene::pop(int n)
{
	lua_pop(m_pLuaState, n);
}

bool GameScene::toLuaBoolean(int index)
{
	return lua_toboolean(m_pLuaState, index) == 1;
}

int GameScene::checkLuaInt(int index)
{
	return (int)luaL_checkinteger(m_pLuaState, index);
}

float GameScene::checkLuaFloat(int index)
{
	return (float)luaL_checknumber(m_pLuaState, index);
}

const char* GameScene::checkLuaString(int index)
{
	return luaL_checkstring(m_pLuaState, index);
}

int GameScene::getLuaLen(int index)
{
	return (int)lua_rawlen(m_pLuaState, index);
}

int GameScene::getLuaIndex(int index, int n)
{
	return lua_geti(m_pLuaState, index, (lua_Integer)n);
}

void GameScene::removeLuaIndex(int index)
{
	lua_remove(m_pLuaState, index);
}
