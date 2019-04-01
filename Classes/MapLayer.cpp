#include "MapLayer.h"
#include "StaticData.h"
#include "GameScene.h"

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

	m_pTiledMap = TMXTiledMap::create(filepath);
	this->addChild(m_pTiledMap);

	this->resetLocalZOrderOfTile();

	return true;
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

TMXLayer* MapLayer::getMetaLayer() const
{
	return static_cast<TMXLayer*>(m_pTiledMap->getLayer("meta layer"));
}

TMXLayer* MapLayer::getCollisionLayer() const
{
	auto layerName = STATIC_DATA_STRING("collision_layer_name");

	return static_cast<TMXLayer*>(m_pTiledMap->getLayer(layerName));
}

TMXObjectGroup* MapLayer::getScriptObjectGroup() const
{
	auto layerName = STATIC_DATA_STRING("script_layer_name");

	return m_pTiledMap->getObjectGroup(layerName);
}

TMXTiledMap* MapLayer::getTiledMap() const
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

			if (it != valueMap.cend() && it->second.asInt() == PRIORITY_HIGH)
				tileIds.push_back(id + tileset->firstGrid);
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
			child->setLocalZOrder(child->getLocalZOrder() + GameScene::CHARACTER_LOCAL_Z_ORDER);
		}
	}
}
