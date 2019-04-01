#ifndef __ShortestPathStep_H__
#define __ShortestPathStep_H__
#include<vector>
#include "SDL_Engine/SDL_Engine.h"

using namespace std;
using namespace SDL;

class ShortestPathStep : public Object
{
	SDL_SYNTHESIZE(int, m_nGScore, GScore);
	SDL_SYNTHESIZE(int, m_nHScore, HScore);
public:
	static Point fromTile;
	static Point toTile;
private:
	ShortestPathStep* m_pParent;
	Point m_tilePos;
public:
	ShortestPathStep();
	~ShortestPathStep();

	static ShortestPathStep* create(const Point& tilePos);
	bool init(const Point& tilePos);

	bool equals(const ShortestPathStep& other) const;
	Point& getTilePos() { return m_tilePos; }
	void setTilePos(const Point& pos) { m_tilePos = pos;};
	int getFScore() const;
	void description();
	ShortestPathStep* getParent() const;
	void setParent(ShortestPathStep* other);

	bool operator>(ShortestPathStep* other);
	bool operator<(ShortestPathStep* other);
	bool operator<=(ShortestPathStep* other);
	bool operator>=(ShortestPathStep* other);
};
#endif