#include "AStar.h"
#include "PathStep.h"
#include "../GameMacros.h"

AStar::AStar()
	:m_mapRowTileNum(0)
	,m_mapColTileNum(0)
	,m_pDelegate(nullptr)
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

PathStep* AStar::parse(const SDL_Point& fromTile,const SDL_Point& toTile, unsigned maxCount)
{
	PathStep* pTail = nullptr;
	//把开始位置插入到开始列表中
	auto from = PathStep::create(fromTile);
	m_openSteps.push_back(from);
	//循环次数不超过最大循环次数
	unsigned int count = 0;
	//步进
	do
	{
		pTail = this->step(toTile);
		count++;

	}while( !m_openSteps.empty() && count < maxCount);

	return pTail;
}


void AStar::startStep(const SDL_Point& fromTile, const SDL_Point& toTile)
{
	//把开始位置插入到开始列表中
	auto from = PathStep::create(fromTile);
	SDL_SAFE_RETAIN(from);
	m_openSteps.push_back(from);
}

PathStep* AStar::step(const SDL_Point& toTile)
{
	if (m_openSteps.empty())
		return nullptr;
	//获取数组首个元素
	PathStep* currentStep = m_openSteps.back();
	m_openSteps.pop_back();
	//添加到封闭列表
	m_closeSteps.push_back(currentStep);
	//如果当前路径就是目的点，搜索完成，退出循环
	if (currentStep->equals(toTile))
	{
		//清除列表
		m_openSteps.clear();
		m_closeSteps.clear();
		return currentStep;
	}
	//对四方向进行遍历
	SDL_Point tilePos = { 0, 0 };
	Direction oppositeDir;
	for (const auto& dir : m_dirs)
	{
		//获取下一个方向和对应的方向
		AStar::getNextTilePos(dir, currentStep, nullptr, &tilePos, &oppositeDir);
		//出界或者已经访问过
		if (!this->isValid(tilePos) || containsTilePos(m_closeSteps,tilePos) != m_closeSteps.end())
			continue;
		int moveCost = calculateCost(tilePos);
		//在列表中，尝试更新 否则，直接添加
		auto it = containsTilePos(m_openSteps, tilePos);
		if (it == m_openSteps.end())
		{
			//目标合法才添加
			if (((tilePos.x == toTile.x && tilePos.y == toTile.y) || m_pDelegate->isPassing(tilePos)) //1 可通过
					&& m_pDelegate->isPassing4(currentStep->getTilePos(), dir) //2 当前所在侧边可通过
					&& m_pDelegate->isPassing4(tilePos, oppositeDir) //3 下一个图块对立侧边可通过
				)
			{
				PathStep* step = PathStep::create(tilePos);

				step->setParent(currentStep);
				step->setGScore(currentStep->getGScore() + moveCost);
				step->setHScore(computeHScoreFromCoord(tilePos, toTile));
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
	//找到合适的插入位置
	int score = step->getFScore();
	auto it = m_openSteps.begin();
	while (it != m_openSteps.end())
	{
		if (score > (*it)->getFScore())
			break;
		it++;
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
		return step->equals(tilePos);
	});

	return it;
}

bool AStar::getNextTilePos(Direction dir, PathStep* step, string* sDir,SDL_Point* nextPos,Direction* oppsite)
{
	if (sDir == nullptr && nextPos == nullptr && oppsite == nullptr)
		return false;

	SDL_Point temp = { 0,0 };
	if (step != nullptr)
	{
		temp = step->getTilePos();
	}
	Direction oppsiteDir = dir;
	string text;

	switch (dir)
	{
	case Direction::Down:
		text = "down";
		temp.y += 1;
		oppsiteDir = Direction::Up;
		break;
	case Direction::Left:
		text = "left";
		temp.x -= 1;
		oppsiteDir = Direction::Right;
		break;
	case Direction::Right:
		text = "right";
		temp.x += 1;
		oppsiteDir = Direction::Left;
		break;
	case Direction::Up:
		text = "up";
		temp.y -= 1;
		oppsiteDir = Direction::Down;
		break;
	default:
		break;
	}

	if (sDir != nullptr)
		*sDir = text;
	if (nextPos != nullptr)
	{
		*nextPos = temp;
	}
	if (oppsite != nullptr)
		*oppsite = oppsiteDir;

	return true;
}
