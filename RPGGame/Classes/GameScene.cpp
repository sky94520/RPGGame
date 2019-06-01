#include "GameScene.h"
#include "GameMacros.h"

#include "layer/MapLayer.h"
#include "layer/EffectLayer.h"
#include "layer/PlayerLayer.h"
#include "layer/SpritePool.h"
#include "layer/ScriptLayer.h"

#include "data/StaticData.h"
#include "data/DynamicData.h"
#include "entity/AStar.h"
#include "entity/Character.h"

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
	,m_pPlayerLayer(nullptr)
	,m_pScriptLayer(nullptr)
	,m_gameState(GameState::Normal)
{
}

GameScene::~GameScene()
{
}

bool GameScene::init()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();

	//地图层
	m_pMapLayer = MapLayer::create();
	this->addChild(m_pMapLayer);
	//特效层
	m_pEffectLayer = EffectLayer::create();
	this->addChild(m_pEffectLayer);
	//玩家层
	m_pPlayerLayer = PlayerLayer::create();
	this->addChild(m_pPlayerLayer);
	//脚本层
	m_pScriptLayer = ScriptLayer::create();
	this->addChild(m_pScriptLayer);
	//事件
	auto listener = EventListenerTouchOneByOne::create();

	listener->setPriority(1);
	listener->onTouchBegan = SDL_CALLBACK_2(GameScene::onTouchBegan,this);
	_eventDispatcher->addEventListener(listener,this);

	//初始化地图
	this->initializeMap();

	this->scheduleUpdate();

	return true;
}

bool GameScene::initializeMap()
{
	//创建A星算法实例,并存入StaticData中
	AStar* pAStar = AStar::create();
	pAStar->isPassing = SDL_CALLBACK_1(GameScene::isPassing, this);
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
	m_pPlayerLayer->initializePlayers(collisionLayer);
	//设置中心点
	m_pMapLayer->setViewpointFollow(m_pPlayerLayer->getPlayer());

	return true;
}

bool GameScene::isPassing(const SDL_Point& tilePos)
{
	auto tiledMap = m_pMapLayer->getTiledMap();
	auto mapSize = tiledMap->getMapSize();
	//不可超出地图
	if (tilePos.x < 0 || tilePos.x > (mapSize.width - 1)
		|| tilePos.y > (mapSize.height - 1) || tilePos.y < 0)
	{
		return false;
	}
	//检测碰撞层
	auto layer = m_pMapLayer->getCollisionLayer();
	auto gid = layer->getTileGIDAt(tilePos);
	bool ret = m_pMapLayer->isPassing(gid);

	return ret;
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

	//TODO:角色移动
	if (isPassing(toTile))
		m_pPlayerLayer->movePlayer(toTile);

	return true;
}

void GameScene::update(float dt)
{
	m_pMapLayer->update(dt);
	m_pScriptLayer->update(dt, m_gameState);
}

Layer* GameScene::getCollisionLayer() const
{
	return m_pMapLayer->getCollisionLayer();
}

void GameScene::changeMap(const string& mapFilename, const Point& tileCoodinate)
{
	//改变当前地图
	m_pMapLayer->clear();
	m_pMapLayer->init(mapFilename);
	//更新A星算法的地图尺寸
	auto tiledMap = m_pMapLayer->getTiledMap();
	auto mapSize = tiledMap->getMapSize();
	StaticData::getInstance()->getAStar()->setMapSize(int(mapSize.width), int(mapSize.height));

	//改变当前中心点
	auto tileSize = tiledMap->getTileSize();
	//设置瓦片大小
	Character::setTileSize((int)tileSize.width, (int)tileSize.height);
	auto pos = Point(tileSize.width * (tileCoodinate.x + 0.5f)
		,tileSize.height * (tileCoodinate.y + 0.5f));

	m_pMapLayer->setViewpointCenter(pos);
	//尝试获取脚本名称，若存在则执行
	auto scriptName = m_pMapLayer->getTiledMap()->getPropertyForName("script");

	if (scriptName.getType() == Value::Type::STRING)
	{
		m_pScriptLayer->getLuaStack()->executeScriptFile(scriptName.asString());
	}
}
