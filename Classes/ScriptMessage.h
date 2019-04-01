#ifndef __ScriptMessage_H__
#define __ScriptMessage_H__
#include "lua.hpp"

//------------------------------message------------------------------------
extern int open_message(lua_State* pL);
//显示文本
extern int showText(lua_State* pL);
//显示提示文本
extern int showTip(lua_State* pL);
//展示选择
extern int showChoices(lua_State* pL);
//设置是否可点击,配合showText搭配使用
extern int setClickable(lua_State* pL);

#endif