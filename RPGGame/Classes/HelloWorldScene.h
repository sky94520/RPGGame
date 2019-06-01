#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "SDL_Engine/SDL_Engine.h"

using namespace SDL;

class Text;

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
	Text* m_pText;
};

#endif // __HELLOWORLD_SCENE_H__
