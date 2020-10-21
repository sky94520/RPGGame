#include "SoundScript.h"

int open_sound(lua_State* pL)
{
	const luaL_Reg lib[] = {
		{"playBGM", playBGM},
		{"stopBGM", stopBGM},
		{"setBGMVolume", setBGMVolume},
		{"playEffect", playEffect},
		{"setEffectVolume", setEffectVolume},
		{NULL, NULL}
	};
	luaL_newlib(pL, lib);
	return 1;
}

int playBGM(lua_State* pL)
{
	auto file = luaL_checkstring(pL, 1);
	int loops = (int)luaL_checkinteger(pL, 2);

	int ret = SoundManager::getInstance()->playBackgroundMusic(file, loops);
	//是否播放成功
	lua_pushboolean(pL, ret != -1);

	return 1;
}

int stopBGM(lua_State* pL)
{
	SoundManager::getInstance()->stopBackgroundMusic();

	return 0;
}

int setBGMVolume(lua_State* pL)
{
	int volume = (int)luaL_checkinteger(pL, 1);
	SoundManager::getInstance()->setBackgroundMusicVolume(volume);

	return 0;
}

int playEffect(lua_State* pL)
{
	auto file = luaL_checkstring(pL, 1);
	int loops = (int)luaL_checkinteger(pL, 2);

	int ret = SoundManager::getInstance()->playEffect(file, loops);
	lua_pushboolean(pL, ret != -1);

	return 1;
}

int setEffectVolume(lua_State* pL)
{
	int volume = (int)luaL_checkinteger(pL, 1);
	SoundManager::getInstance()->setEffectVolume(volume);

	return 0;
}
