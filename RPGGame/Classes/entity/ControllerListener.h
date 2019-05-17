#ifndef __ControllerListener_H__
#define __ControllerListener_H__

#include "SDL_Engine/SDL_Engine.h"
USING_NS_SDL;

enum class State;
enum class Direction;

class ControllerListener
{
public:
	//获取当前所在的图块位置
	virtual SDL_Point getTilePosition() const = 0;
	//切换状态
	virtual void changeState(State state) = 0;
	//移动
	virtual void moveTo(const SDL_Point& toTile, float duration, CallFunc* endCallback) = 0;

	virtual Direction getDirection() const = 0;
	virtual void setDirection(Direction dir) = 0;
	
	virtual bool isMoving() const = 0;
	virtual void setMoving(bool moving) = 0;
};
#endif