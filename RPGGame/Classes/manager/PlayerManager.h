#ifndef __PlayerManager_H__
#define __PlayerManager_H__
#include<vector>
#include "SDL_Engine/SDL_Engine.h"
using namespace std;
using namespace SDL;

class Character;
class Controller;
class AStarController;

class PlayerManager : public Layer
{
public:
	PlayerManager();
	~PlayerManager();
	CREATE_FUNC(PlayerManager);
	bool init();

	//添加角色到指定层
	void initializePlayers(Node* layer);

	virtual void update(float dt);
	//获取主角 默认第一个为主角
	Character* getPlayer() const;
	AStarController* getAStarController();
	//获取角色对应的索引
	int getIndexOfCharacter(const string& chartletName);
	Character* getPlayerOfID(int id);
	Controller* getControllerOfPlayerID(int id);
	//是否与角色发生碰撞
	bool isCollidedWithCharacter(const Rect& rect);
	void movePlayer(const SDL_Point& toTile);
	bool movePlayer(Touch* touch);
	//改变玩家的所在层
	void changeLayerOfPlayer(Node* layer, const Point& location);
	//获取所有的玩家角色
	vector<Character*> getCharacterList();
	const vector<Controller*>& getControllers()const { return m_controllers; }
private:
	vector<Controller*> m_controllers;
};
#endif