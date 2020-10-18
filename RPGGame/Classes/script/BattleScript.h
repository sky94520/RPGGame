#ifndef __BattleScript_H__
#define __BattleScript_H__

#include "lua.hpp"
using namespace std;

extern int open_battle(lua_State* pL);
//指定战斗背景
extern int specifyBattleback(lua_State* pL);
//开始战斗
extern int startBattle(lua_State* pL);
//展示敌人动画
extern int showEnemyAnimation(lua_State* pL);
//展示战斗动画
extern int showBattleAnimation(lua_State* pL);
//展示子弹动画
extern int showBullet(lua_State* pL);
//受伤
extern int hurt(lua_State* pL);
//治愈
extern int heal(lua_State* pL);
//搜索Fighter并返回对应ID 若不存在，不返回
extern int searchFighter(lua_State* pL);
//回合结束
extern int roundOver(lua_State* pL);
//添加奖励
extern int addReward(lua_State* pL);
//获取角色属性
extern int getProperty(lua_State* pL);
//设置角色属性
extern int setProperty(lua_State* pL);
//获取命中率
extern int getHitRate(lua_State* pL);
//获取躲避率
extern int getEvasionRate(lua_State* pL);
//获取暴击率
extern int getCriticalRate(lua_State* pL);
//攻击倍率
extern int getAttackScale(lua_State* pL);
extern int setAttackScale(lua_State* pL);
//防御倍率
extern int getDefenseScale(lua_State* pL);
extern int setDefenseScale(lua_State* pL);
//修改状态
extern int changeFightState(lua_State* pL);
//翻转设置
extern int setFlipX(lua_State* pL);
extern int isFlipX(lua_State* pL);
//移动至目标
extern int moveToTarget(lua_State* pL);
//返回
extern int backTo(lua_State* pL);
//贴图
extern int setChartletName(lua_State* pL);
extern int getChartletName(lua_State* pL);

#endif