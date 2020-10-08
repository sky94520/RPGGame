#include "FollowController.h"
#include "PathStep.h"
#include "AStarController.h"
#include "../GameMacros.h"
#include "ControllerListener.h"

FollowController::FollowController()
	:m_pLastStep(nullptr)
	,m_pCurStep(nullptr)
	,m_pPendingStep(nullptr)
{
}

FollowController::~FollowController()
{
	SDL_SAFE_RELEASE(m_pLastStep);
	SDL_SAFE_RELEASE(m_pCurStep);
	SDL_SAFE_RELEASE(m_pPendingStep);
}

bool FollowController::init()
{
	return true;
}

float FollowController::moveToward(const SDL_Point& tilePos)
{
	//当前角色正在运动.先停止运动
	if (m_pCurStep != nullptr)
	{
		SDL_SAFE_RELEASE_NULL(m_pCurStep);
	}
	m_pCurStep = PathStep::create(tilePos);
	SDL_SAFE_RETAIN(m_pCurStep);
	//开始运动
	this->popStepAndAnimate();

	return true;
}

void FollowController::moveOneStep(PathStep* step)
{
	if (step == nullptr)
	{
		m_pListener->changeState(State::Idle);

		if (m_pFollowController != nullptr)
			m_pFollowController->moveOneStep(nullptr);
		return;
	}
	SDL_SAFE_RETAIN(step);
	//当前角色正在运动.暂存该点
	if (m_pCurStep != nullptr)
	{
		m_pPendingStep = step;
		return;
	}
	m_pCurStep = step;

	this->popStepAndAnimate();
}

void FollowController::popStepAndAnimate()
{
	m_pListener->setMoving(false);

	if (m_pPendingStep != nullptr)
	{
		m_pCurStep = m_pPendingStep;
		m_pPendingStep = nullptr;
	}
	else if (m_pCurStep == nullptr)
		return;

	auto tilePos = m_pCurStep->getTilePos();
	CallFunc* moveCallback = CallFunc::create([this]()
	{
		//调用一步的不需要回调,仅仅需要保存上一个
		SDL_SAFE_RELEASE(m_pLastStep);
		m_pLastStep = m_pCurStep;
		m_pCurStep = nullptr;

		this->popStepAndAnimate();
	});
	//存在跟随角色，设置跟随
	if (m_pFollowController != nullptr)
	{
		m_pFollowController->moveOneStep(m_pLastStep);
	}
	m_pListener->moveTo(tilePos, m_durationPerGrid, moveCallback);
	//是否改变方向
	SDL_Point delta = {};
	delta.x = tilePos.x - m_pListener->getTilePosition().x;
	delta.y = tilePos.y - m_pListener->getTilePosition().y;

	Direction newDir = AStarController::getDirection(delta);
	Direction dir = m_pListener->getDirection();

	if (newDir != dir)
	{
		m_pListener->setDirection(newDir);
	}
	//改为运动状态
	m_pListener->changeState(State::Walking);
	m_pListener->setMoving(true);
}
