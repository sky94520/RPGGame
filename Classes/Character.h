#ifndef __Character_H__
#define __Character_H__
#include <string>
#include "Entity.h"
using namespace std;

enum class State
{
	None,
	Idle,
	Walking,
};
enum class Direction;
class ShortestPathStep;
class NonPlayerCharacter;

class Character : public Entity
{
	SDL_SYNTHESIZE_READONLY(string, m_chartletName, ChartletName);//当前贴图名，也可以认为是人物名称，唯一
	SDL_SYNTHESIZE(float, m_durationPerGrid, DurationPerGrid);//每格的行走时间
protected:
	bool m_bDirty;
private:
	Direction m_dir;
	State m_state;
	Character* m_pFollowCharacter;
	//运动相关
	vector<ShortestPathStep*> m_shortestPath;
	unsigned int m_nStepIndex;

	ShortestPathStep* m_lastStep;
	Point m_pendingMove;
	bool m_bHavePendingMove;
	bool m_bMoving;

	NonPlayerCharacter* m_pTriggerNPC;
public:
	Character();
	~Character();
	static Character* create(const string& chartletName);
	static Character* create(const string& chartletName, Direction direction);

	bool init(const string& chartletName);
	bool init(const string& chartletName, Direction direction);

	void clear();
	//跟随某角色
	void follow(Character* character);
	//站立
	void sit();
	//设置npc
	void setTriggerNPC(NonPlayerCharacter* npc);
	//方向改变
	Direction getDirection() const;
	void setDirection(Direction direction);

	bool isMoving() const { return m_bMoving; }
	//运动 默认tilePos必能通过，由上层筛选
	float moveToward(const Point& tilePos);
	//移动一步
	bool moveOneStep(ShortestPathStep* step);
	void popStepAndAnimate();
private:
	//切换状态
	void changeState(State state);
	//构造路径并运行动画 并返回总持续时间
	float constructPathAndStartAnimation(ShortestPathStep* pHead);
	//清除行走路径
	void clearShortestPath();
	Direction getDirection(const Point& delta) const;
};
#endif