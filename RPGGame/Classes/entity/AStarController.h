#ifndef __AStarController_H__
#define __AStarController_H__

#include "Controller.h"

enum class Direction;
class ShortestPathStep;
class AStar;
class LuaObject;

/*A星算法控制器，通过A星算法获取路径后进行寻路，并带动ControllerListener*/
class AStarController : public Controller
{
public:
	AStarController();
	~AStarController();

	static AStarController* create(AStar* pAStar);
	bool init(AStar* pAStar);

	void setTriggerObject(LuaObject* luaObject);

	virtual float moveToward(const SDL_Point& tilePos);
public:
	static Direction getDirection(const SDL_Point& delta);
private:
	void popStepAndAnimate();
	float constructPathAndStartAnimation(ShortestPathStep* pHead);
	void clearShortestPath();
public:
	static const string CHARACTER_MOVE_TO_TILE;
private:
	//保存了当前的行走路径
	vector<ShortestPathStep*> m_paths;
	//当前运动到了哪一步
	unsigned int m_stepIndex;

	//上一步
	ShortestPathStep* m_pLastStep;
	//是否存在待运动点
	SDL_Point m_pendingMove;
	bool m_bPendingMove;
	//A*算法
	AStar* m_pAStar;
	//触发NPC
	LuaObject* m_pTriggerObject;
};
#endif