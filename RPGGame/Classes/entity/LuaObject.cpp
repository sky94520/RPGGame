#include "LuaObject.h"
#include "../GameMacros.h"

LuaObject::LuaObject()
	:m_triggerType(TriggerType::None)
	,m_nPriority(PRIORITY_SAME)
	,m_bObsolete(false)
{
}

LuaObject::~LuaObject()
{
}

LuaObject* LuaObject::create(const string& chartletName)
{
	LuaObject* luaObject = new LuaObject();

	if (luaObject != nullptr && luaObject->init(chartletName))
		luaObject->init(chartletName);
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
