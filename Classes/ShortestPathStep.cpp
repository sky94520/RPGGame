#include "ShortestPathStep.h"

Point ShortestPathStep::fromTile = Point::ZERO;
Point ShortestPathStep::toTile = Point::ZERO;

ShortestPathStep::ShortestPathStep()
	:m_nGScore(0)
	,m_nHScore(0)
	,m_pParent(nullptr)
{
}

ShortestPathStep::~ShortestPathStep()
{
}
ShortestPathStep* ShortestPathStep::create(const Point& tilePos)
{
	auto step = new ShortestPathStep();

	if (step && step->init(tilePos))
		step->autorelease();
	else
		SDL_SAFE_DELETE(step);

	return step;
}

bool ShortestPathStep::init(const Point& tilePos)
{
	this->setTilePos(tilePos);

	return true;
}

bool ShortestPathStep::equals(const ShortestPathStep& other) const
{
	return m_tilePos.equals(other.m_tilePos);
}

int ShortestPathStep::getFScore() const
{
/*	auto dx1 = m_tilePos.x - ShortestPathStep::toTile.x;
	auto dy1 = m_tilePos.y - ShortestPathStep::toTile.y;

	auto dx2 = fromTile.x - ShortestPathStep::toTile.x;
	auto dy2 = fromTile.y - ShortestPathStep::toTile.y;

	auto cross = abs(dx1 * dy2 - dx2 * dy1);

	if (cross != 1 && cross != 2)
		cross = 100;*/

	return m_nGScore + m_nHScore /** (int)cross*/;
}

void ShortestPathStep::description()
{
	printf("tile_pos:%.f,%.f gscore%d,hscore%d\n",m_tilePos.x,m_tilePos.y,m_nGScore,m_nHScore);
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