#ifndef __Controller_H__
#define __Controller_H__

#include <vector>

#include "SDL_Engine/SDL_Engine.h"

USING_NS_SDL;
using namespace std;

class ShortestPathStep;
class ControllerListener;

class Controller: public Object
{
public:
	Controller();
	virtual ~Controller();
	//设置监听对象
	void setControllerListener(ControllerListener* pListener);
	virtual float moveToward(const SDL_Point& tilePos) = 0;
	virtual void moveOneStep(ShortestPathStep* step){}

	float getDurationPerGrid() const 
	{ 
		return m_durationPerGrid; 
	}
	void setDurationPerGrid(float duration)
	{
		m_durationPerGrid = duration;
	}
	void setFollower(Controller* pController);
protected:
	ControllerListener* m_pListener;
	float m_durationPerGrid;
	Controller* m_pFollowController;
};
#endif