#include "GameScene.h"
#include "GameMacros.h"

#include "layer/MapLayer.h"
#include "layer/EffectLayer.h"
#include "layer/SpritePool.h"

#include "manager/PlayerManager.h"
#include "manager/ScriptManager.h"

#include "ui/OperationLayer.h"
#include "ui/BagLayer.h"

#include "data/StaticData.h"
#include "data/DynamicData.h"
#include "entity/AStar.h"
#include "entity/Character.h"
#include "entity/AStarController.h"

#include "script/LuaStack.h"

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
	,m_pBagLayer(nullptr)
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
	Size visibleSize = Director::getInstance()->getVisibleSize();

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
	//ui/背包层
	m_pBagLayer = BagLayer::create();
	m_pBagLayer->setDelegate(this);
	this->addChild(m_pBagLayer);
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
	//StaticData::getInstance()->loadCharacterFile("data/character.plist");
	//加载SpriteFrame
	auto frameCache = Director::getInstance()->getSpriteFrameCache();
	auto& framePathArray = STATIC_DATA_ARRAY("sprite_frame_array");
	for (auto& value : framePathArray)
	{
		auto filepath = value.asString();
		frameCache->addSpriteFramesWithFile(filepath);
	}
	//加载Animation动画
	auto animationCache = AnimationCache::getInstance();
	auto& aniPathArray = STATIC_DATA_ARRAY("animation_array");

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

	//只有正常状态下才可以寻路
	if (m_gameState != GameState::Normal)
		return true;
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
	m_pScriptManager->update(dt, m_gameState);
}

Node* GameScene::getCollisionLayer() const
{
	return m_pMapLayer->getCollisionLayer();
}
//---OperationDelegate---
void GameScene::openBag()
{
	m_pOperationLayer->setTouchEnabled(false);
	m_pBagLayer->setType(BagLayer::Type::Warehouse);
	m_pBagLayer->setVisibleofBagLayer(true);
}

void GameScene::saveProgress()
{
}
//---GoodLayerDelegate---
void GameScene::pageBtnCallback(GoodLayer* goodLayer, int value)
{
	m_pBagLayer->pageBtnCallback(goodLayer, value);
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

void GameScene::changeMap(const string& mapFilename, const Point& tileCoodinate)
{
	//MapLayer 改变当前地图
	m_pMapLayer->clear();
	m_pMapLayer->init(mapFilename);

	//AStar 更新A星算法的地图尺寸
	auto tiledMap = m_pMapLayer->getTiledMap();
	auto mapSize = tiledMap->getMapSize();
	StaticData::getInstance()->getAStar()->setMapSize(int(mapSize.width), int(mapSize.height));

	//MapLayer 改变当前中心点
	auto tileSize = tiledMap->getTileSize();
	//设置瓦片大小
	Character::setTileSize((int)tileSize.width, (int)tileSize.height);
	auto pos = Point(tileSize.width * (tileCoodinate.x + 0.5f)
		,tileSize.height * (tileCoodinate.y + 0.5f));
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
