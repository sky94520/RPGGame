#include "ScriptLayer.h"
#include "NonPlayerCharacter.h"
#include "GameScene.h"
#include "StaticData.h"

ScriptLayer::ScriptLayer()
	:m_waitType(WaitType::None)
	,m_waitTime(0.f)
{
}

ScriptLayer::~ScriptLayer()
{
	for (auto it = m_npcList.begin();it != m_npcList.end();)
	{
		auto script = *it;
		it = m_npcList.erase(it);

		script->clean();
		SDL_SAFE_RELEASE(script);
	}
	for (auto it = m_npcPool.begin(); it != m_npcPool.end();)
	{
		auto script = *it;

		it = m_npcPool.erase(it);

		script->clean();
		SDL_SAFE_RELEASE(script);
	}
}

bool ScriptLayer::init()
{
	return true;
}

void ScriptLayer::update(float dt)
{
	for (auto it = m_npcList.begin();it != m_npcList.end();)
	{
		auto script = *it;

		if (script->isObsolete())
		{
			script->clear();
		
			it = m_npcList.erase(it);
			m_npcPool.push_back(script);
		}
		//只有在应该更新的时候才会调用update函数
		else if (script->isUpdate())
		{
			script->update(dt);
			it++;
		}
		else
			it++;
	}
}

void ScriptLayer::updateWaitTime(float dt)
{
	if (m_waitType == WaitType::Time)
	{
		m_waitTime -= dt;
		//回调函数
		if (m_waitTime <= 0.f)
		{
			m_waitTime = 0.f;
			GameScene::getInstance()->resumeLuaScript(m_waitType, 0);
		}
	}
}

bool ScriptLayer::checkAndTriggerScriptEvent(const Rect& boundingBox, int playerID, TriggerType type, int priority)
{
	bool bEvent = false;
	//是否吞并事件
	bool bSwallowed = false;

	for (auto it = m_npcList.begin();it != m_npcList.end();it++)
	{
		auto script = *it;

		//id不同 且发生碰撞 且未废弃
		if (script->getUniqueID() != playerID 
			&& script->intersectRect(boundingBox) && !script->isObsolete())
		{
			//触发条件相同 优先级相同 并且没有吞并事件 调用对应的脚本
			if (script->isTrigger(type) && (script->getPriority() & priority)
				&& !bSwallowed)
			{
				bEvent = bSwallowed = script->execute(playerID);
			}
		}
	}
	return bEvent;
}

NonPlayerCharacter* ScriptLayer::getClickedNPC(const Rect& r, int priority) const
{
	NonPlayerCharacter* npc = nullptr;
	auto it = find_if(m_npcList.begin(), m_npcList.end(), [&r, &priority](NonPlayerCharacter* npc)
	{
		return npc->getPriority() == priority && npc->intersectRect(r);
	});

	if (it != m_npcList.end())
		npc = *it;

	return npc;
}

NonPlayerCharacter* ScriptLayer::getNPCByID(int id) const
{
	NonPlayerCharacter* npc = nullptr;

	auto callback = [&id](NonPlayerCharacter* npc)
	{
		return npc->getUniqueID() == id;
	};
	auto it = find_if(m_npcList.begin(), m_npcList.end(), callback);

	if (it != m_npcList.end())
		npc = *it;

	return npc;
}

NonPlayerCharacter* ScriptLayer::addNPC(const ValueMap& valueMap)
{
	NonPlayerCharacter* npc = nullptr;

	if (m_npcPool.empty())
	{
		npc = NonPlayerCharacter::create(valueMap);
		SDL_SAFE_RETAIN(npc);
	}
	else
	{
		npc = m_npcPool.back();
		m_npcPool.pop_back();

		npc->init(valueMap);
	}

	m_npcList.push_back(npc);

	return npc;
}

void ScriptLayer::clear()
{
	//设置废弃标志 避免脚本对象在切换场景时出错
	for (auto it = m_npcList.begin(); it != m_npcList.end();it++)
	{
		auto script = *it;
		script->setObsolete(true);	
	}
}
