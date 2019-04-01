#ifndef __ScriptFunc_H__
#define __ScriptFunc_H__
#include <string>
#include <vector>
#include <memory>
#include "lua.hpp"
#include "SDL_Engine/SDL_Engine.h"

using namespace std;
using namespace SDL;
using namespace SDL::ui;
//脚本加载器
extern int sdl_lua_loader(lua_State* pL);
//注册c/c++函数
extern void register_funcs(lua_State* pL);
//------------------------------base------------------------------------
//一些基础的函数及声明
extern int open_base(lua_State* pL);
//游戏状态
extern int getGameState(lua_State* pL);
extern int setGameState(lua_State* pL);
//改变中心点
extern int setViewpointCenter(lua_State* pL);
//设置视角跟随
extern int setViewpointFollow(lua_State* pL);
//------------------------------movement------------------------------------
extern int open_movement(lua_State* pL);
//切换地图
extern int changeMap(lua_State* pL);
//------------------------------screen------------------------------------
extern int open_screen(lua_State* pL);
//淡入场景
extern int fadeInScreen(lua_State* pL);
//淡出场景
extern int fadeOutScreen(lua_State* pL);
//------------------------------timer------------------------------------
extern int open_timer(lua_State* pL);
//脚本等待一段时间
extern int delay(lua_State* pL);
//------------------------------scene------------------------------------
extern int open_scene(lua_State* pL);
//商店处理
extern int shopProcessing(lua_State* pL);
#endif