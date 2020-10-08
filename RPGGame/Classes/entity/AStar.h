#ifndef __AStar_H__
#define __AStar_H__
#include <cmath>
#include <algorithm>
#include <functional>
#include "SDL_Engine/SDL_Engine.h"

using namespace std;
using namespace SDL;

class PathStep;
enum class Direction;

//抽象类，使用A星算法必须要实现该方法
class AStartDelegate
{
public:
	virtual bool isPassing(const SDL_Point& tilePos) const=0; //tilePos是否可通过
	virtual bool isPassing4(const SDL_Point& tilePos, Direction direction) const=0;
};

/*
	A星算法
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
	//获取fromTile=>toTile的路径，若无则返回NULL
	PathStep* parse(const SDL_Point& fromTile, const SDL_Point& toTile, unsigned maxCount=100);
	//模拟使用
	void startStep(const SDL_Point& fromTile, const SDL_Point& toTile);
	PathStep* step(const SDL_Point& toTile);
	void stopStep();
	vector<PathStep*>* getOpenSteps() { return &m_openSteps; }
	void setMapSize(unsigned int rowTileNum, unsigned int colTileNum);
	void setDelegate(AStartDelegate* pDelegate) { m_pDelegate = pDelegate; }
public:
	static int computeHScoreFromCoord(const SDL_Point& fromTileCoord, const SDL_Point& toTileCoord);
private:
	void insertToOpenSteps(PathStep* step);
	//根据对应位置获取代价
	int calculateCost(const SDL_Point& tilePos);
	bool isValid(const SDL_Point& tilePos)const;

	vector<PathStep*>::const_iterator containsTilePos(const vector<PathStep*>& vec, const SDL_Point& tilePos);
private:
	static bool getNextTilePos(Direction dir, PathStep* step,string* sDir,SDL_Point* nextPos,Direction* oppsite);
private:
	vector<PathStep*> m_openSteps;
	vector<PathStep*> m_closeSteps;
	vector<Direction> m_dirs;
	//地图宽、高的图块个数
	unsigned int m_mapRowTileNum;
	unsigned int m_mapColTileNum;
	//委托者
	AStartDelegate* m_pDelegate;
};
#endif
