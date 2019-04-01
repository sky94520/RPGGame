#ifndef __BattleLayer_H__
#define __BattleLayer_H__
#include <cstring>
#include "SDL_Engine/SDL_Engine.h"

using namespace std;
USING_NS_SDL;

class Fighter;
class HpBar;
class Actor;
class BattleDelegate;

enum class TurnType
{
	Player,//主角 我方可操控
	Friend,//友军 我方不可操控
	Enemy,//敌军
};
/*搜索类型*/
enum class SearchType
{
	HpMax,/*hp最大*/
	HpMin,/*hp最小*/
	Random,/*随机*/
};

struct Turn
{
	TurnType type;
	int speed;
	Fighter* fighter;
	HpBar* hpBar;
	LabelBMFont* label;
	//展示
	Sprite* showingSprite;
	LabelAtlas* atlas;
public:
	Turn();
	~Turn();
	//设置隐藏
	void setVisible(bool visibility);
	//设置位置
	void setPosition(const Point& pos);
	Point getPosition() const;
};

class BattleLayer : public Layer
{
private:
	//------------------队列相关--------------------------
	//出手循环队列TODO
	Turn* m_turnQueue[8];
	//队列总长度
	int m_nQueueLength;
	int m_nFront;
	int m_nRear;
	//我方个数
	int m_nOurNumber;
	//敌方个数
	int m_nEnemyNumber;
	//死亡后的turn
	vector<Turn*> m_deadTurnList;
	//当前正在初始化 主要用于脚本初始化
	Fighter* m_pCurInitialFighter;
	//是否回合结束
	bool m_bRoundOver;

	BattleDelegate* m_pDelegate;
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
	//弹出队顶的元素
	Turn* popTopTurn();
	//获取对应的turn
	Turn* getTurnByID(int uniqueID);
	Fighter* getFighterByID(int uniqueID);
	//条件搜索 仅区分是 Enemy or not
	Fighter* searchFighter(TurnType turnType, SearchType searchType);
	//获取点击到的turn
	Turn* getClickedTurn(const Point& pos);
	//获得当前正在执行的turn
	Turn* getTopTurn();
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
	//插入 交换排序
	void insertTurnOrderly(Turn* turn);
	void insertTurnOrderly(TurnType turnType,Fighter* fighter,LabelBMFont* label = nullptr,HpBar* hpBar = nullptr, LabelAtlas* atlas = nullptr);
	//fighter死亡回调函数
	void fighterDeadCallback(EventCustom* eventCustom);
};
#endif