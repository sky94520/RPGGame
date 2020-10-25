#include "PlayerManager.h"
#include "../GameMacros.h"
#include "../GameScene.h"

#include "../entity/Character.h"
#include "../entity/AStarController.h"
#include "../entity/FollowController.h"

#include "../data/StaticData.h"
#include "../data/DynamicData.h"

#include "../layer/MapLayer.h"
#include "../layer/EffectLayer.h"

#include "../manager/ScriptManager.h"

PlayerManager::PlayerManager()
{
}

PlayerManager::~PlayerManager()
{
	auto it = m_controllers.begin();
	while (it != m_controllers.end())
	{
		auto controller = *it;
		SDL_SAFE_RELEASE(controller);

		it = m_controllers.erase(it);
	}
}

bool PlayerManager::init()
{
	return true;
}

void PlayerManager::initializePlayers(Node* layer)
{
	auto dynamicData = DynamicData::getInstance();
	auto& tileCoordinate = dynamicData->getTileCoordinate();
	Direction direction = Direction::Down;

	//获取角色数据
	Controller* last = nullptr;
	const unordered_map<string, PlayerData*>& data = dynamicData->getTotalPlayerData();
	for (auto it = data.begin(); it != data.end(); it++)
	{
		string name = it->first;
		//创建角色
		Character* player = Character::create(name, direction);
		player->setTilePosition(tileCoordinate);
		layer->addChild(player, PRIORITY_SAME);
		//创建控制器
		Controller* controller = nullptr;
		//仅有一个AStar控制器
		if (last == nullptr || m_controllers.size() == 0)
			controller = AStarController::create(StaticData::getInstance()->getAStar());
		else
			controller = FollowController::create();

		controller->setDurationPerGrid(0.2f);
		controller->setControllerListener(player);
		//保存控制器
		SDL_SAFE_RETAIN(controller);
		m_controllers.push_back(controller);
		//设置跟随
		if (last != nullptr)
			controller->setFollower(last);

		last = controller;
	}
	/*
	m_characters.push_back(character);
	SDL_SAFE_RETAIN(character);
	*/
}

void PlayerManager::update(float dt)
{
}

Character* PlayerManager::getPlayer()const
{
	auto player = m_controllers.front()->getControllerListener();
	return static_cast<Character*>(player);
}

AStarController* PlayerManager::getAStarController()
{
	return static_cast<AStarController*>(m_controllers.front());
}

int PlayerManager::getIndexOfCharacter(const string& chartletName)
{
	int i = -1;
	for (int j = 0;j < m_controllers.size(); j++){
		Controller* controller = m_controllers[j];
		ControllerListener* listener = controller->getControllerListener();
		Character* player = static_cast<Character*>(listener);
		if (player->getChartletName() == chartletName) {
			i = j;
			break;
		}
	}
	return i;
}

Character* PlayerManager::getPlayerOfID(int id)
{
	//遍历寻找
	for (int j = 0;j < m_controllers.size(); j++){
		Controller* controller = m_controllers[j];
		ControllerListener* listener = controller->getControllerListener();
		Character* player = static_cast<Character*>(listener);
		if (player->getUniqueID() == id)
			return player;
	}
	return nullptr;
}

Controller* PlayerManager::getControllerOfPlayerID(int id)
{
	//遍历寻找
	for (int j = 0;j < m_controllers.size(); j++){
		Controller* controller = m_controllers[j];
		ControllerListener* listener = controller->getControllerListener();
		Character* player = static_cast<Character*>(listener);
		if (player->getUniqueID() == id)
			return controller;
	}
	return nullptr;
}

bool PlayerManager::isCollidedWithCharacter(const Rect& rect)
{
	for (int j = 0;j < m_controllers.size(); j++){
		Controller* controller = m_controllers[j];
		ControllerListener* listener = controller->getControllerListener();
		Character* player = static_cast<Character*>(listener);
		Rect r = player->getBoundingBox();
		if (r.intersectRect(rect))
			return true;
	}
	return false;
}

void PlayerManager::movePlayer(const SDL_Point& toTile)
{
	auto controller = m_controllers.front();
	controller->moveToward(toTile);
}

bool PlayerManager::movePlayer(Touch* touch)
{
	auto gameScene = GameScene::getInstance();
	//转换成地图坐标
	auto location = touch->getLocation();
	auto pMapLayer = gameScene->getMapLayer();
	auto tiledMap = pMapLayer->getTiledMap();
	auto tileSize = tiledMap->getTileSize();
	auto nodePos = tiledMap->convertToNodeSpace(location);
	SDL_Point toTile = { int(nodePos.x / tileSize.width), int(nodePos.y / tileSize.height) };

	//显示点击特效
	auto collisionLayer = pMapLayer->getCollisionLayer();
	Point pos((toTile.x + 0.5f) * tileSize.width, (toTile.y + 0.3f) * tileSize.height);

	gameScene->getEffectLayer()->showClickAnimation(pos, collisionLayer);

	//是否点击了相同优先级的NPC
	LuaObject* luaObject = gameScene->getScriptManager()->getClickedNPC(Rect(nodePos, Size(1.f, 1.f)), PRIORITY_SAME);

	auto controller = this->getAStarController();
	controller->setTriggerObject(luaObject);
	//目的地无法移动
	if (luaObject == nullptr && !gameScene->isPassing(toTile))
	{
		return true;
	}
	this->movePlayer(toTile);

	return true;
}

void PlayerManager::changeLayerOfPlayer(Node* layer, const Point& location)
{
	for (auto controller : m_controllers) {
		ControllerListener* listener = controller->getControllerListener();
		Character* player = static_cast<Character*>(listener);
		//层和父亲节点相同
		if (layer == player->getParent())
			break;
		player->setPosition(location);
		SDL_SAFE_RETAIN(player);
		player->removeFromParent();
		layer->addChild(player);
		SDL_SAFE_RELEASE(player);
	}
}

vector<Character*> PlayerManager::getCharacterList()
{
	vector<Character*> results;
	for (auto controller : m_controllers) {
		ControllerListener* listener = controller->getControllerListener();
		Character* player = static_cast<Character*>(listener);
		results.push_back(player);
	}
	return results;
}
