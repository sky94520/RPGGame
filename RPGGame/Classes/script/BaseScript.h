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

//------------------------------base------------------------------------
/*
 * 一些基础的函数及声明
 * 大部分函数都需要转为在!GameState::Normal下执行
 * 大部分函数需要配合协程使用，以实现剧本的逐步解析，而不是一次性解析
*/
extern int open_base(lua_State* pL);
//游戏状态
/*
 * 获取当前的游戏状态
 * @return 返回游戏状态
*/
extern int getGameState(lua_State* pL);

/*
 * 设置当前的游戏状态
 * @param gameState 游戏状态
 * @return
*/
extern int setGameState(lua_State* pL);

/*
 * 改变中心点 tilePos
 * @param x x坐标 int
 * @param y y坐标 int
 * @param duration 持续时间 以毫秒为单位
 * @return
*/
extern int setViewpointCenter(lua_State* pL);

/*
 * 设置视角跟随TODO
 * @param x x坐标
 * @param y y坐标
 * @param duration 持续时间
 * @return
*/
extern int setViewpointFollow(lua_State* pL);
//------------------------------movement------------------------------------
extern int open_movement(lua_State* pL);

/*
 * 切换地图
 * @param mapName 地图名称
 * @param tileX 横轴图块坐标
 * @param tileY 纵轴图块坐标
 * @return
*/
extern int changeMap(lua_State* pL);
//------------------------------screen------------------------------------
extern int open_screen(lua_State* pL);

/*
 * 淡入场景
 * @param duration 持续时间
 * @return
*/
extern int fadeInScreen(lua_State* pL);

/*
 * 淡出场景
 * @param duration 持续时间
 * @return
*/
extern int fadeOutScreen(lua_State* pL);
//------------------------------timer------------------------------------
extern int open_timer(lua_State* pL);

/*
 * 脚本等待一段时间
 * @param duration 持续时间
 * @return
*/
extern int delay(lua_State* pL);
#endif