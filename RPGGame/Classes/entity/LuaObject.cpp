#include "LuaObject.h"
#include "../GameMacros.h"
#include "../script/LuaStack.h"

LuaObject::LuaObject()
	:m_pLuaStack(nullptr)
	,m_bUsingBox(false)
	,m_triggerType(TriggerType::None)
	,m_nPriority(PRIORITY_SAME)
	,m_bObsolete(false)
{
}

LuaObject::~LuaObject()
{
	SDL_SAFE_RELEASE(m_pLuaStack);
}

LuaObject* LuaObject::create(const string& chartletName)
{
	LuaObject* luaObject = new LuaObject();

	if (luaObject != nullptr && luaObject->init(chartletName))
		luaObject->autorelease();
	else
		SDL_SAFE_DELETE(luaObject);

	return luaObject;
}

bool LuaObject::init(const string& chartletName)
{
	bool ret = true;
	if (Character::init(chartletName))
	{
		ret = true;
	}

	return ret;
}

void LuaObject::setLuaStack(LuaStack* pLuaStack)
{
	SDL_SAFE_RETAIN(pLuaStack);
	SDL_SAFE_RELEASE(m_pLuaStack);
	m_pLuaStack = pLuaStack;
}

bool LuaObject::execScriptFunc(const string& playerName)
{
	int result = LUA_ERRRUN;
	bool ret = false;

	if (m_funcName.empty())
		return ret;
	//获得函数
	m_pLuaStack->getLuaGlobal(m_funcName.c_str());
	//放入参数
	m_pLuaStack->pushString(playerName.c_str());
	//执行函数
	result = m_pLuaStack->resumeFunction(1);

	int n = lua_gettop(m_pLuaStack->getLuaState());
	//如果为 LUA_YIELD 则阻塞
	if (result == LUA_YIELD)
	{
		ret = true;
	}

	return ret;
}

bool LuaObject::intersectRect(const Rect& rect)
{
	//当为隐藏时，不存在碰撞
	if (!this->isVisible())
		return false;
	else if (m_bUsingBox)
		return m_boundingBox.intersectRect(rect);
	else
	{
		auto boundingBox = this->getBoundingBox();
		return boundingBox.intersectRect(rect);
	}
}
