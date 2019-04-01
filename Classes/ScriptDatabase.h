#ifndef __ScriptDatabase_H__
#define __ScriptDatabase_H__
#include <string>
#include "lua.hpp"
#include "SDL_Engine/SDL_Engine.h"

using namespace std;
USING_NS_SDL;
//------------------------------database----------------------------------
extern int open_database(lua_State* pL);
//获取数据
extern int getValueOfKey(lua_State* pL);
//设置数据
extern int setValueOfKey(lua_State* pL);
//移除
extern int removeKeyValue(lua_State* pL);
//是否存在任务
extern int isHaveQuest(lua_State* pL);
//添加任务
extern int addQuest(lua_State* pL);
//完成任务
extern int completeQuest(lua_State* pL);
//是否完成了此任务
extern int isCompletedQuest(lua_State* pL);

#endif