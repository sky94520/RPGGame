#ifndef __BagLayer_H__
#define __BagLayer_H__

#include <SDL_Engine/SDL_Engine.h>

#include "GoodLayer.h"

using namespace std;
using namespace SDL;

class BagLayer : public Layer, GoodLayerDelegate
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
public:
	/**
	 * 点击上一页按钮回调函数
	 * @param goodLayer 对应的物品层
	 * @param value 下一页+1 上一页-1
	 */
	virtual void pageBtnCallback(GoodLayer* goodLayer, int value);
	/**
	 * 使用按钮回调函数
	 * @param goodLayer 对应的物品层
	 */
	virtual void useBtnCallback(GoodLayer* goodLayer);
	/**
	 * 装备按钮回调函数
	 * @param goodLayer 对应的物品层
	 */
	virtual void equipBtnCallback(GoodLayer* goodLayer);
	/**
	 * 关闭按钮回调函数
	 * @param goodLayer 对应的物品层
	 */
	virtual void closeBtnCallback(GoodLayer* goodLayer);
	/**
	 * 选中物品回调函数
	 * @param goodLayer 对应的物品层
	 * @param good 对应的物品
	 */
	virtual void selectGoodCallback(GoodLayer* goodLayer, GoodInterface* good);
private:
	Type m_type;//类型(玩家背包、商店等)
	unsigned int m_nCurPage;//当前页
	GoodLayer* m_pGoodLayer;
};

#endif