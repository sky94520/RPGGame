#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "SDL_Engine/SDL_Engine.h"

using namespace SDL;

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
    //add a touch listener
    bool onTouchBegan(Touch* touch, SDL_Event* event);

	void onKeyPressed(SDL_Keycode keycode, SDL_Event* e);
	void onKeyReleased(SDL_Keycode keycode, SDL_Event* e);
};

#endif // __HELLOWORLD_SCENE_H__
