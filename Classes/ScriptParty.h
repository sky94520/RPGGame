#ifndef __ScriptParty_H__
#define __ScriptParty_H__
#include "lua.hpp"

//------------------------------party----------------------------------
extern int open_party(lua_State* pL);
//金币
extern int addGold(lua_State* pL);
extern int subGold(lua_State* pL);
//物品
extern int addGood(lua_State* pL);
extern int subGood(lua_State* pL);
//减少装备
extern int subEquipment(lua_State* pL);
//获取装备
extern int getEquipment(lua_State* pL);
//获取物品个数
extern int getGoodNumber(lua_State* pL);
//获取名字
extern int getName(lua_State* pL);
//恢复血量
extern int recover(lua_State* pL);

#endif