#ifndef __BattleScene_H__
#define __BattleScene_H__
#include <map>
#include <string>
#include <cstring>
#include "SDL_Engine/SDL_Engine.h"

using namespace std;
USING_NS_SDL;
using namespace SDL::ui;
struct Turn;
class BattleLayer;
class Good;

class BattleDelegate
{
public:
	virtual ~BattleDelegate(){}
	virtual void setVisibilityOfActionBtns(bool visibility) = 0;
	virtual void setVisibilityOfUndoBtn(bool visibility) = 0;
	virtual void battleResult(bool victory) = 0;
};
enum class ClickButton
{
	None,
	Attack,
	Good,
};
class BattleScene : public Scene, public BattleDelegate
{
	SDL_SYNTHESIZE_READONLY(BattleLayer*, m_pBattleLayer, BattleLayer);//战斗层
private:
	//战斗背景
	Sprite* m_battleBacks[2];

	Node* m_pXmlNode;
	//战斗结果层
	Node* m_pResultLayer;
	//玩家状态层
	Node* m_pStateLayer;
	//操作按钮显示
	bool m_bVisibilityOfBtns;
	//战斗结算面板显示
	bool m_bVisibilityOfResult;
	//上次点击的按钮
	ClickButton m_clickBtn;
	//待使用的物品
	Good* m_pGood;
	//战斗结束标识
	bool m_bBattleOver;
	//战斗胜利的奖励
	int m_nExp;
	int m_nGold;
	map<string, int> m_dropGoods;
public:
	BattleScene();
	~BattleScene();
	CREATE_FUNC(BattleScene);
	bool init();

	void update(float dt);
	//设置战斗背景
	void setBattleBack(const string& back, int index);
	//是否显示行动按钮
	void setVisibilityOfActionBtns(bool visibility);
	//是否显示撤销按钮
	void setVisibilityOfUndoBtn(bool visibility);
	//是否显示对应的状态面板
	void setVisibilityOfStatePanel(int index, bool visibility);
	//是否显示战斗结算面板
	void setVisibilityOfResultPanel(bool var);
	//更新角色状态
	void updateStateOfPlayer(int i, const string& sName, int hp, int maxHp, int mp, int maxMp, SpriteFrame* frame);
	void updateStateOfTurn(Turn* turn);
	bool onTouchBegan(Touch* touch, SDL_Event* event);
	//设置物品
	void setGood(Good* good);
	Good* getGood();
	//战斗结算
	void battleResult(bool victory);
	//设置战斗是否结束
	void setBattleOver(bool var);
	//清除函数
	void clear();
	//添加奖励
	void addReward(int exp, int gold);
	void addReward(const string& name, int number);
private:
	void attackCallback(Object* sender);
	void magicCallback(Object* sender);
	void goodCallback(Object* sender);
	void guardCallback(Object* sender);
	void escapeCallback(Object* sender);
	void undoCallback(Object* sender);
	//战斗胜利
	void battleWin(LabelAtlas* goldLabel, LabelAtlas* expLabel, LabelBMFont* dropLabel);
	//战斗失败
	void battleFailure(LabelAtlas* goldLabel, LabelAtlas* expLabel, LabelBMFont* dropLabel);
};
#endif