#ifndef __ObjectScript_H__
#define __ObjectScript_H__

#include <string>

#include "lua.hpp"

using namespace std;

/*
 * 一些基础的函数及声明
*/
extern int open_object(lua_State* pL);
/*
 * 创建脚本对象
 * @param name 脚本对象的名称 唯一，可以通过这个name操作该脚本对象
 * @param chartlet 动画名称
 * @param tileX 所在的tileX位置
 * @param tileY 所在的tileY位置
 * @return 是否创建成功
*/
extern int create_object(lua_State* pL);
/*
 * 删除对象
 * @param name 要删除的脚本对象名称
 * @return 是否创建成功
*/
extern int delete_object(lua_State* pL);
/*
 * 设置脚本对象的触发方式,以及触发后的回调函数
 * @param name 脚本对象名称
 * @param type 触发类型 int=>TriggerType
 * @param funcName lua函数名
*/
extern int set_trigger(lua_State* pL);

/*
 * 设置优先级
 * @param name 脚本对象名称
 * @param type 优先级 int
*/
extern int set_priority(lua_State* pL);

/*
 * 脚本对象显示行走动画
 * @param name 脚本对象名称
 * @param direction 方向
*/
extern int show_walking_animation(lua_State* pL);

#endif