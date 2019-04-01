#include "ScriptDatabase.h"
#include "DynamicData.h"
//------------------------------database----------------------------------
int open_database(lua_State* pL)
{
	const luaL_Reg lib[] = {
		{"getValueOfKey", getValueOfKey},
		{"setValueOfKey", setValueOfKey},
		{"removeKeyValue", removeKeyValue},
		{"isHaveQuest", isHaveQuest},
		{"addQuest", addQuest},
		{"completeQuest", completeQuest},
		{"isCompletedQuest", isCompletedQuest},
		{NULL, NULL}
	};
	luaL_newlib(pL, lib);
	return 1;
}

int getValueOfKey(lua_State* pL)
{
	string key = luaL_checkstring(pL, 1);
	
	Value* value = DynamicData::getInstance()->getValueOfKey(key);
	//未找到
	if (value == nullptr)
	{
		return 0;
	}
	//放入
	auto type = value->getType();
	switch (type)
	{
	case SDL::Value::Type::BYTE:
	case SDL::Value::Type::INTEGER:
		lua_pushinteger(pL, value->asInt());
		break;
	case SDL::Value::Type::FLOAT:
	case SDL::Value::Type::DOUBLE:
		lua_pushnumber(pL, value->asFloat());
		break;
	case SDL::Value::Type::BOOLEAN:
		lua_pushboolean(pL, value->asBool());
		break;
	case SDL::Value::Type::STRING:
		lua_pushstring(pL, value->asString().c_str());
		break;
	default:
		break;
	}
	return 1;
}

int setValueOfKey(lua_State* pL)
{
	string key = luaL_checkstring(pL, 1);
	Value::Type type = static_cast<Value::Type>(luaL_checkinteger(pL, 2));
	Value value;

	switch (type)
	{
	case SDL::Value::Type::BYTE:
	case SDL::Value::Type::INTEGER:
		value = (int)luaL_checkinteger(pL, 3);
		break;
	case SDL::Value::Type::FLOAT:
	case SDL::Value::Type::DOUBLE:
		value = Value(luaL_checknumber(pL, 3));
		break;
	case SDL::Value::Type::BOOLEAN:
		{
			value = Value(lua_toboolean(pL, 3) != 0);
		}
		break;
	case SDL::Value::Type::STRING:
		value = Value(luaL_checkstring(pL, 3));
		break;
	default:
		break;
	}
	DynamicData::getInstance()->setValueOfKey(key, value);

	return 0;
}

int removeKeyValue(lua_State* pL)
{
	auto key = luaL_checkstring(pL, 1);
	bool ret = DynamicData::getInstance()->removeKeyValue(key);

	lua_pushboolean(pL, ret);
	return 1;
}

int isHaveQuest(lua_State* pL)
{
	auto taskName = luaL_checkstring(pL, 1);
	bool ret = DynamicData::getInstance()->isHaveQuest(taskName);
	
	lua_pushboolean(pL, ret);
	return 1;
}

int addQuest(lua_State* pL)
{
	auto taskName = luaL_checkstring(pL, 1);
	bool ret = DynamicData::getInstance()->addQuest(taskName);
	//只有添加成功才进行初始化
	if (ret)
	{
		//主动调用该任务的初始化函数
		lua_getglobal(pL, taskName);
		lua_pushstring(pL, "initialize");
		lua_gettable(pL, -2);
		lua_remove(pL, -2);
		//放入参数
		lua_getglobal(pL, taskName);
		//执行函数
		lua_pcall(pL, 1, 0, 0);
	}

	lua_pushboolean(pL, ret);
	return 1;
}

int completeQuest(lua_State* pL)
{
	auto taskName = luaL_checkstring(pL, 1);
	bool ret = DynamicData::getInstance()->completeQuest(taskName);

	lua_pushboolean(pL, ret);
	return 1;
}

int isCompletedQuest(lua_State* pL)
{
	auto taskName = luaL_checkstring(pL, 1);
	bool ret = DynamicData::getInstance()->isCompletedQuest(taskName);
		
	lua_pushboolean(pL, ret);
	return 1;
}