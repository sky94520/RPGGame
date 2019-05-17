#include "Controller.h"
#include "ControllerListener.h"

Controller::Controller()
	:m_pListener(nullptr)
	,m_durationPerGrid(0.2f)
	,m_pFollowController(nullptr)
{
}

Controller::~Controller()
{
	SDL_SAFE_RELEASE(m_pFollowController);
}

void Controller::setControllerListener(ControllerListener* pListener)
{
	this->m_pListener = pListener;
}

void Controller::setFollower(Controller* pController)
{
	m_pFollowController = pController;
	SDL_SAFE_RETAIN(m_pFollowController);
}