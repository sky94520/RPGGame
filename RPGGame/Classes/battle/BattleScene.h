#ifndef __BattleScene_H__
#define __BattleScene_H__
#include <map>
#include <string>
#include <vector>
#include <cstring>
#include <unordered_map>
#include <SDL_Engine/SDL_Engine.h>

using namespace std;
using namespace SDL;
using namespace SDL::ui;

class Good;
struct Turn;
class BattleLayer;
class BattleResult;
class BattlePanelLayer;

class BattleDelegate
{
public:
	virtual ~BattleDelegate() {}
	virtual void setVisibilityOfActionBtns(bool visibility) = 0;
	virtual void setVisibilityOfUndoBtn(bool visibility) = 0;
	virtual void battleResult(bool victory) = 0;
};
class BattleScene : public Scene
{
public:
	BattleScene();
	~BattleScene();
	CREATE_FUNC(BattleScene);
	bool init();

	void startBattle(const unordered_map<string, int>& enemyData);

	void update(float dt);
	//设置战斗背景
	void setBattleBack(const string& back, int index);
	void updateStateOfTurn(Turn* turn);
	bool onTouchBegan(Touch* touch, SDL_Event* event);
	//清除函数
	void clear();
private:
	Sprite* m_battleBacks[2];//战斗背景
	BattleLayer* m_pBattleLayer; //战斗层
	BattlePanelLayer* m_pPanelLayer; //ui层
	BattleResult* m_pBattleResult;//战斗结果层
};
#endif