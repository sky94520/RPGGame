#include "ScriptLayer.h"
#include "../GameMacros.h"
#include "../entity/LuaObject.h"
#include "../script/BaseScript.h"
#include "../script/ObjectScript.h"
#include "../script/LuaStack.h"

ScriptLayer::ScriptLayer()
	:m_waitType(WaitType::None)
	,m_duration(0.f)
	,m_pLuaStack(nullptr)
{
}

ScriptLayer::~ScriptLayer()
{
	auto it1 = m_objects.begin();
	while (it1 != m_objects.end())
	{
		auto script = it1->second;
		it1 = m_objects.erase(it1);

		//script->clean();
		SDL_SAFE_RELEASE(script);
	}

	//release
	auto it2 = m_toAddedObjects.begin();
	while (it2 != m_toAddedObjects.end())
	{
		auto script = *it2;

		it2 = m_toAddedObjects.erase(it2);

		//script->clean();
		SDL_SAFE_RELEASE(script);
	}
	SDL_SAFE_RELEASE(m_pLuaStack);
}

bool ScriptLayer::init()
{
	m_pLuaStack = LuaStack::create();
	SDL_SAFE_RETAIN(m_pLuaStack);
	//根据平台来添加路径
	auto platform = Director::getInstance()->getPlatform();
	if (platform == "Windows" || platform == "Linux")
		m_pLuaStack->addLuaSearchPath("Resources/script");
	else
		m_pLuaStack->addLuaSearchPath("script");
	//注册c函数给lua
	this->registerFuncs(m_pLuaStack->getLuaState());

	return true;
}

void ScriptLayer::update(float dt, GameState gameState)
{
	//更新等待时间
	if (gameState != GameState::Normal)
	{
		if (m_waitType == WaitType::Time)
		{
			m_duration -= dt;
			//回调函数
			if (m_duration <= 0.f)
			{
				m_duration = 0.f;
				//GameScene::getInstance()->resumeLuaScript(m_waitType, 0);
			}
		}
		return;
	}
	//仅仅在正常状态下才会对脚本对象进行更新
	for (auto it = m_objects.begin();it != m_objects.end();)
	{
		auto script = it->second;

		if (script->isObsolete())
		{
			script->clear();
		
			it = m_objects.erase(it);
			m_toAddedObjects.push_back(script);
		}
		else
			it++;
	}
}

LuaObject* ScriptLayer::addLuaObject(const string& name, const string& chartletName, Layer* layer)
{
	LuaObject* luaObject = LuaObject::create(chartletName);
	luaObject->setLuaName(name);
	layer->addChild(luaObject);
	//待添加到容器
	SDL_SAFE_RETAIN(luaObject);
	m_toAddedObjects.push_back(luaObject);

	return luaObject;
}

bool ScriptLayer::removeLuaObject(const string& name)
{
	return false;
}

LuaObject* ScriptLayer::getClickedNPC(const Rect& r, int priority) const
{
	LuaObject* npc = nullptr;

	return npc;
}

void ScriptLayer::clear()
{
	//设置废弃标志 避免脚本对象在切换场景时出错
	for (auto it = m_objects.begin(); it != m_objects.end();it++)
	{
		auto script = it->second;
		script->setObsolete(true);	
	}
}

void ScriptLayer::registerFuncs(lua_State* pL)
{
	static const luaL_Reg cpplibs[] = {
		{"base", open_base},
		{"movement", open_movement},
		{"screen", open_screen},
		{"timer", open_timer},
		{"object", open_object},
		{NULL, NULL}
	};
	const luaL_Reg* lib = nullptr;
	for (lib = cpplibs;lib->func;lib++)
	{
		luaL_requiref(pL, lib->name, lib->func, 1);
		lua_pop(pL, 1);
	}
}
