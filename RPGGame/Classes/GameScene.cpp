#include "GameScene.h"
#include "layer/MapLayer.h"
#include "layer/EffectLayer.h"
#include "data/StaticData.h"
#include "data/DynamicData.h"
#include "GameMacros.h"

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
}

GameScene::GameScene()
	:m_pMapLayer(nullptr)
	,m_pEffectLayer(nullptr)
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
	//事件层
	auto listener = EventListenerTouchOneByOne::create();

	listener->setPriority(1);
	listener->onTouchBegan = SDL_CALLBACK_2(GameScene::onTouchBegan,this);
	_eventDispatcher->addEventListener(listener,this);

	//初始化地图
	this->initializeMap();

	return true;
}

bool GameScene::initializeMap()
{
	//默认使用第一个存档
	auto dynamicData = DynamicData::getInstance();
	dynamicData->initializeSaveData(1);

	//获取地图和主角所在位置
	const string& mapFilename = dynamicData->getMapFilename();
	const Point& tileCoordinate = dynamicData->getTileCoordinate();
	//改变地图
	this->changeMap(mapFilename, tileCoordinate);

	return true;
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
	/*
	if (isPassing(toTile))
		m_pController->moveToward(toTile);
		*/

	return true;
}

void GameScene::changeMap(const string& mapFilename, const Point& tileCoodinate)
{
	//改变当前地图
	m_pMapLayer->clear();
	m_pMapLayer->init(mapFilename);

	//改变当前中心点
	auto tileSize = m_pMapLayer->getTiledMap()->getTileSize();
	auto pos = Point(tileSize.width * (tileCoodinate.x + 0.5f)
		,tileSize.height * (tileCoodinate.y + 0.5f));

	m_pMapLayer->setViewpointCenter(pos);
}
