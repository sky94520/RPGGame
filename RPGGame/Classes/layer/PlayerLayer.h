#ifndef __PlayerLayer_H__
#define __PlayerLayer_H__
#include<vector>
#include "SDL_Engine/SDL_Engine.h"
using namespace std;
using namespace SDL;

class Character;
class Controller;
class AStarController;

class PlayerLayer : public Layer
{
public:
	PlayerLayer();
	~PlayerLayer();
	CREATE_FUNC(PlayerLayer);
	bool init();

	//添加角色
	void initializePlayers(Node* layer);

	virtual void update(float dt);
	//获取主角 默认第一个为主角
	Character* getPlayer() const;
	AStarController* getAStarController();
	//获取角色对应的索引
	int getIndexOfCharacter(const string& chartletName);
	Character* getPlayerOfID(int id);
	//是否与角色发生碰撞
	bool isCollidedWithCharacter(const Rect& rect);
	void movePlayer(const SDL_Point& toTile);
private:
	vector<Controller*> m_controllers;
};
#endif