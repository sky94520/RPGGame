#ifndef __BattleLayer_H__
#define __BattleLayer_H__
#include <cstring>
#include "SDL_Engine/SDL_Engine.h"

using namespace std;
USING_NS_SDL;

class HpBar;
class Actor;
struct Turn;
class Fighter;
class BattleDelegate;
class PriorityQueue;
enum class TurnType;
enum class SearchType;

class BattleLayer : public Layer
{
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
	Turn* getTurnByID(int uniqueID);
	Fighter* searchFighter(TurnType turnType, SearchType searchType);
	//回合结束
	void roundOver();
	//战斗结束 返回死了的友方的名字
	vector<string> endBattle();
	void fighterDead(Fighter* fighter);
	int getOurNumber() const;
	int getEnemyNumber() const;
	//设置战斗背景
	void setBattleBack(const string& back, int index);
	//清除函数
	void clear();
private:
	//当前正在初始化 主要用于脚本初始化
	Fighter* m_pCurInitialFighter;
	//是否回合结束
	bool m_bRoundOver;
	PriorityQueue* m_pPriorityQueue;
	Sprite* m_battleBacks[2];//战斗背景
};
#endif