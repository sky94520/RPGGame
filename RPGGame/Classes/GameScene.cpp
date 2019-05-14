#include "GameScene.h"
#include "layer/MapLayer.h"
#include "data/DynamicData.h"

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

	this->setViewpointCenter(pos);
}

void GameScene::setViewpointCenter(const Point& position, unsigned millisecond)
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

	if (millisecond == 0)
	{
		action = Place::create(delta);
	}
	else
	{
		float duration = (float)millisecond / 1000;
		action = MoveTo::create(duration, delta);
	}

	action->setTag(tag);

	if (tiledMap->getActionByTag(tag) != nullptr)
	{
		tiledMap->stopActionByTag(tag);
	}
	tiledMap->runAction(action);
}
