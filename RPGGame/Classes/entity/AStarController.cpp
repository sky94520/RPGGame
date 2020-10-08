#include "AStarController.h"
#include "AStar.h"
#include "PathStep.h"
#include "../GameMacros.h"
#include "LuaObject.h"
#include "ControllerListener.h"

string const AStarController::CHARACTER_MOVE_TO_TILE = "character move to tile";

AStarController::AStarController()
	:m_stepIndex(0)
	,m_pLastStep(nullptr)
	,m_pendingMove({0,0})
	,m_bPendingMove(false)
	,m_pAStar(nullptr)
	,m_pTriggerObject(nullptr)
{
}

AStarController::~AStarController()
{
	SDL_SAFE_RELEASE(m_pAStar);
	this->clearShortestPath();	
	SDL_SAFE_RELEASE(m_pLastStep);
	SDL_SAFE_RELEASE(m_pTriggerObject);
}

AStarController* AStarController::create(AStar* pAStar)
{
	auto controller = new AStarController();

	if (controller && controller->init(pAStar))
		controller->autorelease();
	else
		SDL_SAFE_DELETE(controller);

	return controller;
}

bool AStarController::init(AStar* pAStar)
{
	SDL_SAFE_RETAIN(pAStar);
	m_pAStar = pAStar;

	return true;
}

void AStarController::setTriggerObject(LuaObject* luaObject)
{
	SDL_SAFE_RETAIN(luaObject);
	SDL_SAFE_RELEASE(m_pTriggerObject);
	m_pTriggerObject = luaObject;
}

float AStarController::moveToward(const SDL_Point& tilePos)
{
	float duration = 0.f;
	//当前角色正在运动，则更改待到达目的地
	if (m_pListener->isMoving())
	{
		m_bPendingMove = true;
		m_pendingMove = tilePos;

		return duration;
	}
	//获取角色的位置
	SDL_Point fromTile = m_pListener->getTilePosition();
	//A*算法解析路径
	auto pTail = m_pAStar->parse(fromTile, tilePos);
	//目标可达,做运动前准备
	if (pTail != nullptr)
	{
		duration = this->constructPathAndStartAnimation(pTail);
	}
	return duration;
}

void AStarController::popStepAndAnimate()
{
	m_pListener->setMoving(false);
	//存在待到达目的点，转入
	if (m_bPendingMove)
	{
		m_bPendingMove = false;

		this->clearShortestPath();
		//滞后改变
		this->moveToward(m_pendingMove);

		return ;
	}//运动结束
	else if (m_stepIndex >= m_paths.size())
	{
		this->clearShortestPath();
		//站立动画
		m_pListener->changeState(State::Idle);
		//存在跟随角色,停止行走动画
		if (m_pFollowController != nullptr)
			m_pFollowController->moveOneStep(nullptr);

		return ;
	}//触发NPC
	else if (m_pTriggerObject != nullptr && m_stepIndex == m_paths.size() - 1)
	{
		SDL_Point backTile = m_paths.back()->getTilePos();
		SDL_Point delta = {backTile.x - m_pLastStep->getTilePos().x, backTile.y - m_pLastStep->getTilePos().y};
		auto newDir = this->getDirection(delta);
		auto character = static_cast<Character*>(m_pListener);
		//改变方向
		if (newDir != character->getDirection())
			character->setDirection(newDir);
		//停止行走
		this->clearShortestPath();
		character->changeState(State::Idle);

		//触发脚本
		m_pTriggerObject->execScriptFunc(character->getChartletName());
		printf("DEBUG:trigger npc\n");
		SDL_SAFE_RELEASE_NULL(m_pTriggerObject);
		return ;
	}
	//存在跟随角色，设置跟随
	if (m_pFollowController != nullptr)
	{
		m_pFollowController->moveOneStep(m_pLastStep);
	}
	SDL_SAFE_RELEASE(m_pLastStep);
	m_pLastStep = m_paths.at(m_stepIndex);
	SDL_SAFE_RETAIN(m_pLastStep);

	auto step = m_paths.at(m_stepIndex++);
	auto tilePos = step->getTilePos();

	CallFunc* moveCallback = CallFunc::create([this]()
	{
		//发送角色到达图块事件
		auto eventDispatcher = Director::getInstance()->getEventDispatcher();
		eventDispatcher->dispatchCustomEvent(CHARACTER_MOVE_TO_TILE, this);
		this->popStepAndAnimate();
	});
	m_pListener->moveTo(tilePos, m_durationPerGrid, moveCallback);
	//是否改变方向
	SDL_Point delta = {};
	delta.x = tilePos.x - m_pListener->getTilePosition().x;
	delta.y = tilePos.y - m_pListener->getTilePosition().y;

	Direction newDir = this->getDirection(delta);
	Direction dir = m_pListener->getDirection();

	if (newDir != dir)
	{
		m_pListener->setDirection(newDir);
	}
	//改为运动状态
	m_pListener->changeState(State::Walking);
	m_pListener->setMoving(true);
}

float AStarController::constructPathAndStartAnimation(PathStep* pHead)
{
	int number = 0;
	//此时的角色一定不在运动中 构建运动列表
	while (pHead != nullptr && pHead->getParent() != nullptr)
	{
		auto it = m_paths.begin();
		m_paths.insert(it,pHead);

		SDL_SAFE_RETAIN(pHead);
		pHead = pHead->getParent();

		number++;
	}
	//此位置为主角当前tile 位置
	SDL_SAFE_RELEASE(m_pLastStep);
	m_pLastStep = pHead;
	SDL_SAFE_RETAIN(m_pLastStep);

	//开始运动
	this->popStepAndAnimate();

	return number * m_durationPerGrid;
}

void AStarController::clearShortestPath()
{
	for (auto it = m_paths.begin();it != m_paths.end();)
	{
		auto step = *it;

		SDL_SAFE_RELEASE(step);
		it = m_paths.erase(it);
	}
	m_stepIndex = 0;
}

Direction AStarController::getDirection(const SDL_Point& delta)
{
	Direction dir = Direction::Down;

	if (delta.x > 0.f)
	{
		dir = Direction::Right;
	}
	else if (delta.x < 0.f)
	{
		dir = Direction::Left;
	}
	else if (delta.y > 0)
	{
		dir = Direction::Down;
	}
	else if (delta.y < 0)
	{
		dir = Direction::Up;
	}
	return dir;
}
