#include "PathStep.h"

PathStep::PathStep()
	:m_nGScore(0)
	,m_nHScore(0)
	,m_pParent(nullptr)
{
}

PathStep::~PathStep()
{
}
PathStep* PathStep::create(const SDL_Point& tilePos)
{
	auto step = new PathStep();

	if (step && step->init(tilePos))
		step->autorelease();
	else
		SDL_SAFE_DELETE(step);

	return step;
}

bool PathStep::init(const SDL_Point& tilePos)
{
	m_tilePos = tilePos;
	return true;
}

bool PathStep::equals(const PathStep& other) const
{
	return m_tilePos.x == other.m_tilePos.x &&
		m_tilePos.y == other.m_tilePos.y;
}

bool PathStep::equals(const SDL_Point& other) const 
{
	return m_tilePos.x == other.x
		&& m_tilePos.y == other.y;
}

int PathStep::getFScore() const
{
	return m_nGScore + m_nHScore /** (int)cross*/;
}

void PathStep::description()
{
	printf("tile_pos:%d,%d gscore%d,hscore%d\n",m_tilePos.x,m_tilePos.y,m_nGScore,m_nHScore);
}

PathStep* PathStep::getParent() const
{
	return m_pParent;
}

void PathStep::setParent(PathStep* other)
{
	m_pParent = other;
}

bool PathStep::operator>(PathStep* other)
{
	return this->getFScore() > other->getFScore();
}

bool PathStep::operator<(PathStep* other)
{
	return this->getFScore() < other->getFScore();
}

bool PathStep::operator<=(PathStep* other)
{
	return this->getFScore() <= other->getFScore();
}

bool PathStep::operator>=(PathStep* other)
{
	return this->getFScore() >= other->getFScore();
}
