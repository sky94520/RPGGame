#include "EventLayer.h"
#include "GameScene.h"
#include "StaticData.h"
#include "GoodLayer.h"
#include "ScriptLayer.h"
#include "BattleScene.h"

EventLayer::EventLayer()
{
}

EventLayer::~EventLayer()
{
}

bool EventLayer::init()
{
	auto listener = EventListenerTouchOneByOne::create();

	listener->setPriority(1);
	listener->onTouchBegan = SDL_CALLBACK_2(EventLayer::onTouchBegan,this);
	listener->onTouchMoved = SDL_CALLBACK_2(EventLayer::onTouchMoved,this);
	listener->onTouchEnded = SDL_CALLBACK_2(EventLayer::onTouchEnded,this);

	_eventDispatcher->addEventListener(listener,this);

	return true;
}

bool EventLayer::onTouchBegan(Touch* touch, SDL_Event* event)
{
	auto location = touch->getLocation();
	auto gameScene = GameScene::getInstance();
	auto gameState = gameScene->getGameState();
	//仅在正常状态下且背包未打开，才能点击地面
	if (gameState == GameState::Normal)
	{
		if (gameScene->isPathClickable())
			gameScene->clickPath(location);
	}//非正常状态下，且等待类型为Click
	else if(gameScene->getScriptLayer()->getWaitType() == WaitType::Click)
	{
		gameScene->clickEvent(location);
	}
	else if (gameState == GameState::Fighting)
	{
		gameScene->getBattleScene()->onTouchBegan(touch, event);
	}

	return true;
}

void EventLayer::onTouchMoved(Touch* touch,SDL_Event* event)
{
}

void EventLayer::onTouchEnded(Touch* touch,SDL_Event* event)
{
}
