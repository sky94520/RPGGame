#ifndef __FollowController_H__
#define __FollowController_H__

#include "Controller.h"

class ShortestPathStep;
/*
 跟随控制器
 负责跟随其他控制器的行动，并带动ControllerListener
*/
class FollowController : public Controller
{
public:
	FollowController();
	~FollowController();

	CREATE_FUNC(FollowController);
	bool init();

	virtual float moveToward(const SDL_Point& tilePos);
	virtual void moveOneStep(ShortestPathStep* step);
private:
	void popStepAndAnimate();
private:
	//上一步
	ShortestPathStep* m_pLastStep;
	//当前行走步数
	ShortestPathStep* m_pCurStep;
	//待行走步
	ShortestPathStep* m_pPendingStep;
};
#endif