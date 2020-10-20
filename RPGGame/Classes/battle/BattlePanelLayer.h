#ifndef __BattlePanelLayer_H__
#define __BattlePanelLayer_H__

#include <string>
#include <SDL_Engine/SDL_Engine.h>

using namespace std;
using namespace SDL;
using namespace SDL::ui;

class Good;

class BattlePanelLayer : public Layer
{
public:
	enum class ClickedType
	{
		None,
		Attack,
		Good,
	};
public:
	BattlePanelLayer();
	~BattlePanelLayer();
	static BattlePanelLayer* create(const string& xmlPath);
	bool init(const string& xmlPath);
	//是否显示行动按钮
	bool isVisibileOfActionBtns() const { return m_bVisibileOfActionBtn; }
	void setVisibileOfActionBtns(bool visibility);
	//是否显示撤销按钮
	bool isVisibleOfUndoBtn() const { return m_bVisibleOfUndoBtn; }
	void setVisibileOfUndoBtn(bool visibility);
	//是否显示对应的状态面板
	void setVisibileOfStatePanel(int index, bool visibility);
	//更新角色状态
	void updateStateOfPlayer(int i, const string& sName, int hp, int maxHp, int mp, int maxMp, SpriteFrame* frame);
	//设置物品
	void setGood(Good* good);
	Good* getGood();
	//当前点击的按钮的类型
	ClickedType getClickedType() const { return m_clickedType; }
	void setClickedType(ClickedType type) { m_clickedType = type; }
private:
	void attackBtnCallback(Object* sender);
	void magicBtnCallback(Object* sender);
	void goodBtnCallback(Object* sender);
	void guardBtnCallback(Object* sender);
	void escapeBtnCallback(Object* sender);
	void clickUndoBtnCallback(Object* sender);
private:
	Node* m_pXmlNode;
	//玩家状态层
	Node* m_pStateNode;
	//操作按钮显示
	bool m_bVisibileOfActionBtn;
	bool m_bVisibleOfUndoBtn;
	//上次点击的按钮
	ClickedType m_clickedType;
	//待使用的物品
	Good* m_pGood;
};

#endif