#ifndef __BattleScene_H__
#define __BattleScene_H__
#include <map>
#include <string>
#include <vector>
#include <cstring>
#include <unordered_map>
#include <SDL_Engine/SDL_Engine.h>

#include "BattlePanelLayer.h"

using namespace std;
using namespace SDL;
using namespace SDL::ui;

class Good;
struct Turn;
class Fighter;
class BattleLayer;
class BattleResult;
enum class TurnType;
enum class SearchType;

class BattleScene : public Scene, public ClickButtonDelegate
{
public:
	BattleScene();
	~BattleScene();
	CREATE_FUNC(BattleScene);
	bool init();
	//开始战斗
	void startBattle(const unordered_map<string, int>& enemyData);

	void roundOver();
	void update(float dt);
	void addReward(int exp, int gold);
	void addReward(const string& name, int number);
	//设置战斗背景
	void setBattleBack(const string& back, int index);
	Fighter* getFighterByID(int uniqueID);
	Turn* getTurnByID(int uniqueID);
	Fighter* searchFighter(TurnType turnType, SearchType searchType);
	BattleLayer* getBattleLayer() const { return m_pBattleLayer; }

	void setVisibileOfActionBtns(bool visibile);
	void setVisibileOfUndoBtn(bool visibile);

	Good* getGood()const;
	void setGood(Good* good);

	void updateStateOfTurn(Turn* turn);
	bool onTouchBegan(Touch* touch, SDL_Event* event);
	//清除函数
	void clear();
public:
	virtual void attackBtnCallback();
	virtual void magicBtnCallback();
	virtual void goodBtnCallback();
	virtual void guardBtnCallback();
	virtual void escapeBtnCallback();
	virtual void undoBtnCallback();
private:
	void showOperationBtnCallback(EventCustom* eventCustom);
	void fighterDeadCallback(EventCustom* eventCustom);
private:
	BattleLayer* m_pBattleLayer; //战斗层
	BattlePanelLayer* m_pPanelLayer; //ui层
	BattleResult* m_pBattleResult;//战斗结果层
};
#endif