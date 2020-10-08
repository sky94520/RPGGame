#ifndef __PathStep_H__
#define __PathStep_H__
#include<vector>
#include "SDL_Engine/SDL_Engine.h"

using namespace std;
using namespace SDL;

class PathStep : public Object
{
	SDL_SYNTHESIZE(int, m_nGScore, GScore);
	SDL_SYNTHESIZE(int, m_nHScore, HScore);
private:
	PathStep* m_pParent;
	SDL_Point m_tilePos;
public:
	PathStep();
	~PathStep();

	static PathStep* create(const SDL_Point& tilePos);

	bool init(const SDL_Point& tilePos);

	bool equals(const PathStep& other) const;
	bool equals(const SDL_Point& other) const;

	SDL_Point& getTilePos() { return m_tilePos; }
	void setTilePos(const SDL_Point& pos) { m_tilePos = pos;};
	int getFScore() const;
	void description();
	PathStep* getParent() const;
	void setParent(PathStep* other);

	bool operator>(PathStep* other);
	bool operator<(PathStep* other);
	bool operator<=(PathStep* other);
	bool operator>=(PathStep* other);
};
#endif
