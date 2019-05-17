#include "ShortestPathStep.h"

SDL_Point ShortestPathStep::fromTile = { 0, 0 };
SDL_Point ShortestPathStep::toTile = { 0, 0 };

ShortestPathStep::ShortestPathStep()
	:m_nGScore(0)
	,m_nHScore(0)
	,m_pParent(nullptr)
{
}

ShortestPathStep::~ShortestPathStep()
{
}
ShortestPathStep* ShortestPathStep::create(const SDL_Point& tilePos)
{
	auto step = new ShortestPathStep();

	if (step && step->init(tilePos))
		step->autorelease();
	else
		SDL_SAFE_DELETE(step);

	return step;
}

bool ShortestPathStep::init(const SDL_Point& tilePos)
{
	this->setTilePos(tilePos);

	return true;
}

bool ShortestPathStep::equals(const ShortestPathStep& other) const
{
	return m_tilePos.x == other.m_tilePos.x &&
		m_tilePos.y == other.m_tilePos.y;
}

int ShortestPathStep::getFScore() const
{
	return m_nGScore + m_nHScore /** (int)cross*/;
}

void ShortestPathStep::description()
{
	printf("tile_pos:%d,%d gscore%d,hscore%d\n",m_tilePos.x,m_tilePos.y,m_nGScore,m_nHScore);
}

ShortestPathStep* ShortestPathStep::getParent() const
{
	return m_pParent;
}

void ShortestPathStep::setParent(ShortestPathStep* other)
{
	m_pParent = other;
}

bool ShortestPathStep::operator>(ShortestPathStep* other)
{
	return this->getFScore() > other->getFScore();
}

bool ShortestPathStep::operator<(ShortestPathStep* other)
{
	return this->getFScore() < other->getFScore();
}

bool ShortestPathStep::operator<=(ShortestPathStep* other)
{
	return this->getFScore() <= other->getFScore();
}

bool ShortestPathStep::operator>=(ShortestPathStep* other)
{
	return this->getFScore() >= other->getFScore();
}
