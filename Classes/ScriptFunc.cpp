#include "ScriptFunc.h"
#include "GameScene.h"
#include "EffectLayer.h"
#include "ScriptLayer.h"
#include "Character.h"
#include "StaticData.h"
#include "GoodLayer.h"
#include "Good.h"

#include "ScriptBattle.h"
#include "ScriptCharacter.h"
#include "ScriptMessage.h"
#include "ScriptSound.h"
#include "ScriptDatabase.h"
#include "ScriptParty.h"

int sdl_lua_loader(lua_State* pL)
{
	//后缀为 luac 和lua
	static const std::string BYTECODE_FILE_EXT = ".luac";
	static const std::string NOT_BYTECODE_FILE_EXT = ".lua";
	//取出文件名
	std::string filename(luaL_checkstring(pL, 1));
	//仅保留文件名
	auto begin = filename.find_last_of('/');
	auto end = filename.find_last_of('.');

	auto name = filename.substr(begin + 1, end - begin - 1);
	//去掉扩展名，如果有的话
	size_t pos = filename.rfind(BYTECODE_FILE_EXT);
	if (pos != std::string::npos)
	{
		filename = filename.substr(0, pos);
	}
	else
	{
		pos = filename.rfind(NOT_BYTECODE_FILE_EXT);

		if (pos == filename.length() - NOT_BYTECODE_FILE_EXT.size())
		{
			filename = filename.substr(0, pos);
		}
	}
	//将所有的.替换成/
	pos = filename.find_first_of(".");
	while (pos != std::string::npos)
	{
		filename.replace(pos, 1, "/");
		pos = filename.find_first_of(".", pos + 1);
	}
	//先在package.path中搜索脚本
	FileUtils* utils = FileUtils::getInstance();
	std::unique_ptr<char> chunk = nullptr;
	size_t chunkSize = 0;
	string chunkName;
	//获取package.path
	lua_getglobal(pL, "package");
	lua_getfield(pL, -1, "path");

	std::string searchpath(luaL_checkstring(pL, -1));
	lua_pop(pL, 1);

	begin = 0;
	size_t next = searchpath.find_first_of(";", 0);
	//遍历path中的所有路径，结合文件名进行组装
	do
	{
		if (next == std::string::npos)
			next = searchpath.length();
		std::string prefix = searchpath.substr(begin, next);
		if (prefix[0] == '.' && prefix[1] == '/')
		{
			prefix = prefix.substr(2);
		}
		pos = prefix.find("?.lua");
		chunkName = prefix.substr(0, pos) + filename + NOT_BYTECODE_FILE_EXT;

		if (utils->isFileExist(chunkName))
		{
			chunk =  std::move(utils->getUniqueDataFromFile(chunkName, &chunkSize));
			break;
		}
		else
		{
			chunkName = prefix.substr(0, pos) + filename + BYTECODE_FILE_EXT;
			if (utils->isFileExist(chunkName))
			{
				chunk =  std::move(utils->getUniqueDataFromFile(chunkName, &chunkSize));
				break;
			}
		}
		begin = next + 1;
		next = searchpath.find_first_of(";", begin);
	}while (begin < (int)searchpath.length());
	//可在此处进行解密
	if (chunk != nullptr)
	{
		const char* buffer = chunk.get();
		luaL_loadbuffer(pL, buffer, chunkSize, chunkName.c_str());

		chunk.reset();

		lua_pushstring(pL, name.c_str());

		return 2;
	}
	lua_pushstring(pL, "can not get file data");
	return 1;
}

void register_funcs(lua_State* pL)
{
	static const luaL_Reg cpplibs[] = {
		{"base", open_base},
		{"movement", open_movement},
		{"screen", open_screen},
		{"timer", open_timer},
		{"character", open_character},
		{"message", open_message},
		{"scene", open_scene},
		{"battle", open_battle},
		{"sound", open_sound},
		{"database", open_database},
		{"party", open_party},
		{NULL, NULL}
	};
	const luaL_Reg* lib = nullptr;
	for (lib = cpplibs;lib->func;lib++)
	{
		luaL_requiref(pL, lib->name, lib->func, 1);
		lua_pop(pL, 1);
	}
}
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

	GameScene::getInstance()->setViewpointCenter(Point(x, y), duration);
	return 0;
}

int setViewpointFollow(lua_State* pL)
{
	int id = (int)luaL_checkinteger(pL, 1);
	auto gameScene = GameScene::getInstance();

	auto character = gameScene->getCharacterByID(id);

	if (character != nullptr)
	{
		gameScene->setViewpointFollow(character);
	}
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
	scriptLayer->setWaitTime(duration);

	gameScene->getEffectLayer()->fadeInScreen(duration);
	gameScene->yield(0);

	return 0;
}

int fadeOutScreen(lua_State* pL)
{
	float duration = (float)luaL_checknumber(pL, 1);

	auto gameScene = GameScene::getInstance();
	auto scriptLayer = gameScene->getScriptLayer();
	//设置等待时间
	scriptLayer->setWaitType(WaitType::Time);
	scriptLayer->setWaitTime(duration);

	gameScene->getEffectLayer()->fadeOutScreen(duration);
	gameScene->yield(0);

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
	scriptLayer->setWaitTime(duration);

	gameScene->yield(0);

	return 0;
}
//------------------------------scene------------------------------------
int open_scene(lua_State* pL)
{
	const luaL_Reg lib[] = {
		{"shopProcessing", shopProcessing},
		{NULL, NULL}
	};
	luaL_newlib(pL, lib);
	return 1;
}

int shopProcessing(lua_State* pL)
{
	auto gameScene = GameScene::getInstance();
	auto goodLayer = gameScene->getGoodLayer();
	auto dynamicData = DynamicData::getInstance();
	//栈顶为table,表示购买
	if (lua_gettop(pL) == 1 && lua_istable(pL, -1))
	{
		dynamicData->clearShopGoods();
		//获取栈顶table长度
		size_t len = lua_rawlen(pL, -1);
		//遍历添加物品
		for (unsigned i = 1; i <= len; i++)
		{
			//顺序获取
			lua_rawgeti(pL, -1, i);
			auto goodName = luaL_checkstring(pL, -1);
			//TODO:默认物品为10个
			dynamicData->addShopGood(goodName, 10);

			lua_pop(pL, 1);
		}
		//打开背包
		goodLayer->show(GoodLayer::Type::ShopBuy);
	}
	else//出售
	{
		goodLayer->show(GoodLayer::Type::ShopSale);
	}
	gameScene->getScriptLayer()->setWaitType(WaitType::Button);
	//音效
	SoundManager::getInstance()->playEffect(STATIC_DATA_STRING("open_shop_se"), 0);
	gameScene->yield(0);

	return 0;
}
