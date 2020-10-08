#include "PathStep.h"

SDL_Point PathStep::fromTile = { 0, 0 };
SDL_Point PathStep::toTile = { 0, 0 };

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
	this->setTilePos(tilePos);

	return true;
}

bool PathStep::equals(const PathStep& other) const
{
	return m_tilePos.x == other.m_tilePos.x &&
		m_tilePos.y == other.m_tilePos.y;
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
