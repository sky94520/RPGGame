#include "MapLayer.h"
#include "../GameMacros.h"
#include "../entity/Character.h"

MapLayer::MapLayer()
	:m_pTiledMap(nullptr)
	,m_pViewpointCharacter(nullptr)
{
}

MapLayer::~MapLayer()
{
	SDL_SAFE_RELEASE(m_pViewpointCharacter);
}

MapLayer* MapLayer::create(const string& filepath)
{
	auto layer = new MapLayer();

	if (layer && layer->init(filepath))
		layer->autorelease();
	else
		SDL_SAFE_DELETE(layer);

	return layer;
}

bool MapLayer::init()
{
	return true;
}

bool MapLayer::init(const string& filepath)
{
	m_filepath = filepath;

	m_pTiledMap = FastTiledMap::create(filepath);
	this->addChild(m_pTiledMap);

	return true;
}

void MapLayer::update(float dt)
{
	//视角跟随
	if (m_pViewpointCharacter != nullptr 
		&& m_pViewpointCharacter->isMoving())
	{
		this->setViewpointCenter(m_pViewpointCharacter->getPosition());
	}
}

void MapLayer::setViewpointCenter(const Point& pos, unsigned millisecond, bool isTilePos)
{
	//获取地图大小
	auto tileSize = m_pTiledMap->getTileSize();
	auto mapSize = m_pTiledMap->getMapSize();
	auto mapSizePixel = Size(tileSize.width * mapSize.width, tileSize.height * mapSize.height);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Size halfOfVisibleSize = visibleSize / 2.f;

	Point position = isTilePos ? Point(pos.x * tileSize.width, pos.y*tileSize.height) : pos;
	//如果主角坐标小于屏幕的一半，则取屏幕中心的坐标，否则取主角的坐标
	float x = max(position.x, halfOfVisibleSize.width);
	float y = max(position.y, halfOfVisibleSize.height);
	//显示区域不超过地图边界
	x = min(x, (mapSizePixel.width - halfOfVisibleSize.width));
	y = min(y, (mapSizePixel.height - halfOfVisibleSize.height));
	//实际移动的位置
	Point actualPosition = Point(x, y);
	//屏幕中心位置坐标
	Point centerOfView = Point(halfOfVisibleSize.width, halfOfVisibleSize.height);

	Point delta = centerOfView - actualPosition;
	if (delta.equals(Point::ZERO))
		return;
	//移动动作
	FiniteTimeAction* action = nullptr;
	if (millisecond == 0)
	{
		action = Place::create(delta);
	}
	else
	{
		float duration = millisecond / 1000.f;
		action = MoveTo::create(duration, delta);
	}
	const int tag = 10;
	action->setTag(tag);
	if (m_pTiledMap->getActionByTag(tag) != nullptr)
	{
		m_pTiledMap->stopActionByTag(tag);
	}
	m_pTiledMap->runAction(action);
}

void MapLayer::setViewpointFollow(Character* character)
{
	SDL_SAFE_RETAIN(character);
	SDL_SAFE_RELEASE(m_pViewpointCharacter);
	m_pViewpointCharacter = character;
}


void MapLayer::clear()
{
	if (m_pTiledMap != nullptr)
	{
		m_pTiledMap->removeFromParent();
		m_pTiledMap = nullptr;
	}
}

bool MapLayer::isPassing(int gid)
{
	//获取图块优先级
	//默认为人物优先级最高
	int priority = 1;
	//获取对应属性
	ValueMap* properties = nullptr;
	//获取失败
	if ( !m_pTiledMap->getTilePropertiesForGID(gid, &properties))
		return true;
	//获取图块优先级
	ValueMap::iterator it = properties->find("priority");

	if (it != properties->end())
	{
		int value = it->second.asInt();

		priority = value;
	}
	//优先级为0则不可通过
	return priority != 0;
}

bool MapLayer::isPassing(int gid, Direction direction)
{
	//获取对应属性
	ValueMap* properties = nullptr;
	if (!m_pTiledMap->getTilePropertiesForGID(gid, &properties))
		return true;
	//获取对应的键
	string key;
	switch (direction)
	{
		case Direction::Down: key = "pass_down"; break;
		case Direction::Left: key = "pass_left"; break;
		case Direction::Right: key = "pass_right"; break;
		case Direction::Up: key = "pass_up"; break;
	}
	auto it = properties->find(key);
	//获取对应值并返回
	if (it != properties->end())
	{
		bool ret = it->second.asBool();
		return ret;
	}
	else//默认为可通过
		return true;
}

Point MapLayer::convertToTileCoordinate(const Point& pos)
{
	auto tileSize = m_pTiledMap->getTileSize();

	auto tileCoordinateX = int(pos.x / tileSize.width);
	auto tileCoordinateY = int(pos.y / tileSize.height);

	return Point((float)tileCoordinateX, (float)tileCoordinateY);
}

string& MapLayer::getFilepath()
{
	return m_filepath;
}

string MapLayer::getMapName() const
{
	auto text = m_pTiledMap->getPropertyForName("name").asString();

	return text;
}

string MapLayer::getBGMFilename() const
{
	auto value = m_pTiledMap->getPropertyForName("bgm");

	if (value.getType() == Value::Type::NONE)
		return "";

	return value.asString();
}

FastLayer* MapLayer::getCollisionLayer() const
{
	return static_cast<FastLayer*>(m_pTiledMap->getLayer("collision layer"));
}

const TMXObjectGroup* MapLayer::getScriptObjectGroup() const
{
	return m_pTiledMap->getObjectGroup("script layer");
}

FastTiledMap* MapLayer::getTiledMap() const
{
	return m_pTiledMap;
}

