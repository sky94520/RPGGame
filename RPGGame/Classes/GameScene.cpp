#include "GameScene.h"
#include "layer/MapLayer.h"
#include "data/StaticData.h"
#include "data/DynamicData.h"

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
