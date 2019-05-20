#include "HelloWorldScene.h"
#include "entity/Character.h"
#include "layer/MapLayer.h"
#include "entity/AStar.h"
#include "entity/Controller.h"
#include "data/StaticData.h"
#include "entity/AStarController.h"
#include "entity/FollowController.h"

HelloWorld::HelloWorld()
	:m_pMapLayer(nullptr)
	,m_pController(nullptr)
	,m_pViewpointCharacter(nullptr)
{
}

HelloWorld::~HelloWorld()
{
	SDL_SAFE_RELEASE_NULL(m_pController);
	StaticData::purge();
}

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    Director::getInstance()->getRenderer()->setDrawColor(Color4B(0,255,255));
	//创建地图层
	m_pMapLayer = MapLayer::create("map/Map01.tmx");
	this->addChild(m_pMapLayer);

	auto tileSize = m_pMapLayer->getTiledMap()->getTileSize();
	Character::setTileSize((int)tileSize.width, (int)tileSize.height);

	//创建一个A星对象
	auto mapSize = m_pMapLayer->getTiledMap()->getMapSize();
	AStar* pAStar = AStar::create();
	pAStar->isPassing = SDL_CALLBACK_1(HelloWorld::isPassing, this);
	pAStar->setMapSize((int)mapSize.width, (int)mapSize.height);

    //add event listener
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = SDL_CALLBACK_2(HelloWorld::onTouchBegan, this);

    _eventDispatcher->addEventListener(listener, this);

	Character* sky = Character::create("sky");
	sky->setTilePosition(Point(6.f, 5.f));
	m_pMapLayer->getTiledMap()->addChild(sky);
	//创建控制器
	m_pController = AStarController::create(pAStar);
	SDL_SAFE_RETAIN(m_pController);
	m_pController->setControllerListener(sky);

	Character* moon = Character::create("moon");
	moon->setTilePosition(Point(6.f, 5.f));
	m_pMapLayer->getTiledMap()->addChild(moon);
	//控制器
	FollowController* followController = FollowController::create();
	followController->setControllerListener(moon);
	m_pController->setFollower(followController);

	Character* sun = Character::create("moon");
	sun->setTilePosition(Point(6.f, 5.f));
	m_pMapLayer->getTiledMap()->addChild(sun);
	//控制器
	FollowController* followController2 = FollowController::create();
	followController2->setControllerListener(sun);
	followController->setFollower(followController2);

	//中心点
	m_pViewpointCharacter = sky;
	this->scheduleUpdate();

    return true;
}

void HelloWorld::update(float dt)
{
	//视角跟随
	if (m_pViewpointCharacter != nullptr 
		&& m_pViewpointCharacter->isMoving())
	{
		m_pMapLayer->setViewpointCenter(m_pViewpointCharacter->getPosition());
	}
}

void HelloWorld::menuCloseCallback(Object* pSender)
{
}

bool HelloWorld::onTouchBegan(Touch* touch, SDL_Event* event)
{
	auto location = touch->getLocation();
	auto tileSize = m_pMapLayer->getTiledMap()->getTileSize();
	auto tiledMap = m_pMapLayer->getTiledMap();
	auto nodePos = tiledMap->convertToNodeSpace(location);

	SDL_Point toTile = { nodePos.x / tileSize.width, nodePos.y / tileSize.height };

	if (isPassing(toTile))
		m_pController->moveToward(toTile);

	return true;
}

bool HelloWorld::isPassing(const SDL_Point& tilePos)
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

