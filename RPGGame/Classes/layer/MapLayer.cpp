#include "MapLayer.h"

MapLayer::MapLayer()
	:m_pTiledMap(nullptr)
{
}

MapLayer::~MapLayer()
{
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

	//this->resetLocalZOrderOfTile();

	return true;
}

void MapLayer::setViewpointCenter(const Point& position, unsigned millisecond)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	const int tag = 10;
	//地图跟随点移动
	float x = (float)MAX(position.x, visibleSize.width / 2.f);
	float y = (float)MAX(position.y, visibleSize.height / 2.f);
	//获取地图层的地图

	auto tileSize = m_pTiledMap->getTileSize();
	auto mapSize = m_pTiledMap->getMapSize();
	auto mapSizePixel = Size(tileSize.width * mapSize.width, tileSize.height * mapSize.height);
	//不让显示区域超过地图的边界
	x = (float)MIN(x, (mapSizePixel.width - visibleSize.width / 2.f));
	y = (float)MIN(y, (mapSizePixel.height - visibleSize.height / 2.f));
	//实际移动的位置
	Point actualPosition = Point(x, y);
	//屏幕中心位置坐标
	Point centerOfView = Point(visibleSize.width / 2.f, visibleSize.height / 2.f);

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

	if (m_pTiledMap->getActionByTag(tag) != nullptr)
	{
		m_pTiledMap->stopActionByTag(tag);
	}
	m_pTiledMap->runAction(action);
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

bool MapLayer::isPassing(int gid,Direction direction)
{
	//获取对应属性
	ValueMap* properties = nullptr;
	if (!m_pTiledMap->getTilePropertiesForGID(gid, &properties))
		return true;
	//获取对应的键
	string key;

	/*
	switch (direction)
	{
	case Direction::Down: key = "pass_down"; break;
	case Direction::Left: key = "pass_left"; break;
	case Direction::Right: key = "pass_right"; break;
	case Direction::Up: key = "pass_up"; break;
	}
	*/
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

void MapLayer::resetLocalZOrderOfTile()
{
	//保存所有优先级为PRIORITY_HIGH的图块
	vector<int> tileIds;
	auto tilesets = m_pTiledMap->getTilesets();

	for (auto tileset : tilesets)
	{
		auto& properties = tileset->getProperties();
		
		for(auto itMap = properties.cbegin(); itMap != properties.cend(); itMap++)
		{
			auto id = itMap->first;
			auto& valueMap= itMap->second;

			auto it = valueMap.find("priority");

			//if (it != valueMap.cend() && it->second.asInt() == PRIORITY_HIGH)
			//	tileIds.push_back(id + tileset->firstGrid);
		}
	}
	//设置 优先级为PRIORITY_HIGH即为2 的图块localZOrder
	auto& children = this->getCollisionLayer()->getChildren();

	for (auto child : children)
	{
		//获取名字
		auto name = child->getName();

		if (name.empty())
			continue;
		
		int id = SDL_atoi(name.c_str());
		//设置localZOrder
		if (find(tileIds.begin(),tileIds.end(),id) != tileIds.end())
		{
			child->setLocalZOrder(child->getLocalZOrder() /*+ GameScene::CHARACTER_LOCAL_Z_ORDER*/);
		}
	}
}
