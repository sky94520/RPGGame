#ifndef __BagLayer_H__
#define __BagLayer_H__

#include <vector>
#include <SDL_Engine/SDL_Engine.h>

#include "GoodLayer.h"

using namespace std;
using namespace SDL;

class Good;
class Character;

class BagLayer : public Layer
{
public:
	/*物品层显示类型 待修改*/
	enum class Type
	{
		None,
		Bag, //背包
		Skill,//技能
		Shop, //商店
		SeedBag, //种子背包
	};
public:
	BagLayer();
	virtual ~BagLayer();
	CREATE_FUNC(BagLayer);
	bool init();
	void setVisibleofBagLayer(bool visible);
	void setDelegate(GoodLayerDelegate* pDelegate) { m_pGoodLayer->setDelegate(pDelegate); }
	void setType(Type type);

	void pageBtnCallback(GoodLayer* goodLayer, int value);
	void useBtnCallback(GoodLayer* goodLayer);
	void equipBtnCallback(GoodLayer* goodLayer);
	void closeBtnCallback(GoodLayer* goodLayer);
	void selectGoodCallback(GoodLayer* goodLayer, GoodInterface* good);
private:
	void initializeUI(Node* pXmlNode);
	void selectPlayer(RadioButton*, int, RadioButtonGroup::EventType);
	Character* getSelectedPlayer() const;
	void showGoodLayer(const string& titleFrameName, const string& btnFrameName, const vector<Good*>& vec, int curPage);
private:
	Type m_type;//类型(玩家背包、商店等)
	unsigned int m_nCurPage;//当前页
	GoodLayer* m_pGoodLayer;
	Node* m_pStatusLayer;//状态节点
	ui::RadioButtonGroup* m_pPlayerGroup;//玩家组
};

#endif