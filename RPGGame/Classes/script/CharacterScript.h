#ifndef __ScriptCharacter_H__
#define __ScriptCharacter_H__
#include "lua.hpp"

extern int open_character(lua_State* pL);
//展示动画
extern int showAnimation(lua_State* pL);
//获取方向
extern int getDirection(lua_State* pL);
//设置方向,并返回原来的方向
extern int setDirection(lua_State* pL);
//显示气泡
extern int showBalloonIcon(lua_State* pL);
//移动
extern int moveToward(lua_State* pL);
//是否显示
extern int setVisible(lua_State* pL);
//设置行走速度
extern int setDurationPerGrid(lua_State* pL);
//获取所在图块位置
extern int getTilePosition(lua_State* pL);
//获取所在位置
extern int getPosition(lua_State* pL);

#endif