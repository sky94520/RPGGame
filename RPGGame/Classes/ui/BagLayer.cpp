#include "BagLayer.h"
#include "GoodLayer.h"

#include "../entity/GoodInterface.h"
#include "../data/DynamicData.h"


BagLayer::BagLayer()
	:m_type(Type::None)
	,m_nCurPage(1)
	,m_pGoodLayer(nullptr)
{
}

BagLayer::~BagLayer()
{
}

bool BagLayer::init()
{
	m_pGoodLayer = GoodLayer::create();
	this->addChild(m_pGoodLayer);

	return true;
}

void BagLayer::pageBtnCallback(GoodLayer* pGoodLayer, int value)
{
	vector<Good*>* pBagGoodList = nullptr;
	/*
	if (m_type == Type::Shop)
		pBagGoodList = &m_shopList;
	else*/
	if (m_type == Type::Warehouse)
		pBagGoodList = DynamicData::getInstance()->getBagGoodList();

	//总页码
	int size = 0;
	size = pBagGoodList->size();
	int totalPage = size / 4;
	if (size % 4 != 0)
		totalPage += 1;

	m_nCurPage += value;
	//越界处理
	if (m_nCurPage <= 0)
		m_nCurPage = totalPage;
	else if (m_nCurPage > totalPage)
		m_nCurPage = 1;
	//切片处理
	vector<GoodInterface*> content;
	for (int i = 0; i < 4; i++)
	{
		int index = (m_nCurPage - 1) * 4 + i;

		if (index >= size)
			break;
		content.push_back((GoodInterface*)pBagGoodList->at(index));
	}
	//更新页码和物品填充
	pGoodLayer->updateShowingPage(m_nCurPage, totalPage);
	pGoodLayer->updateShowingGoods(content);
}

void BagLayer::useBtnCallback(GoodLayer* goodLayer)
{

}

void BagLayer::equipBtnCallback(GoodLayer* goodLayer)
{
}

void BagLayer::closeBtnCallback(GoodLayer* goodLayer)
{
}

void BagLayer::selectGoodCallback(GoodLayer* goodLayer, GoodInterface* good)
{
}
