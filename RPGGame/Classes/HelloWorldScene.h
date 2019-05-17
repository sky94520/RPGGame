#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "SDL_Engine/SDL_Engine.h"

using namespace SDL;

class AStarController;
class MapLayer;
class Character;

class HelloWorld : public Layer
{
public:
	HelloWorld();
	~HelloWorld();
    CREATE_FUNC(HelloWorld);
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
   	static Scene* createScene();
	bool init();
	void update(float dt);
	 
	 // a selector callback
    void menuCloseCallback(Object* pSender);
	bool onTouchBegan(Touch* touch, SDL_Event* event);
private:
	bool isPassing(const SDL_Point& tilePos);
private:
	MapLayer* m_pMapLayer;
	AStarController* m_pController;
	Character* m_pViewpointCharacter;
};

#endif // __HELLOWORLD_SCENE_H__
