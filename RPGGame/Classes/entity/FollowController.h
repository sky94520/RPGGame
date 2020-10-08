#ifndef __FollowController_H__
#define __FollowController_H__

#include "Controller.h"

class PathStep;
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
	virtual void moveOneStep(PathStep* step);
private:
	void popStepAndAnimate();
private:
	//上一步
	PathStep* m_pLastStep;
	//当前行走步数
	PathStep* m_pCurStep;
	//待行走步
	PathStep* m_pPendingStep;
};
#endif