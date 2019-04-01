#include "SDL_Engine/SDL_Engine.h"

#include "GameScene.h"
#include "StartScene.h"
#include "DynamicData.h"
#include "StaticData.h"

using namespace std;
using namespace SDL;

int main(int argv,char**argc)
{
	//窗口创建成功
	if(Director::getInstance()->init("RPG",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,960,640,SDL_WINDOW_SHOWN))
	{
		auto platform = Director::getInstance()->getPlatform();
		//Windows下添加资源路径
		if (platform == "Windows" || platform == "Linux")
		{
			FileUtils::getInstance()->addSearchPath("Resources");
			FileUtils::getInstance()->addSearchPath("Resources/script");
		}
		else//设置鼠标和触碰事件分离
		{
			SDL_SetHint(SDL_HINT_ANDROID_SEPARATE_MOUSE_AND_TOUCH,"1");
			FileUtils::getInstance()->addSearchPath("script");
		}
		//开启垂直同步
		SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

		//第一个启动场景
		Director::getInstance()->runWithScene(StartScene::create());
		Director::getInstance()->setDisplayStates(true);
		Director::getInstance()->setSecondsPerFrame(1/60.f);
		Director::getInstance()->setResolutionScale();

		while(Director::getInstance()->isRunning())
		{
			Director::getInstance()->update();
			PoolManager::getInstance()->getCurReleasePool()->clear();
		}
		StaticData::purge();
		DynamicData::purge();
		//释放GameScene
		GameScene::purge();
		//系统清除
		Director::purge();
		//释放内存池
		PoolManager::getInstance()->purge();
	}

	return 0;
}
