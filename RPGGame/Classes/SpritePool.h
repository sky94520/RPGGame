#ifndef __SpritePool_H__
#define __SpritePool_H__

#include "SDL_Engine/SDL_Engine.h"

#include <list>

USING_NS_SDL;
using namespace std;

class SpritePool
{
public:
	static SpritePool* getInstance()
	{
		if (m_pInstance == nullptr)
		{
			m_pInstance = new SpritePool();
		}
		return m_pInstance;
	}
	static void purge()
	{
		SDL_SAFE_DELETE(m_pInstance);
	}
private:
	SpritePool()
	{
	}
	~SpritePool()
	{
		auto it = m_objectList.begin();
		while (it != m_objectList.end())
		{
			auto sprite = *it;
			SDL_SAFE_RELEASE(sprite);
			it = m_objectList.erase(it);
		}
	}
public:
	Sprite* createObject()
	{
		if (m_objectList.size() > 0)
		{
			Sprite* ret = *m_objectList.begin();
			ret->autorelease();
			m_objectList.erase(m_objectList.begin());
			return ret;
		}
		else
		{
			return Sprite::create();
		}
	}
	void freeObject(Sprite* sprite)
	{
		sprite->setLocalZOrder(0);
		sprite->setGlobalZOrder(0);
		sprite->setFlipX(false);
		sprite->setRotation(0.f);
		sprite->setAnchorPoint(Point(0.5f, 0.5f));

		m_objectList.push_back(sprite);
	}
private:
	list<Sprite*> m_objectList;
	static SpritePool* m_pInstance;
};

SpritePool* SpritePool::m_pInstance = nullptr;
#endif