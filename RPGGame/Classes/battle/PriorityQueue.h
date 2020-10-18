#ifndef __PriorityQueue_H__
#define __PriorityQueue_H__

#include <cstring>
#include <SDL_Engine/SDL_Engine.h>

using namespace std;
using namespace SDL;

class Fighter;
class HpBar;

/*搜索类型*/
enum class SearchType
{
	HpMax,/*hp最大*/
	HpMin,/*hp最小*/
	Random,/*随机*/
};

enum class TurnType
{
	Player,//主角 我方可操控
	Friend,//友军 我方不可操控
	Enemy,//敌军
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

class PriorityQueue: public Node
{
public:
	PriorityQueue();
	~PriorityQueue();
	CREATE_FUNC(PriorityQueue);
	bool init();
	Turn* getTop();
	//弹出队顶的元素
	Turn* popTopTurn();
	//获取对应的turn
	Turn* getTurnByID(int uniqueID);
	//条件搜索 仅区分是 Enemy or not
	Fighter* searchFighter(TurnType turnType, SearchType searchType);
	//获取点击到的turn
	Turn* getClickedTurn(const Point& pos);
	//回合结束
	void roundOver();
	//战斗结束
	void endBattle();
	void clear();

	int getOurNumber() const { return m_nOurNumber; }
	int getEnemyNumber() const { return m_nEnemyNumber; }
	//插入 交换排序
	void insertTurnOrderly(Turn* turn);
	void insertTurnOrderly(TurnType turnType, Fighter* fighter, LabelBMFont* label=nullptr, HpBar* hpBar=nullptr, LabelAtlas* atlas=nullptr);
	//fighter死亡回调函数
	void fighterDeadCallback(EventCustom* eventCustom);
private:
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
};

#endif