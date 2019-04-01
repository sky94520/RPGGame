#ifndef __EventLayer_H__
#define __EventLayer_H__
#include<cmath>
#include "SDL_Engine/SDL_Engine.h"

using namespace std;
using namespace SDL;

enum class GameState;
enum class Direction;

class EventLayer : public Layer
{
public:
	EventLayer();
	~EventLayer();
	CREATE_FUNC(EventLayer);
	bool init();
private:
	virtual bool onTouchBegan(Touch* touch,SDL_Event* event);
	virtual void onTouchMoved(Touch* touch,SDL_Event* event);
	virtual void onTouchEnded(Touch* touch,SDL_Event* event);
};
#endif