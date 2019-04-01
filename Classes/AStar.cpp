#include "AStar.h"
#include "ShortestPathStep.h"
#include "StaticData.h"

AStar::AStar()
	:isPassing(nullptr)
	,isPassing4(nullptr)
{
}

AStar::~AStar()
{
}

bool AStar::init()
{
	return true;
}

ShortestPathStep* AStar::parse(const Point& fromTile,const Point& toTile)
{
	bool bPathFound = false;
	ShortestPathStep* pTail = nullptr;
	//设置开始和结束位置
	ShortestPathStep::fromTile = fromTile;
	ShortestPathStep::toTile = toTile;
	//方向数组
	vector<Direction> dirs;
	dirs.push_back(Direction::Down);
	dirs.push_back(Direction::Left);
	dirs.push_back(Direction::Right);
	dirs.push_back(Direction::Up);
	//把开始位置插入到开始列表中
	auto from = ShortestPathStep::create(fromTile);
	m_openSteps.push_back(from);

	do
	{
		ShortestPathStep* currentStep = m_openSteps.front();
		m_openSteps.erase(m_openSteps.begin());
		//添加到封闭列表
		m_closeSteps.push_back(currentStep);
		//如果当前路径就是目的点，搜索完成，退出循环
		if (currentStep->getTilePos().equals(toTile))
		{
			bPathFound = true;
			pTail = currentStep;
			//清除列表
			m_openSteps.clear();
			m_closeSteps.clear();

			break;
		}
		//对四方向进行遍历
		for (const auto& dir : dirs)
		{
			Point tilePos;
			Direction oppositeDir;

			StaticData::getInstance()->direction(dir, nullptr, &tilePos, &oppositeDir);

			tilePos += currentStep->getTilePos();
			//在闭合列表已经存在该位置 直接跳过
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
				if (isValid(tilePos) && isPassing4(currentStep->getTilePos(), dir)
					&& (tilePos == toTile || isPassing(tilePos)) && isPassing4(tilePos, oppositeDir))
				{
					ShortestPathStep* step = ShortestPathStep::create(tilePos);

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
	}while( !m_openSteps.empty());

	return pTail;
}

int AStar::computeHScoreFromCoord(const Point& fromTileCoord, const Point& toTileCoord)
{
	return (int)abs(fromTileCoord.x - toTileCoord.x ) + (int)abs(fromTileCoord.y - toTileCoord.y);
}

void AStar::insertToOpenSteps(ShortestPathStep* step)
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

int AStar::calculateCost(const Point& tilePos)
{
	return 1;
}

bool AStar::isValid(const Point&tilePos) const
{
	if (tilePos.x < 0 || tilePos.x > m_mapSize.width
		|| tilePos.y < 0 || tilePos.y > m_mapSize.height)
		return false;

	return true;
}

vector<ShortestPathStep*>::const_iterator AStar::containsTilePos(const vector<ShortestPathStep*>& vec,const Point& tilePos)
{
	auto it = find_if(vec.cbegin(), vec.cend(), [&tilePos](ShortestPathStep* step)
	{
		return step->getTilePos().equals(tilePos);
	});

	return it;
}