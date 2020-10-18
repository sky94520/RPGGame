#include "BagLayer.h"
#include "AttributeLayer.h"

#include "../entity/Good.h"
#include "../entity/Character.h"

#include "../data/StaticData.h"
#include "../data/DynamicData.h"
#include "../data/CharacterData.h"


BagLayer::BagLayer()
	:m_type(Type::None)
	,m_nCurPage(1)
	,m_pGoodLayer(nullptr)
	,m_pAttributeLayer(nullptr)
	,m_bVisible(false)
{
}

BagLayer::~BagLayer()
{
}

bool BagLayer::init()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	
	m_pAttributeLayer = AttributeLayer::create("scene/bag/bag_layer.xml", SDL_CALLBACK_1(BagLayer::toggle, this));
	this->addChild(m_pAttributeLayer);
	//物品层
	m_pGoodLayer = GoodLayer::create();
	this->addChild(m_pGoodLayer);
	//隐藏
	m_pGoodLayer->setPositionY(-visibleSize.height);
	m_pAttributeLayer->setPositionY(-visibleSize.height);

	return true;
}

void BagLayer::setVisibleofBagLayer(bool visible)
{
	if (m_bVisible == visible)
		return;
	m_bVisible = visible;
	//显示动作
	const int tag = 1;
	Size visibleSize = Director::getInstance()->getVisibleSize();
	ActionInterval* action = nullptr;
	//出现
	if (visible)
	{
		MoveTo* move = MoveBy::create(0.5f, Point(0.f, visibleSize.height));
		action = EaseExponentialOut::create(move);
		//更新玩家组
		if (m_type != Type::SeedBag) 
		{
			m_pAttributeLayer->updatePlayerGroup();
		}
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
	
	auto newAction = action->clone();
	newAction->setTag(tag);
	m_pAttributeLayer->stopActionByTag(tag);
	m_pAttributeLayer->runAction(newAction);
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

void BagLayer::toggle(Object* sender)
{
	if (m_type == Type::ShopBuy || m_type == Type::ShopSell || m_type == Type::SeedBag)
		return;

	if (m_type == Type::Bag)
	{
		this->setType(Type::Skill);
	}
	else if (m_type == Type::Skill)
	{
		this->setType(Type::Bag);
	}
}

void BagLayer::pageBtnCallback(GoodLayer* pGoodLayer, int delta)
{
	switch (m_type)
	{
		case Type::Bag:
		{
			const vector<Good*>& goodList = DynamicData::getInstance()->getBagGoodList();
			this->showGoodLayer("bag_title_txt1.png", "use_text.png", "equip_text.png", goodList, m_nCurPage+delta);

			m_pAttributeLayer->changeToggleBtnFrame("sbt2_1.png");
		}break;
		case Type::Skill:
		{
			Character* player = m_pAttributeLayer->getSelectedPlayer();
			string name = player->getChartletName();
			const vector<Good*>& goodList = DynamicData::getInstance()->getSkills(name);
			this->showGoodLayer("bag_title_txt2.png", "use_text.png", "", goodList, m_nCurPage + delta);

			m_pAttributeLayer->changeToggleBtnFrame("sbt2_2.png");
		}
		break;
		default:
			LOG("BagLayer::setType the type has not handled\n");
	}
}

void BagLayer::updateGoodHook(LabelAtlas* pCostLabel, LabelAtlas* pNumberLabel, int cost, int number)
{
	pNumberLabel->setString(StringUtils::toString(number));
	//价钱的更新受到当前的出售比例和打开类型有关
	if (m_type == Type::Skill)
	{
		pCostLabel->setVisible(false);
	}
	else
	{
		auto ratio = DynamicData::getInstance()->getSellRatio();
		//在出售时才会有出售的比率
		if (m_type == Type::ShopSell)
			cost = int(cost * ratio);

		pCostLabel->setVisible(true);
		pCostLabel->setString(StringUtils::toString(cost));
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

bool BagLayer::touchOutsideCallback(GoodLayer* goodLayer)
{
	//TODO:点击了外面，暂时不处理
	return false;
}

void BagLayer::showGoodLayer(const string& titleFrameName, const string& useBtnFrameName
	, const string& equipBtnFrameName, const vector<Good*>& vec, int curPage)
{
	this->setVisibleofBagLayer(true);
	//设置title
	m_pGoodLayer->updateShowingTitle(titleFrameName);
	//设置使用按钮
	m_pGoodLayer->updateShowingBtn(BtnType::Use, BtnParamSt(true, true, useBtnFrameName));
	//隐藏装备按钮
	if (equipBtnFrameName.empty())
		m_pGoodLayer->updateShowingBtn(BtnType::Equip, BtnParamSt(false, false));
	else
	m_pGoodLayer->updateShowingBtn(BtnType::Use, BtnParamSt(true, true, equipBtnFrameName));

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
