#ifndef __BattleLayer_H__
#define __BattleLayer_H__
#include <cstring>
#include "SDL_Engine/SDL_Engine.h"

using namespace std;
USING_NS_SDL;

class Turn;
class HpBar;
class Actor;
class Fighter;
class BattleDelegate;
class PriorityQueue;

class BattleLayer : public Layer
{
public:
	static const string BATTLE_FIGHTER_DEAD_EVENT;
public:
	BattleLayer();
	~BattleLayer();
	CREATE_FUNC(BattleLayer);
	bool init();

	void update(float dt);
	//添加我方人员
	void addOur(Actor* actor);
	//添加敌人
	void addEnemy(const string& name, int number);
	Fighter* getFighterByID(int uniqueID);
	//获得当前正在执行的turn
	Turn* getTopTurn();
	//获取点击到的turn
	Turn* getClickedTurn(const Point& pos);
	//回合结束
	void roundOver();
	//设置回合
	void setRoundOver(bool var);
	//战斗结束
	void endBattle();
	//清除函数
	void clear();
	//设置委托
	void setDelegate(BattleDelegate* pDelegate);
private:
	//当前正在初始化 主要用于脚本初始化
	Fighter* m_pCurInitialFighter;
	//是否回合结束
	bool m_bRoundOver;
	PriorityQueue* m_pPriorityQueue;
	BattleDelegate* m_pDelegate;
};
#endif