#ifndef __PlayerLayer_H__
#define __PlayerLayer_H__
#include<vector>
#include "SDL_Engine/SDL_Engine.h"
using namespace std;
using namespace SDL;

class Character;

class PlayerLayer : public Layer
{
private:
	vector<Character*> m_characters;
public:
	PlayerLayer();
	~PlayerLayer();
	CREATE_FUNC(PlayerLayer);
	bool init();

	virtual void update(float dt);
	//获取主角 默认第一个为主角
	Character* getPlayer() const;
	//获取角色列表
	vector<Character*>& getCharacterList();
	//获取角色对应的索引
	int getIndexOfCharacter(const string& chartletName);
	Character* getPlayerOfID(int id);
	//添加角色
	void addCharacter(Character* character);
	//是否与角色发生碰撞
	bool isCollidedWithCharacter(const Rect& rect);
};
#endif