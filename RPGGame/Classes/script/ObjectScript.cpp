#include "ObjectScript.h"

#include "../GameScene.h"
#include "../layer/ScriptLayer.h"
#include "../entity/LuaObject.h"

int open_object(lua_State* pL)
{
	const luaL_Reg baselib[] = {
		{"create", create_object},
		{"delete", delete_object},
		{"setTrigger", set_trigger},
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
	auto scriptLayer = gameScene->getScriptLayer();

	LuaObject* luaObject = scriptLayer->addLuaObject(name, chartletName, layer, gameState);
	luaObject->setTilePosition(tileX, tileY);

	lua_pushboolean(pL, 1);
	return 1;
}

int delete_object(lua_State* pL)
{
	return 0;
}

int set_trigger(lua_State* pL)
{
	return 0;
}
