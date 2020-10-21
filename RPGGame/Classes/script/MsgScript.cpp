#include "MsgScript.h"
#include "../GameScene.h"
#include "../layer/MessageLayer.h"
#include "../script/LuaStack.h"
#include "../manager/ScriptManager.h"

//------------------------------message------------------------------------
int open_message(lua_State* pL)
{
	const luaL_Reg lib[] = {
		{"showText", showText},
		{"showTip", showTip},
		{"showChoices", showChoices},
		{"setClickable",setClickable},
		{NULL, NULL}
	};
	luaL_newlib(pL, lib);
	return 1;
}

int showText(lua_State* pL)
{
	auto n = lua_gettop(pL);
	//获取游戏实例
	auto gameScene = GameScene::getInstance();
	auto msgLayer = gameScene->getMessageLayer();
	//获取参数
	string name = luaL_checkstring(pL, 1);
	string text = luaL_checkstring(pL, 2);
	TextPosition posType = static_cast<TextPosition>(luaL_checkinteger(pL, 3));
	bool run = (lua_toboolean(pL, 3) == 1);
	//持续时间
	float duration = 0.f;
	//有立绘
	if (n == 7)
	{
		string faceFilename = luaL_checkstring(pL, 5);
		int index = (int)luaL_checkinteger(pL, 6);
		FacePosition faceDir = static_cast<FacePosition>(luaL_checkinteger(pL, 7));
		//显示对话框
		duration = msgLayer->showText(name, text, posType, run, faceFilename, index, faceDir);
	}
	//无立绘
	else
	{
		duration = msgLayer->showText(name, text, posType, run);
	}
	auto scriptManager = GameScene::getInstance()->getScriptManager();
	//可点击,设置可点击事件
	if (msgLayer->isClickable())
	{
		scriptManager->setWaitType(WaitType::Click);
	}
	else//不可点击，设置等待事件
	{
		scriptManager->setWaitType(WaitType::Time);
		scriptManager->setDuration(duration);
	}
	scriptManager->getLuaStack()->yield(0);

	return 0;
}

int showTip(lua_State* pL)
{
	string text = luaL_checkstring(pL, 1);
	TextPosition textPos = static_cast<TextPosition>(luaL_checkinteger(pL, 2));
	float duration = (float)luaL_checknumber(pL, 3);

	GameScene::getInstance()->getMessageLayer()->showTip(text, textPos, duration);

	lua_pushnumber(pL, duration);

	return 1;
}

int showChoices(lua_State* pL)
{
	auto gameScene = GameScene::getInstance();
	auto msgLayer = gameScene->getMessageLayer();
	//获取栈顶table长度
	size_t len = lua_rawlen(pL, -1);

	for (unsigned i = 1; i <= len; i++)
	{
		//顺序获取
		lua_rawgeti(pL, -1, i);
		auto text = luaL_checkstring(pL, -1);
		msgLayer->addButton(text);

		lua_pop(pL, 1);
	}
	//排序
	msgLayer->alignItemsVertically();
	//设置等待类型为按钮，并等待
	auto manager = gameScene->getScriptManager();
	manager->setWaitType(WaitType::Button);

	manager->getLuaStack()->yield(0);

	return 0;
}

int setClickable(lua_State* pL)
{
	auto clickable = (lua_toboolean(pL, 1) == 1);
	GameScene::getInstance()->getMessageLayer()->setClickable(clickable);

	return 0;
}