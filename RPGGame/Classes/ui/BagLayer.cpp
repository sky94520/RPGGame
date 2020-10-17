#include "BagLayer.h"
#include "GoodLayer.h"

#include "../entity/Good.h"
#include "../data/DynamicData.h"



BagLayer::BagLayer()
	:m_type(Type::None)
	,m_nCurPage(1)
	,m_pGoodLayer(nullptr)
	,m_pStatusLayer(nullptr)
{
}

BagLayer::~BagLayer()
{
}

bool BagLayer::init()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto manager = ui::UIWidgetManager::getInstance();
	m_pStatusLayer = manager->createWidgetsWithXml("scene/bag/bag_layer.xml");
	this->addChild(m_pStatusLayer);
	//物品层
	m_pGoodLayer = GoodLayer::create();
	//m_pGoodLayer->setPosition(200.f, 0.f);
	this->addChild(m_pGoodLayer);
	//隐藏
	m_pGoodLayer->setPositionY(-visibleSize.height);
	m_pStatusLayer->setPositionY(-visibleSize.height);

	return true;
}

void BagLayer::setVisibleofBagLayer(bool visible)
{
	//显示动作
	const int tag = 1;
	Size visibleSize = Director::getInstance()->getVisibleSize();
	ActionInterval* action = nullptr;
	//出现
	if (visible)
	{
		MoveTo* move = MoveBy::create(0.5f, Point(0, 0));
		action = EaseExponentialOut::create(move);
	}
	else
	{
		MoveTo* move = MoveBy::create(0.5f, Point(0, -visibleSize.height));
		action = EaseExponentialIn::create(move);
	}
	action->setTag(tag);
	m_pGoodLayer->setShowing(visible);
	//停止原先动画并开始新动画
	m_pGoodLayer->stopActionByTag(tag);
	m_pGoodLayer->runAction(action);
}

void BagLayer::setType(Type type)
{
	if (m_type == type)
		return;
	m_type = type;
	//切换，则从第一页开始展示
	m_nCurPage = 1;
	this->pageBtnCallback(m_pGoodLayer, 0);
}

void BagLayer::pageBtnCallback(GoodLayer* pGoodLayer, int delta)
{
	switch (m_type)
	{
		case Type::Warehouse:
		{
			const vector<Good*>& goodList = DynamicData::getInstance()->getBagGoodList();
			this->showGoodLayer("bag_title_txt1.png", "use_text.png", goodList, m_nCurPage+delta);
		}
		break;
		default:
			LOG("BagLayer::setType the type has not handled\n");
	}
}

void BagLayer::useBtnCallback(GoodLayer* goodLayer)
{

}

void BagLayer::equipBtnCallback(GoodLayer* goodLayer)
{
}

void BagLayer::closeBtnCallback(GoodLayer* goodLayer)
{
	this->setVisibleofBagLayer(false);
}

void BagLayer::selectGoodCallback(GoodLayer* goodLayer, GoodInterface* good)
{
}

void BagLayer::showGoodLayer(const string& titleFrameName, const string& btnFrameName
	, const vector<Good*>& vec, int curPage)
{
	this->setVisibleofBagLayer(true);
	//设置title
	m_pGoodLayer->updateShowingTitle(titleFrameName);
	//设置使用按钮
	m_pGoodLayer->updateShowingBtn(BtnType::Use, BtnParamSt(true, true, btnFrameName));
	//隐藏装备按钮
	m_pGoodLayer->updateShowingBtn(BtnType::Equip, BtnParamSt(false, false));
	//更新页码
	int size = vec.size();
	unsigned int totalPage = size / 4;
	if (size % 4 != 0)
		totalPage += 1;

	if (totalPage == 0)
		totalPage = 1;

	//保证页面合法
	m_nCurPage = curPage;
	if (m_nCurPage > totalPage)
		m_nCurPage--;
	if (m_nCurPage <= 0)
		m_nCurPage = 1;

	//切片处理
	vector<GoodInterface*> content;
	for (int i = 0; i < 4; i++)
	{
		int index = (m_nCurPage - 1) * 4 + i;

		if (index >= size)
			break;
		Good* good = vec.at(index);
		content.push_back(dynamic_cast<GoodInterface*>(good));
	}
	//更新页码并填充物品
	m_pGoodLayer->updateShowingPage(m_nCurPage, totalPage);
	m_pGoodLayer->updateShowingGoods(content);
}
