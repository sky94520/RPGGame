#ifndef __BagLayer_H__
#define __BagLayer_H__

#include <SDL_Engine/SDL_Engine.h>

#include "GoodLayer.h"

using namespace std;
using namespace SDL;

class BagLayer : public Layer
{
public:
	/*物品层显示类型 待修改*/
	enum class Type
	{
		None,
		Warehouse, //仓库
		Shop, //商店
		SeedBag, //种子背包
	};
public:
	BagLayer();
	virtual ~BagLayer();
	CREATE_FUNC(BagLayer);
	bool init();
	void setDelegate(GoodLayerDelegate* pDelegate) { m_pGoodLayer->setDelegate(pDelegate); }
	void setVisibleofBagLayer(bool visible);
	/**
	 * 点击上一页按钮回调函数
	 * @param goodLayer 对应的物品层
	 * @param value 下一页+1 上一页-1
	 */
	void pageBtnCallback(GoodLayer* goodLayer, int value);
	/**
	 * 使用按钮回调函数
	 * @param goodLayer 对应的物品层
	 */
	void useBtnCallback(GoodLayer* goodLayer);
	/**
	 * 装备按钮回调函数
	 * @param goodLayer 对应的物品层
	 */
	void equipBtnCallback(GoodLayer* goodLayer);
	/**
	 * 关闭按钮回调函数
	 * @param goodLayer 对应的物品层
	 */
	void closeBtnCallback(GoodLayer* goodLayer);
	/**
	 * 选中物品回调函数
	 * @param goodLayer 对应的物品层
	 * @param good 对应的物品
	 */
	void selectGoodCallback(GoodLayer* goodLayer, GoodInterface* good);
private:
	Type m_type;//类型(玩家背包、商店等)
	unsigned int m_nCurPage;//当前页
	GoodLayer* m_pGoodLayer;
};

#endif