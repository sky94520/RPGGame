#ifndef __ScriptSound_H__
#define __ScriptSound_H__
#include "lua.hpp"
#include "SDL_Engine/SDL_Engine.h"
USING_NS_SDL;
//------------------------------sound------------------------------------
extern int open_sound(lua_State* pL);
//播放背景音乐
extern int playBGM(lua_State* pL);
//停止背景音乐
extern int stopBGM(lua_State* pL);
//设置背景音乐音量
extern int setBGMVolume(lua_State* pL);
//播放音效
extern int playEffect(lua_State* pL);
//设置音效音量
extern int setEffectVolume(lua_State* pL);

#endif