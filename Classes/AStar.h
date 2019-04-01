#ifndef __AStar_H__
#define __AStar_H__
#include <cmath>
#include <algorithm>
#include <functional>
#include "SDL_Engine/SDL_Engine.h"

using namespace std;
using namespace SDL;

enum class Direction;
class ShortestPathStep;

class AStar : public Object
{
	SDL_SYNTHESIZE(Size,m_mapSize,MapSize);
public:
	std::function<bool (const Point& tilePos)> isPassing;
	std::function<bool (const Point& tilePos,Direction dir)> isPassing4;
private:
	vector<ShortestPathStep*> m_openSteps;
	vector<ShortestPathStep*> m_closeSteps;
public:
	AStar();
	~AStar();
	CREATE_FUNC(AStar);
	bool init();

	ShortestPathStep* parse(const Point& fromTile, const Point& toTile);
public:
	static int computeHScoreFromCoord(const Point& fromTileCoord, const Point& toTileCoord);
private:
	void insertToOpenSteps(ShortestPathStep* step);
	//根据对应位置获取代价
	int calculateCost(const Point& tilePos);
	bool isValid(const Point& tilePos)const;

	vector<ShortestPathStep*>::const_iterator containsTilePos(const vector<ShortestPathStep*>& vec, const Point& tilePos);
};
#endif