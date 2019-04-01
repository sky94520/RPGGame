#include "PlayerLayer.h"
#include "Character.h"

PlayerLayer::PlayerLayer()
{
}

PlayerLayer::~PlayerLayer()
{
	for (auto it = m_characters.begin();it != m_characters.end();)
	{
		auto player = *it;

		SDL_SAFE_RELEASE_NULL(player);
		it = m_characters.erase(it);
	}
}

bool PlayerLayer::init()
{
	return true;
}

void PlayerLayer::update(float dt)
{
}

Character* PlayerLayer::getPlayer()const
{
	return m_characters.front();
}

vector<Character*>& PlayerLayer::getCharacterList()
{
	return m_characters;
}

int PlayerLayer::getIndexOfCharacter(const string& chartletName)
{
	int i = -1;

	for (i = 0;i < (int)m_characters.size();i++)
	{
		auto player = m_characters.at(i);
		if (player->getChartletName() == chartletName)
			break;
	}
	return i;
}

Character* PlayerLayer::getPlayerOfID(int id)
{
	//遍历寻找
	for (unsigned int i = 0;i < m_characters.size();i++)
	{
		auto player = m_characters.at(i);
		
		if (player->getUniqueID() == id)
			return player;
	}
	return nullptr;
}

void PlayerLayer::addCharacter(Character* character)
{
	m_characters.push_back(character);
	SDL_SAFE_RETAIN(character);
}

bool PlayerLayer::isCollidedWithCharacter(const Rect& rect)
{
	for (unsigned int i = 0;i < m_characters.size();i++)
	{
		auto r = m_characters.at(i)->getBoundingBox();

		if (r.intersectRect(rect))
			return true;
	}
	return false;
}