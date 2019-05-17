#ifndef __AStar_H__
#define __AStar_H__
#include <cmath>
#include <algorithm>
#include <functional>
#include "SDL_Engine/SDL_Engine.h"

using namespace std;
using namespace SDL;

class ShortestPathStep;
enum class Direction;

/*
	A*算法
	需要预先获取地图图块个数和赋值isPassing函数
	为了扩展性，本算法认为目的地一定可以达到
*/
class AStar : public Object
{
public:
	AStar();
	~AStar();
	CREATE_FUNC(AStar);
	bool init();

	ShortestPathStep* parse(const SDL_Point& fromTile, const SDL_Point& toTile);
	//模拟使用
	void startStep(const SDL_Point& fromTile, const SDL_Point& toTile);
	ShortestPathStep* step();
	void stopStep();
	vector<ShortestPathStep*>* getOpenSteps() { return &m_openSteps; }
	void setMapSize(unsigned int rowTileNum, unsigned int colTileNum);
public:
	static int computeHScoreFromCoord(const SDL_Point& fromTileCoord, const SDL_Point& toTileCoord);
private:
	void insertToOpenSteps(ShortestPathStep* step);
	//根据对应位置获取代价
	int calculateCost(const SDL_Point& tilePos);
	bool isValid(const SDL_Point& tilePos)const;

	vector<ShortestPathStep*>::const_iterator containsTilePos(const vector<ShortestPathStep*>& vec, const SDL_Point& tilePos);
	bool equal(const SDL_Point& p1, const SDL_Point& p2) const;
private:
	static bool direction(Direction dir,string* sDir,SDL_Point* delta,Direction* oppsite);
public:
	std::function<bool (const SDL_Point& tilePos)> isPassing;
private:
	vector<ShortestPathStep*> m_openSteps;
	vector<ShortestPathStep*> m_closeSteps;
	vector<Direction> m_dirs;
	//地图宽|高图块个数
	unsigned int m_mapRowTileNum;
	unsigned int m_mapColTileNum;
};
#endif
