#include "BaseScript.h"
#include "../GameScene.h"
#include "../GameMacros.h"

#include "../layer/MapLayer.h"
#include "../layer/ScriptLayer.h"
#include "../layer/EffectLayer.h"

#include "../script/LuaStack.h"

//------------------------------base------------------------------------
int open_base(lua_State* pL)
{
	const luaL_Reg baselib[] = {
		{"getGameState", getGameState},
		{"setGameState", setGameState},
		{"setViewpointCenter", setViewpointCenter},
		{"setViewpointFollow", setViewpointFollow},
		{NULL, NULL}
	};
	luaL_newlib(pL, baselib);
	return 1;
}

int getGameState(lua_State* pL)
{
	auto state = GameScene::getInstance()->getGameState();
	lua_Integer nState = static_cast<lua_Integer>(state);

	lua_pushinteger(pL, nState);

	return 1;
}

int setGameState(lua_State* pL)
{
	lua_Integer nState = luaL_checkinteger(pL, 1);
	GameState state = static_cast<GameState>(nState);

	GameScene::getInstance()->setGameState(state);

	return 0;
}

int setViewpointCenter(lua_State* pL)
{
	float x = (float)luaL_checknumber(pL, 1);
	float y = (float)luaL_checknumber(pL, 2);
	float duration = (float)luaL_checknumber(pL, 3);
	
	MapLayer* pMapLayer = GameScene::getInstance()->getMapLayer();

	pMapLayer->setViewpointCenter(Point(x, y), duration);
	return 0;
}

int setViewpointFollow(lua_State* pL)
{
	//以name在ScriptLayer和PlayerLayer里寻找
	/*
	int id = (int)luaL_checkinteger(pL, 1);
	auto gameScene = GameScene::getInstance();

	auto character = gameScene->getCharacterByID(id);

	if (character != nullptr)
	{
		gameScene->setViewpointFollow(character);
	}
	*/
	return 0;
}

//------------------------------movement------------------------------------
int open_movement(lua_State* pL)
{
	const luaL_Reg movementlib[] = {
		{"changeMap", changeMap},
		{NULL, NULL}
	};
	luaL_newlib(pL, movementlib);

	return 1;
}

int changeMap(lua_State* pL)
{
	const char* mapName = luaL_checkstring(pL, 1);
	float x = (float)luaL_checknumber(pL, 2);
	float y = (float)luaL_checknumber(pL, 3);

	GameScene::getInstance()->changeMap(mapName, Point(x, y));

	return 0;
}

//------------------------------screen------------------------------------
int open_screen(lua_State* pL)
{
	const luaL_Reg screenlib[] = {
		{"fadeInScreen", fadeInScreen},
		{"fadeOutScreen", fadeOutScreen},
		{NULL, NULL}
	};
	luaL_newlib(pL, screenlib);

	return 1;
}

int fadeInScreen(lua_State* pL)
{
	float duration = (float)luaL_checknumber(pL, 1);

	auto gameScene = GameScene::getInstance();
	auto scriptLayer = gameScene->getScriptLayer();
	//设置等待时间
	scriptLayer->setWaitType(WaitType::Time);
	scriptLayer->setDuration(duration);

	gameScene->getEffectLayer()->fadeInScreen(duration);
	scriptLayer->getLuaStack()->yield(0);

	return 0;
}

int fadeOutScreen(lua_State* pL)
{
	float duration = (float)luaL_checknumber(pL, 1);

	auto gameScene = GameScene::getInstance();
	auto scriptLayer = gameScene->getScriptLayer();
	//设置等待时间
	scriptLayer->setWaitType(WaitType::Time);
	scriptLayer->setDuration(duration);

	gameScene->getEffectLayer()->fadeOutScreen(duration);
	scriptLayer->getLuaStack()->yield(0);

	return 0;
}

//------------------------------screen------------------------------------
int open_timer(lua_State* pL)
{
	const luaL_Reg timerlib[] = {
		{"delay", delay},
		{NULL, NULL}
	};
	luaL_newlib(pL, timerlib);

	return 1;
}

int delay(lua_State* pL)
{
	float duration = (float)luaL_checknumber(pL, 1);

	auto gameScene = GameScene::getInstance();
	auto scriptLayer = gameScene->getScriptLayer();
	//设置等待时间
	scriptLayer->setWaitType(WaitType::Time);
	scriptLayer->setDuration(duration);
	scriptLayer->getLuaStack()->yield(0);

	return 0;
}
