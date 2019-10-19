#include "ObjectScript.h"

#include "../GameScene.h"
#include "../manager/ScriptManager.h"
#include "../entity/LuaObject.h"
#include "../data/StaticData.h"
#include "../data/CharacterData.h"

int open_object(lua_State* pL)
{
	const luaL_Reg baselib[] = {
		{"create", create_object},
		{"delete", delete_object},
		{"setTrigger", set_trigger},
		{"setPriority", set_priority},
		{"showWalkingAnimation", show_walking_animation},
		{NULL, NULL}
	};
	luaL_newlib(pL, baselib);
	return 1;
}

int create_object(lua_State* pL)
{
	string name = luaL_checkstring(pL, 1);
	string chartletName = luaL_checkstring(pL, 2);
	int tileX = (int)luaL_checkinteger(pL, 3);
	int tileY = (int)luaL_checkinteger(pL, 4);

	//创建脚本对象
	auto gameScene = GameScene::getInstance();
	auto gameState = gameScene->getGameState();
	auto layer = gameScene->getCollisionLayer();
	auto scriptLayer = gameScene->getScriptManager();

	LuaObject* luaObject = scriptLayer->addLuaObject(name, chartletName, layer, gameState);
	luaObject->setTilePosition(tileX, tileY);

	lua_pushboolean(pL, 1);
	return 1;
}

int delete_object(lua_State* pL)
{
	string name = luaL_checkstring(pL, 1);

	auto gameScene = GameScene::getInstance();
	auto scriptLayer = gameScene->getScriptManager();
	//尝试删除脚本对象
	bool ret = scriptLayer->removeLuaObject(name);

	lua_pushboolean(pL, int(ret));
	return 1;
}

int set_trigger(lua_State* pL)
{
	string name = luaL_checkstring(pL, 1);
	TriggerType triggerType = static_cast<TriggerType>(luaL_checkinteger(pL, 2));
	string funcName = luaL_checkstring(pL, 3);
	//获取对应的脚本对象
	LuaObject* luaObject = GameScene::getInstance()->getScriptManager()->getLuaObject(name);
	if (luaObject == nullptr)
	{
		printf("set_trigger: not found the object: %s\n", name.c_str());
		return 0;
	}
	luaObject->setTriggerType(triggerType);
	luaObject->setFuncName(funcName);

	return 0;
}

int set_priority(lua_State* pL)
{
	string name = luaL_checkstring(pL, 1);
	int priority = static_cast<int>(luaL_checkinteger(pL, 2));

	//获取对应的脚本对象
	LuaObject* luaObject = GameScene::getInstance()->getScriptManager()->getLuaObject(name);
	if (luaObject == nullptr)
	{
		printf("set_priority: not found the object: %s\n", name.c_str());
		return 0;
	}
	//设置优先级，就是设置的localZOrder
	luaObject->setPriority(priority);
	luaObject->setLocalZOrder(priority);
	return 0;
}

int show_walking_animation(lua_State* pL)
{
	string name = luaL_checkstring(pL, 1);
	Direction direction = static_cast<Direction>(luaL_checkinteger(pL, 2));

	//获取对应的脚本对象
	auto gameScene = GameScene::getInstance();
	LuaObject* luaObject = gameScene->getScriptManager()->getLuaObject(name);
	if (luaObject == nullptr)
	{
		printf("show_walking_animation: not found the object: %s\n", name.c_str());
		return 0;
	}
	//获取动画并运行
	auto chartletName = luaObject->getChartletName();
	auto animation = StaticData::getInstance()->getCharacterData()->getWalkingAnimation(chartletName, direction);
	auto animate = Animate::create(animation);
	animate->setTag(Entity::ANIMATION_TAG);

	luaObject->getSprite()->stopActionByTag(Entity::ANIMATION_TAG);
	luaObject->getSprite()->runAction(animate);

	lua_Number duration = (lua_Number)animation->getDuration();
	lua_pushnumber(pL, duration);
	return 1;
}
