#include "ScriptManager.h"
#include "../GameMacros.h"
#include "../entity/LuaObject.h"
#include "../entity/AStarController.h"
#include "../script/BaseScript.h"
#include "../script/ObjectScript.h"
#include "../script/LuaStack.h"

ScriptManager::ScriptManager()
	:m_waitType(WaitType::None)
	,m_duration(0.f)
	,m_pLuaStack(nullptr)
{
}

ScriptManager::~ScriptManager()
{
	auto it1 = m_objects.begin();
	while (it1 != m_objects.end())
	{
		auto script = it1->second;
		it1 = m_objects.erase(it1);

		SDL_SAFE_RELEASE(script);
	}

	//release
	auto it2 = m_toAddedObjects.begin();
	while (it2 != m_toAddedObjects.end())
	{
		auto script = *it2;

		it2 = m_toAddedObjects.erase(it2);

		SDL_SAFE_RELEASE(script);
	}
	SDL_SAFE_RELEASE(m_pLuaStack);
}

bool ScriptManager::init()
{
	m_pLuaStack = LuaStack::create();
	SDL_SAFE_RETAIN(m_pLuaStack);
	//根据平台来添加脚本路径
	auto platform = Director::getInstance()->getPlatform();
	if (platform == "Windows" || platform == "Linux")
	{
		m_pLuaStack->addLuaSearchPath("Resources/script");
		FileUtils::getInstance()->addSearchPath("Resources/script");
	}
	else
		m_pLuaStack->addLuaSearchPath("script");
	//注册c函数给lua
	this->registerFuncs(m_pLuaStack->getLuaState());
	//加载必要的脚本
	m_pLuaStack->executeScriptFile("item.lua");
	return true;
}

void ScriptManager::update(float dt, GameState gameState)
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
				this->resumeCoroutine(WaitType::Time, 0);
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
			script->removeFromParent();
			SDL_SAFE_RELEASE(script);
			it = m_objects.erase(it);
		}
		else
			it++;
	}
	//把待添加对象添加到m_objects中
	for (auto it = m_toAddedObjects.begin(); it != m_toAddedObjects.end(); )
	{
		LuaObject* luaObject = *it;
		m_objects.insert(make_pair(luaObject->getLuaName(), luaObject));
		it = m_toAddedObjects.erase(it);
	}
}

LuaObject* ScriptManager::addLuaObject(const string& name, const string& chartletName, Node* layer, GameState gameState)
{
	LuaObject* luaObject = LuaObject::create(chartletName);
	luaObject->setLuaName(name);
	luaObject->setLuaStack(m_pLuaStack);

	layer->addChild(luaObject, PRIORITY_SAME);
	//待添加到容器
	SDL_SAFE_RETAIN(luaObject);
	//根据游戏状态来添加到不同的容器中
	if (gameState == GameState::Normal)
	{
		m_objects.insert(make_pair(name, luaObject));
	}
	else
	{
		m_toAddedObjects.push_back(luaObject);
	}

	return luaObject;
}

bool ScriptManager::removeLuaObject(const string& name)
{
	//是否删除成功
	bool ret = false;
	//在objects中查找
	auto it = m_objects.find(name);
	if (it != m_objects.end()) {
		auto object = it->second;
		object->setObsolete(true);
		ret = true;
	}
	else {
		for (auto iter = m_toAddedObjects.begin(); iter != m_toAddedObjects.end(); ) {
			auto object = *iter;
			if (object->getLuaName() == name) {
				SDL_SAFE_RELEASE(object);
				m_toAddedObjects.erase(iter);
				ret = true;
				break;
			}
		}
	}
	return ret;
}

void ScriptManager::triggerTouchScript(AStarController* controller, GameState gameState)
{
	Character* character = static_cast<Character*>(controller->getControllerListener());
	//TODO:只有处于正常状态下才会触发脚本对象
	if (gameState != GameState::Normal)
		return;
	auto rect = Rect(character->getPosition(),Size(1.f,1.f));
	//是否吞并事件
	bool bSwallowed = false;
	//触发高低优先级的脚本事件
	int priority = PRIORITY_HIGH | PRIORITY_LOW;

	for (auto it = m_objects.begin();it != m_objects.end();it++)
	{
		auto script = it->second;

		//发生碰撞 且未废弃
		if (script->intersectRect(rect) && !script->isObsolete())
		{
			//触发条件相同 优先级相同 并且没有吞并事件 调用对应的脚本
			if (script->getTriggerType() == TriggerType::Touch 
				&& (script->getPriority() & priority)
				&& !bSwallowed)
			{
				//执行脚本
				printf("DEBUG:trigger touch script\n");
				bSwallowed = script->execScriptFunc(character->getChartletName());
			}
		}
	}
}

LuaObject* ScriptManager::getClickedNPC(const Rect& r, int priority) const
{
	LuaObject* luaObject = nullptr;

	auto it = find_if(m_objects.begin(), m_objects.end(), [&r, &priority](const pair<string, LuaObject*>& pair)
	{
		auto luaObject = pair.second;

		return luaObject->getPriority() == priority 
			&& luaObject->intersectRect(r);
	});
	if (it != m_objects.end())
		luaObject = it->second;

	return luaObject;
}

LuaObject* ScriptManager::getLuaObject(const string& name)
{
	LuaObject* object = nullptr;
	auto it = m_objects.find(name);

	if (it == m_objects.end()) {
		for (auto iter = m_toAddedObjects.begin(); iter != m_toAddedObjects.end(); ) {
			if ((*iter)->getLuaName() == name) {
				object = *iter;
				break;
			}
		}
	}
	else
		object = it->second;

	return object;
}

int ScriptManager::resumeCoroutine(WaitType waitType, int nargs)
{
	int ret = LUA_ERRRUN;
	//当前等待类型为空 或者不同，唤醒失败
	if (waitType == WaitType::None ||
		waitType != m_waitType)
		return ret;
	//初始化等待类型
	this->setWaitType(WaitType::None);
	//恢复协程
	ret = m_pLuaStack->resumeFunction(nargs);

	return ret;
}

void ScriptManager::clear()
{
	//设置废弃标志 避免脚本对象在切换场景时出错
	for (auto it = m_objects.begin(); it != m_objects.end();it++)
	{
		auto script = it->second;
		script->setObsolete(true);	
	}
}

void ScriptManager::registerFuncs(lua_State* pL)
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

