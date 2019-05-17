#ifndef __Character_H__
#define __Character_H__

#include <string>

#include "Entity.h"
#include "ControllerListener.h"

using namespace std;

enum class State;
enum class Direction;

class Character : public Entity, public ControllerListener
{
	SDL_SYNTHESIZE_READONLY(string, m_chartletName, ChartletName);//当前贴图名，也可以认为是人物名称，唯一
public:
	Character();
	~Character();
	static Character* create(const string& chartletName);
	static Character* create(const string& chartletName, Direction direction);

	bool init(const string& chartletName);
	bool init(const string& chartletName, Direction direction);

	void clear();
	//站立
	void sit();

	virtual SDL_Point getTilePosition() const;
	//切换状态
	virtual void changeState(State state);
	//移动
	virtual void moveTo(const SDL_Point& toTile, float duration, CallFunc* endCallback);

	virtual Direction getDirection() const;
	virtual void setDirection(Direction dir);
	
	virtual bool isMoving() const { return m_bMoving; };
	virtual void setMoving(bool moving) 
	{
		m_bMoving = moving;
	};
	static void setTileSize(int tileWidth, int tileHeight);
protected:
	static int TILE_WIDTH;
	static int TILE_HEIGHT;
protected:
	bool m_bDirty;
private:
	Direction m_dir;
	State m_state;
	//运动相关
	bool m_bMoving;
};
#endif