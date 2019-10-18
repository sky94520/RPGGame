#include "PlayerManager.h"
#include "../entity/Character.h"
#include "../entity/AStarController.h"
#include "../entity/FollowController.h"
#include "../data/StaticData.h"
#include "../data/DynamicData.h"
#include "../GameMacros.h"

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
	const unordered_map<string, PlayerData*>& data = dynamicData->getPlayerData();
	for (auto it = data.begin(); it != data.end(); it++)
	{
		string name = it->first;
		//创建角色
		Character* player = Character::create(name, direction);
		player->setTilePosition(tileCoordinate);
		layer->addChild(player);
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

	/*
	for (i = 0;i < (int)m_characters.size();i++)
	{
		auto player = m_characters.at(i);
		if (player->getChartletName() == chartletName)
			break;
	}
	*/
	return i;
}

Character* PlayerManager::getPlayerOfID(int id)
{
	//遍历寻找
	/*
	for (unsigned int i = 0;i < m_characters.size();i++)
	{
		auto player = m_characters.at(i);
		
		if (player->getUniqueID() == id)
			return player;
	}
	*/
	return nullptr;
}

bool PlayerManager::isCollidedWithCharacter(const Rect& rect)
{
	/*
	for (unsigned int i = 0;i < m_characters.size();i++)
	{
		auto r = m_characters.at(i)->getBoundingBox();

		if (r.intersectRect(rect))
			return true;
	}
	*/
	return false;
}

void PlayerManager::movePlayer(const SDL_Point& toTile)
{
	auto controller = m_controllers.front();

	controller->moveToward(toTile);
}

void PlayerManager::changeLayerOfPlayer(Node* layer)
{
	for (auto controller : m_controllers) {
		auto listener = controller->getControllerListener();
		auto player = static_cast<Character*>(listener);
		//层和父亲节点相同
		if (layer == player->getParent())
			break;
		player->removeFromParent();
		layer->addChild(player);
	}
}
