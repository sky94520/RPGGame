#include "GameScene.h"
#include "GameMacros.h"

#include "layer/MapLayer.h"
#include "layer/EffectLayer.h"
#include "layer/SpritePool.h"
#include "layer/MessageLayer.h"

#include "manager/PlayerManager.h"
#include "manager/ScriptManager.h"

#include "ui/OperationLayer.h"
#include "ui/BagLayer.h"

#include "data/StaticData.h"
#include "data/DynamicData.h"
#include "entity/Good.h"
#include "entity/AStar.h"
#include "entity/Character.h"
#include "entity/AStarController.h"

#include "script/LuaStack.h"

#include "battle/BattleScene.h"

//static
GameScene* GameScene::s_pInstance = nullptr;

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
	StaticData::purge();
	DynamicData::purge();
	SpritePool::purge();
}

GameScene::GameScene()
	:m_pMapLayer(nullptr)
	,m_pEffectLayer(nullptr)
	,m_pOperationLayer(nullptr)
	,m_pBattleScene(nullptr)
	,m_pBagLayer(nullptr)
	,m_pMsgLayer(nullptr)
	,m_pPlayerManager(nullptr)
	,m_pScriptManager(nullptr)
	,m_gameState(GameState::Normal)
{
}

GameScene::~GameScene()
{
}

bool GameScene::init()
{
	//加载资源
	this->preloadResources();
	//地图层
	m_pMapLayer = MapLayer::create();
	this->addChild(m_pMapLayer);
	//特效层
	m_pEffectLayer = EffectLayer::create();
	this->addChild(m_pEffectLayer);
	//ui/展示层
	m_pOperationLayer = OperationLayer::create();
	m_pOperationLayer->setDelegate(this);
	this->addChild(m_pOperationLayer);
	//战斗层
	m_pBattleScene = BattleScene::create();
	this->addChild(m_pBattleScene);
	//ui/背包层
	m_pBagLayer = BagLayer::create();
	m_pBagLayer->setDelegate(this);
	this->addChild(m_pBagLayer);
	//文本显示层
	m_pMsgLayer = MessageLayer::create();
	this->addChild(m_pMsgLayer);
	//玩家层
	m_pPlayerManager = PlayerManager::create();
	this->addChild(m_pPlayerManager);
	//脚本层
	m_pScriptManager = ScriptManager::create();
	this->addChild(m_pScriptManager);
	//点击事件
	auto listener = EventListenerTouchOneByOne::create();
	listener->setPriority(1);
	listener->onTouchBegan = SDL_CALLBACK_2(GameScene::onTouchBegan,this);
	_eventDispatcher->addEventListener(listener,this);

	//初始化地图
	this->initializeMap();
	//添加角色移动结束触发器
	_eventDispatcher->addEventCustomListener(AStarController::CHARACTER_MOVE_TO_TILE,
		SDL_CALLBACK_1(GameScene::moveToTile, this), this);
	this->endBattle();

	this->scheduleUpdate();

	return true;
}

bool GameScene::initializeMap()
{
	//创建A星算法实例,并存入StaticData中
	AStar* pAStar = AStar::create();
	pAStar->setDelegate(this);
	StaticData::getInstance()->setAStar(pAStar);

	//默认使用第一个存档
	auto dynamicData = DynamicData::getInstance();
	dynamicData->initializeSaveData(1);
	//获取地图和主角所在位置
	const string& mapFilename = dynamicData->getMapFilename();
	const Point& tileCoordinate = dynamicData->getTileCoordinate();
	//改变地图
	this->changeMap(mapFilename, tileCoordinate);
	auto collisionLayer = m_pMapLayer->getCollisionLayer();
	//初始化角色
	m_pPlayerManager->initializePlayers(collisionLayer);
	//设置中心点
	m_pMapLayer->setViewpointFollow(m_pPlayerManager->getPlayer());

	return true;
}

void GameScene::preloadResources()
{
	//加载SpriteFrame
	auto frameCache = Director::getInstance()->getSpriteFrameCache();
	//StaticData::getInstance()->loadCharacterFile("data/character.plist");
	auto staticData = StaticData::getInstance();
	auto& framePathArray = staticData->getValueForKey("sprite_frame_array");

	for (auto& value : framePathArray)
	{
		auto filepath = value.asString();
		frameCache->addSpriteFramesWithFile(filepath);
	}
	//加载Animation动画
	auto animationCache = AnimationCache::getInstance();
	auto& aniPathArray = staticData->getValueForKey("animation_array");

	for (auto& value : aniPathArray)
	{
		auto filepath = value.asString();
		animationCache->addAnimationsWithFile(filepath);
	}
	//加载纹理
	Director::getInstance()->getTextureCache()->addImage("img/system/IconSet.png");
}

bool GameScene::isPassing(const SDL_Point& tilePos) const
{
	auto tiledMap = m_pMapLayer->getTiledMap();
	auto mapSize = tiledMap->getMapSize();
	//不可超出地图
	if (tilePos.x < 0 || tilePos.x > mapSize.width
		|| tilePos.y > mapSize.height || tilePos.y < 0)
	{
		return false;
	}
	//检测碰撞层
	auto layer = m_pMapLayer->getCollisionLayer();
	auto gid = layer->getTileGIDAt(tilePos);
	bool ret = m_pMapLayer->isPassing(gid);
	//再次检测脚本对象
	if (ret)
	{
		auto tileSize = tiledMap->getTileSize();
		//获取矩形
		Rect r;
		r.origin = Point(tileSize.width * (tilePos.x + 0.5f), tileSize.height * (tilePos.y + 0.5f));
		r.size = Size(1.f, 1.f);
		auto npc = m_pScriptManager->getClickedNPC(r, PRIORITY_SAME);
		ret = ( npc == nullptr ? true : false);
	}
	return ret;
}

bool GameScene::isPassing4(const SDL_Point& tilePos, Direction direction) const
{
	auto layer = m_pMapLayer->getCollisionLayer();
	auto gid = layer->getTileGIDAt(tilePos);
	bool ret = m_pMapLayer->isPassing(gid, direction);
	return ret;
}

void GameScene::moveToTile(EventCustom* eventCustom)
{
	AStarController* controller = static_cast<AStarController*>(eventCustom->getUserData());

	m_pScriptManager->triggerTouchScript(controller, m_gameState);
}

bool GameScene::onTouchBegan(Touch* touch, SDL_Event* event)
{
	//只有正常状态下才可以寻路
	if (m_gameState != GameState::Normal || m_pBagLayer->isShowing())
		return true;
	//转换成地图坐标
	auto location = touch->getLocation();
	auto tiledMap = m_pMapLayer->getTiledMap();
	auto tileSize = tiledMap->getTileSize();
	auto nodePos = tiledMap->convertToNodeSpace(location);
	SDL_Point toTile = { int(nodePos.x / tileSize.width), int(nodePos.y / tileSize.height) };

	//显示点击特效
	auto collisionLayer = m_pMapLayer->getCollisionLayer();
	Point pos((toTile.x + 0.5f) * tileSize.width, (toTile.y + 0.3f) * tileSize.height);

	m_pEffectLayer->showClickAnimation(pos, collisionLayer);

	//是否点击了相同优先级的NPC
	LuaObject* luaObject = m_pScriptManager->getClickedNPC(Rect(nodePos, Size(1.f, 1.f)), PRIORITY_SAME);

	if (luaObject != nullptr)
	{
		auto controller = m_pPlayerManager->getAStarController();
		controller->setTriggerObject(luaObject);
	}
	//目的地无法移动
	else if (!isPassing(toTile))
	{
		return true;
	}
	m_pPlayerManager->movePlayer(toTile);

	return true;
}

void GameScene::update(float dt)
{
	m_pMapLayer->update(dt);
	m_pPlayerManager->update(dt);
	m_pScriptManager->update(dt, m_gameState);
}

Node* GameScene::getCollisionLayer() const
{
	return m_pMapLayer->getCollisionLayer();
}

void GameScene::setGameState(GameState state)
{
	if (m_gameState == state)
		return;
	m_gameState = state;
	//操作层可点击
	bool enable = m_gameState == GameState::Normal ? true : false;
	m_pOperationLayer->setTouchEnabled(enable);
	//TODO: 从脚本中结束，尝试恢复行走
	if (m_gameState == GameState::Normal)
	{
		//m_pPlayerLayer->getPlayer()->popStepAndAnimate();
	}
}

vector<Character*> GameScene::getCharacterList()
{ 
	return m_pPlayerManager->getCharacterList(); 
}

LuaStack* GameScene::getLuaStack() const
{
	return m_pScriptManager->getLuaStack();
}
//---OperationDelegate---
void GameScene::openBag()
{
	m_pOperationLayer->setTouchEnabled(false);
	m_pBagLayer->setType(BagLayer::Type::Bag);
	m_pBagLayer->setVisible(true);
}

void GameScene::saveProgress()
{
}
//---GoodLayerDelegate---
void GameScene::pageBtnCallback(GoodLayer* goodLayer, int value)
{
	m_pBagLayer->pageBtnCallback(goodLayer, value);
}

void GameScene::updateGoodHook(LabelAtlas* pCostLabel, LabelAtlas* pNumberLabel, int cost, int number)
{
	m_pBagLayer->updateGoodHook(pCostLabel, pNumberLabel, cost, number);
}

void GameScene::useBtnCallback(GoodLayer* goodLayer)
{
	m_pBagLayer->useBtnCallback(goodLayer);
}

void GameScene::equipBtnCallback(GoodLayer* goodLayer)
{
	m_pBagLayer->equipBtnCallback(goodLayer);
}

void GameScene::closeBtnCallback(GoodLayer* goodLayer)
{
	m_pOperationLayer->setTouchEnabled(true);
	m_pBagLayer->closeBtnCallback(goodLayer);
}

void GameScene::selectGoodCallback(GoodLayer* goodLayer, GoodInterface* good)
{
	m_pBagLayer->selectGoodCallback(goodLayer, good);
}

bool GameScene::touchOutsideCallback(GoodLayer* goodLayer)
{
	return m_pBagLayer->touchOutsideCallback(goodLayer);
}

void GameScene::changeMap(const string& mapFilename, const Point& tileCoodinate)
{
	//MapLayer 改变当前地图
	m_pMapLayer->clear();
	m_pMapLayer->init(mapFilename);

	//AStar 更新A星算法的地图尺寸
	auto tiledMap = m_pMapLayer->getTiledMap();
	auto mapSize = tiledMap->getMapSize();
	StaticData::getInstance()->getAStar()->setMapSize(int(mapSize.width), int(mapSize.height));

	auto tileSize = tiledMap->getTileSize();
	Character::setTileSize((int)tileSize.width, (int)tileSize.height);
	Point pos = Point(tileSize.width * (tileCoodinate.x + 0.5f),tileSize.height * (tileCoodinate.y + 0.5f));
	//MapLayer 改变当前中心点
	m_pMapLayer->setViewpointCenter(pos);

	//PlayerManager 更改玩家层的玩家所在的层
	auto collisionLayer = m_pMapLayer->getCollisionLayer();
	m_pPlayerManager->changeLayerOfPlayer(collisionLayer, pos);

	//ScriptManager 清除脚本事件
	m_pScriptManager->clear();
	//尝试获取脚本名称，若存在则执行
	auto scriptName = m_pMapLayer->getTiledMap()->getPropertyForName("script");
	if (scriptName.getType() == Value::Type::STRING)
	{
		m_pScriptManager->getLuaStack()->executeScriptFile(scriptName.asString(), true);
	}
}

void GameScene::closeBag()
{
	m_pBagLayer->setVisible(false);
	if (m_gameState == GameState::Normal)
	{
		m_pOperationLayer->setTouchEnabled(true);
	}
	auto type = m_pBagLayer->getType();
	switch (type)
	{
	case BagLayer::Type::Bag:
	case BagLayer::Type::Skill:
		//在战斗状态下且未点击使用按钮，才会呼出行动菜单
		if (m_gameState == GameState::Fighting
			&& m_pBattleScene->getGood() == nullptr)
		{
			m_pBattleScene->setVisibileOfActionBtns(true);
		}
		break;
	case BagLayer::Type::ShopBuy:
	case BagLayer::Type::ShopSell://TODO:恢复协程
		m_pScriptManager->resumeCoroutine(WaitType::Button, 0);
		break;
	case BagLayer::Type::SeedBag:
		break;
	default:
		break;
	}
}
void GameScene::startBattle(const unordered_map<string, int>& enemyData)
{
	//设置状态
	this->setGameState(GameState::Fighting);
	//出现战斗场景
	m_pBattleScene->setVisible(true);
	//隐藏地图
	m_pMapLayer->setVisible(false);
	//添加我方和敌人
	m_pBattleScene->startBattle(enemyData);
	//播放战斗音乐
	SoundManager::getInstance()->playBackgroundMusic(STATIC_DATA_STRING("battle_bgm"), -1);
	SoundManager::getInstance()->playEffect(STATIC_DATA_STRING("battle_me"), 0);
}

void GameScene::endBattle()
{
	//恢复 TODO
	this->setGameState(GameState::Normal);
	//战斗场景消失
	m_pBattleScene->setVisible(false);
	m_pBattleScene->clear();
	//显示地图
	m_pMapLayer->setVisible(true);
	//操作层
	m_pOperationLayer->setVisible(true);
	m_pOperationLayer->setPosition(Point::ZERO);
	//解锁
	m_pBagLayer->unlockPlayer();
	//播放原来的bgm
	auto bgm = m_pMapLayer->getBGMFilename();

	if (!bgm.empty())
	{
		SoundManager::getInstance()->playBackgroundMusic(bgm, -1);
	}
}

Good* GameScene::addGood(const string& goodName, int number)
{
	auto good = DynamicData::getInstance()->addGood(goodName, number);
	//更新背包层
	m_pBagLayer->updateGoods();
	return good;
}

bool GameScene::removeGood(const string& goodName, int number)
{
	bool ret = DynamicData::getInstance()->removeGood(goodName, number);
	m_pBagLayer->updateGoods();
	return ret;
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
		auto player = m_pBagLayer->getSelectedPlayer();
		auto id = player->getUniqueID();

		if (m_gameState == GameState::Normal)
		{
			good->execute(id, id);
			//更新物品状态
			DynamicData::getInstance()->updateGood(good);
			m_pBagLayer->updateGoods();
		}
		else if (m_gameState == GameState::Fighting)
		{
			m_pBattleScene->setGood(good);
			//关闭背包
			this->closeBag();
			//显示撤销按钮
			m_pBattleScene->setVisibileOfUndoBtn(true);
		}
	}
	else
	{
		m_pMsgLayer->showTip(STATIC_DATA_STRING("use_default_text"), TextPosition::Middle, 1.f);
	}
}

bool GameScene::buyGood(Good* good)
{
	//获取价钱并扣除
	auto cost = good->getCost();
	bool ret = m_pBagLayer->removeGold(cost);

	auto goodName = good->getPrototype();
	const Json::Value& array = StaticData::getInstance()->getValueForKey("buy_text");
	string text = array[ret].asString();
	//购买成功
	if (ret)
	{
		this->addGood(goodName, 1);
		//对应物品减少一
		//DynamicData::getInstance()->subShopGood(good, 1);
		m_pBagLayer->updateGoods();
	}
	//提示文本
	m_pMsgLayer->showTip(text, TextPosition::Middle, 1.f);

	return ret;
}

bool GameScene::sellGood(Good* good)
{
	auto dynamicData = DynamicData::getInstance();
	auto sellRatio = dynamicData->getSellRatio();
	int cost = int(good->getCost() * sellRatio);
	//为0则不可出售
	bool ret = (cost != 0);
	const Json::Value& array = StaticData::getInstance()->getValueForKey("sell_text");
	auto text = array[ret].asString();
	//出售成功 减少物品 增加金钱
	if (ret)
	{
		//是装备着的装备 先卸下
		if (good->getGoodType() == GoodType::Equipment
			&& good->isEquiped())
		{
			auto player = m_pBagLayer->getSelectedPlayer();
			auto chartletName = player->getChartletName();
			auto equipType = good->getEquipmentType();

			dynamicData->unequip(chartletName, equipType);
		}
		else
		{
			dynamicData->removeGood(good, 1);
		}
		m_pBagLayer->addGold(cost);
		m_pBagLayer->updateGoods();
	}
	//提示文本
	m_pMsgLayer->showTip(text, TextPosition::Middle, 1.f);

	return ret;
}
