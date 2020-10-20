#ifndef __AttributeLayer_H__
#define __AttributeLayer_H__

#include <vector>
#include <string>
#include <functional>
#include <SDL_Engine/SDL_Engine.h>

USING_NS_SDL;
using namespace std;
using namespace SDL::ui;

class Character;
class Properties;

class AttributeLayer : public Layer
{
public:
	AttributeLayer();
	virtual~AttributeLayer();
	static AttributeLayer* create(const string& xmlPath, const ccWidgetClickCallback& callback);
	bool init(const string& xmlPath, const ccWidgetClickCallback& callback);
	//获取当前的player
	Character* getSelectedPlayer()const;
	//更新玩家组
	void updatePlayerGroup();
	void changeToggleBtnFrame(const string& spriteFrame);
	//更新属性面板
	void updateLabelOfProp(Character* player);
	void updateLabelOfDeltaProp(const Properties& deltaProp);
	//锁定角色
	void lockPlayer(int uniqueID);
	void unlockPlayer();
	//更新exp
	void updateShownOfExp();
private:
	void updateRadioButton(RadioButton* radioBtn, Character* player);
	void initializeUI(Node* pXmlNode);
	void selectPlayer(RadioButton*, int, RadioButtonGroup::EventType);
	void updatePropLabels(const string& chartletName, const Properties& properties, bool bPlayerProperties);
private:
	Node* m_pStatusNode;//状态节点
	ui::RadioButtonGroup* m_pPlayerGroup;//玩家组
	Button* m_pToggleBtn;
	//属性标签
	vector<LabelBMFont*> m_propLabels;
	vector<LabelBMFont*> m_deltaPropLabels;
};

#endif