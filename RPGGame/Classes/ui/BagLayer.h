#ifndef __BagLayer_H__
#define __BagLayer_H__

#include <vector>
#include <SDL_Engine/SDL_Engine.h>
#include "GoodLayer.h"

using namespace std;
using namespace SDL;

class Good;
class Character;
class GoodLayer;
class AttributeLayer;

class BagLayer : public Layer
{
public:
	/*物品层显示类型 待修改*/
	enum class Type
	{
		None,
		Bag, //背包
		Skill,//技能
		ShopBuy, //商店购买
		ShopSell,//商店出售
		SeedBag, //种子背包
	};
public:
	BagLayer();
	virtual ~BagLayer();
	CREATE_FUNC(BagLayer);
	bool init();
	void setVisibleofBagLayer(bool visible);
	bool isShowing() const { return m_bVisible; }
	void setDelegate(GoodLayerDelegate* pDelegate) { m_pGoodLayer->setDelegate(pDelegate); }
	void setType(Type type);
	//物品 技能切换回调函数
	void toggle(Object* sender);

	virtual void pageBtnCallback(GoodLayer* goodLayer, int value);
	virtual void updateGoodHook(LabelAtlas* pCostLabel, LabelAtlas* pNumberLabel, int cost, int number);
	virtual void useBtnCallback(GoodLayer* goodLayer);
	virtual void equipBtnCallback(GoodLayer* goodLayer);
	virtual void closeBtnCallback(GoodLayer* goodLayer);
	virtual void selectGoodCallback(GoodLayer* goodLayer, GoodInterface* good);
	virtual bool touchOutsideCallback(GoodLayer* goodLayer);
private:
	void showGoodLayer(const string& titleFrameName, const string& useBtnFrameName, const string& equipBtnFrameName, const vector<Good*>& vec, int curPage);
private:
	Type m_type;//类型(玩家背包、商店等)
	unsigned int m_nCurPage;//当前页
	GoodLayer* m_pGoodLayer;
	AttributeLayer* m_pAttributeLayer;
	bool m_bVisible; //是否显示
};

#endif