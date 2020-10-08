#include "AStar.h"
#include "PathStep.h"
#include "../GameMacros.h"

AStar::AStar()
	:isPassing(nullptr)
	,m_mapRowTileNum(0)
	,m_mapColTileNum(0)
{
	//方向数组
	m_dirs.push_back(Direction::Down);
	m_dirs.push_back(Direction::Left);
	m_dirs.push_back(Direction::Right);
	m_dirs.push_back(Direction::Up);
}

AStar::~AStar()
{
	this->stopStep();
}

bool AStar::init()
{
	return true;
}

PathStep* AStar::parse(const SDL_Point& fromTile,const SDL_Point& toTile)
{
	PathStep* pTail = nullptr;
	//设置开始和结束位置
	PathStep::fromTile = fromTile;
	PathStep::toTile = toTile;
	//把开始位置插入到开始列表中
	auto from = PathStep::create(fromTile);
	m_openSteps.push_back(from);
	//记录循环次数
	unsigned int count = 0;
	//TODO:假定最大循环次数
	unsigned int maxCount = 100;

	do
	{
		pTail = this->step();
		count++;

	}while( !m_openSteps.empty() && count < maxCount);

	return pTail;
}


void AStar::startStep(const SDL_Point& fromTile, const SDL_Point& toTile)
{
	//设置开始和结束位置
	PathStep::fromTile = fromTile;
	PathStep::toTile = toTile;
	//把开始位置插入到开始列表中
	auto from = PathStep::create(fromTile);
	SDL_SAFE_RETAIN(from);
	m_openSteps.push_back(from);
}

PathStep* AStar::step()
{
	if (m_openSteps.empty())
		return nullptr;

	PathStep* currentStep = m_openSteps.front();
	m_openSteps.erase(m_openSteps.begin());
	//添加到封闭列表
	m_closeSteps.push_back(currentStep);
	//如果当前路径就是目的点，搜索完成，退出循环
	if (equal(currentStep->getTilePos(), PathStep::toTile))
	{
		//清除列表
		m_openSteps.clear();
		m_closeSteps.clear();

		return currentStep;
	}
	//对四方向进行遍历
	for (const auto& dir : m_dirs)
	{
		SDL_Point tilePos = { 0, 0 };
		Direction oppositeDir;

		AStar::direction(dir, nullptr, &tilePos, &oppositeDir);

		tilePos.x += currentStep->getTilePos().x;
		tilePos.y += currentStep->getTilePos().y;
		//避免出界
		if (!this->isValid(tilePos))
			continue;
		//在闭合列表已经存在该位置 跳过本方向
		if (containsTilePos(m_closeSteps,tilePos) != m_closeSteps.end())
		{
			continue;
		}
		int moveCost = calculateCost(tilePos);
		//如果该位置不在开放列表中，添加
		auto it = containsTilePos(m_openSteps, tilePos);
		if (it == m_openSteps.end())
		{
			//目标合法才添加 目标为toTile时，不进行通过检测
			if (equal(tilePos, PathStep::toTile) || isPassing(tilePos))
			{
				PathStep* step = PathStep::create(tilePos);

				step->setParent(currentStep);
				step->setGScore(currentStep->getGScore() + moveCost);
				step->setHScore(computeHScoreFromCoord(tilePos, PathStep::toTile));
				//插入到开放列表中
				insertToOpenSteps(step);
			}
		}
		else
		{
			auto step = (*it);
			//当前花费小于原来的花费，覆盖其值
			if (currentStep->getGScore() + moveCost < step->getGScore())
			{
				step->setGScore(currentStep->getGScore() + moveCost);
				step->setParent(currentStep);
				//移除后重新添加
				m_openSteps.erase(it);
				insertToOpenSteps(step);
				
			}
		}
	}
	return currentStep;
}

void AStar::stopStep()
{
	for (auto it = m_openSteps.begin(); it != m_openSteps.end();)
	{
		auto step = *it;
		SDL_SAFE_RELEASE(step);

		it = m_openSteps.erase(it);
	}
	for (auto it = m_closeSteps.begin(); it != m_closeSteps.end();)
	{
		auto step = *it;
		SDL_SAFE_RELEASE(step);

		it = m_closeSteps.erase(it);
	}
}

void AStar::setMapSize(unsigned int rowTileNum, unsigned int colTileNum)
{
	m_mapRowTileNum = rowTileNum;
	m_mapColTileNum = colTileNum;
}

int AStar::computeHScoreFromCoord(const SDL_Point& fromTileCoord, const SDL_Point& toTileCoord)
{
	return abs(fromTileCoord.x - toTileCoord.x ) + abs(fromTileCoord.y - toTileCoord.y);
}

void AStar::insertToOpenSteps(PathStep* step)
{
	int stepFScore = step->getFScore();

	auto it = m_openSteps.begin();
	//找到合适的插入位置
	for (;it != m_openSteps.end();it++)
	{
		auto temp = *it;
		if (stepFScore < temp->getFScore())
		{
			break;
		}
	}
	//插入
	m_openSteps.insert(it, step);
}

int AStar::calculateCost(const SDL_Point& tilePos)
{
	return 1;
}

bool AStar::isValid(const SDL_Point& tilePos) const
{
	if (tilePos.x < 0 || tilePos.x >= m_mapRowTileNum ||
		tilePos.y < 0 || tilePos.y >= m_mapColTileNum)
		return false;

	return true;
}

vector<PathStep*>::const_iterator AStar::containsTilePos(const vector<PathStep*>& vec,const SDL_Point& tilePos)
{
	auto it = find_if(vec.cbegin(), vec.cend(), [&tilePos, this](PathStep* step)
	{
		return equal(step->getTilePos(), tilePos);
	});

	return it;
}

bool AStar::equal(const SDL_Point& p1, const SDL_Point& p2) const
{
	return p1.x == p2.x && p1.y == p2.y;
}

bool AStar::direction(Direction dir,string* sDir,SDL_Point* delta,Direction* oppsite)
{
	if (sDir == nullptr && delta == nullptr && oppsite == nullptr)
		return false;

	SDL_Point temp = { 0,0 };
	Direction oppsiteDir = dir;
	string text;

	switch (dir)
	{
	case Direction::Down:
		text = "down";
		temp.y = 1;
		oppsiteDir = Direction::Up;
		break;
	case Direction::Left:
		text = "left";
		temp.x = -1;
		oppsiteDir = Direction::Right;
		break;
	case Direction::Right:
		text = "right";
		temp.x = 1;
		oppsiteDir = Direction::Left;
		break;
	case Direction::Up:
		text = "up";
		temp.y = -1;
		oppsiteDir = Direction::Down;
		break;
	default:
		break;
	}

	if (sDir != nullptr)
		*sDir = text;
	if (delta != nullptr)
		*delta = temp;
	if (oppsite != nullptr)
		*oppsite = oppsiteDir;

	return true;
}
