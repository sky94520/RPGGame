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
		pNumberLabel->setVisible(false);
	}
	else
	{
		auto ratio = DynamicData::getInstance()->getSellRatio();
		//在出售时才会有出售的比率
		if (m_type == Type::ShopSell)
			cost = int(cost * ratio);

		pNumberLabel->setVisible(true);
		pCostLabel->setVisible(true);
		pCostLabel->setString(StringUtils::toString(cost));
	}
}

void BagLayer::useBtnCallback(GoodLayer* goodLayer)
{
}

void BagLayer::equipBtnCallback(GoodLayer* goodLayer)
{
	auto dynamicData = DynamicData::getInstance();
	//获取当前选中的装备
	RadioButton* selectedBtn = m_pGoodLayer->getSelectedButton();
	Good* good = static_cast<Good*>(selectedBtn->getUserObject());
	int goodNum = good->getNumber();
	auto equipmentType = good->getEquipmentType();
	//获取当前选中角色
	auto player = m_pAttributeLayer->getSelectedPlayer();
	auto chartletName = player->getChartletName();
	//获取当前对应部件的装备
	auto oldGood = dynamicData->getEquipment(chartletName, equipmentType);
	bool isStackable = false;
	bool bSuccess = false;
	//可叠加
	if (oldGood != nullptr
		&& good->getPrototype() == oldGood->getPrototype()
		&& equipmentType == EquipmentType::OffHand
		&& oldGood->getOffHandType() == OffHandType::Arrow)
	{
		isStackable = true;
	}
	SDL_SAFE_RETAIN(oldGood);
	//卸下装备 此时oldGood == good
	if (nullptr != oldGood)
	{
		//卸下一个当前的装备到物品栏中
		if (isStackable && good == oldGood)
		{
			dynamicData->splitEquipment(chartletName, equipmentType, oldGood, 1);
			bSuccess = true;
		}
		else if (!isStackable)
		{
			//添加装备到背包中
			dynamicData->addGood(oldGood->getPrototype(), oldGood->getNumber());
			dynamicData->unequip(chartletName, equipmentType);
			bSuccess = true;
		}
	}
	//进行装备
	if (good != oldGood)
	{
		//副手存在且为弓箭 则表示可以叠加
		if (isStackable)
		{
			dynamicData->overlyingEquipment(chartletName, oldGood, 1);
		}
		else
		{
			dynamicData->equip(chartletName, player->getUniqueID(), Good::create(good->getPrototype()));
		}
		bSuccess = true;
		//从背包中移除一个项
		dynamicData->removeGood(good->getPrototype(), 1);
	}
	SDL_SAFE_RELEASE(oldGood);
	//更新属性
	m_pAttributeLayer->updateLabelOfProp(player);
	this->pageBtnCallback(m_pGoodLayer, 0);
	//重新选中，只有在当前的物品个数大于1时才会主动刷新delta属性值
	if (goodNum > 1)
	{
		m_pGoodLayer->reselectedButton(selectedBtn);
	}
	//操作成功才会播放音效
	if (bSuccess)
	{
		SoundManager::getInstance()->playEffect(STATIC_DATA_STRING("equip_se"), 0);
	}
}

void BagLayer::closeBtnCallback(GoodLayer* goodLayer)
{
	this->setVisibleofBagLayer(false);
}

void BagLayer::selectGoodCallback(GoodLayer* goodLayer, GoodInterface* item)
{
	Good* good = static_cast<Good*>(item);
	auto dynamicData = DynamicData::getInstance();
	auto goodType = good->getGoodType();
	//是否可以装备
	bool bEquipTouchEnable = false;
	//是否不可点击
	bool bUseBtnEnabled = true;
	string equipFrameName;

	Properties propStruct = good->getProperties();
	Character* player = m_pAttributeLayer->getSelectedPlayer();
	//如果物品不可装备
	if (goodType == GoodType::Item)
	{
		bEquipTouchEnable = false;
	}//当选中装备时，和当前装备进行比较，获取差值
	else if (goodType == GoodType::Equipment)
	{
		bEquipTouchEnable = true;
		Good* oldEquip = nullptr;
		//获取装备属性
		if (player != nullptr)
		{
			auto chartletName = player->getChartletName();
			oldEquip = dynamicData->getEquipment(chartletName, good->getEquipmentType());
		}
		//更改按钮显示 装备 or 卸下
		//点击了已经装备的武器，显示卸下 并设置装备属性相反
		if (good == oldEquip)
		{
			equipFrameName = STATIC_DATA_STRING("unequip_frame_name");

			propStruct = Properties() - propStruct;
			//当前为购买商品且选中了装备栏，则购买按钮不可点击
			bUseBtnEnabled = m_type == Type::ShopBuy ? false : true;
		}
		else//获取装备之间的差值
		{
			equipFrameName = STATIC_DATA_STRING("equip_frame_name");

			if (oldEquip != nullptr)
				propStruct -= oldEquip->getProperties();
		}
	}
	//设置装备按钮
	m_pGoodLayer->updateShowingBtn(BtnType::Equip, BtnParamSt(bEquipTouchEnable, bEquipTouchEnable, equipFrameName));
	//更新属性差值
	m_pAttributeLayer->updateLabelOfDeltaProp(propStruct);
	//当前为购买商品且选中了装备栏，则购买按钮不可点击
	m_pGoodLayer->updateShowingBtn(BtnType::Use, BtnParamSt(bUseBtnEnabled, bUseBtnEnabled, ""));
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
		m_pGoodLayer->updateShowingBtn(BtnType::Equip, BtnParamSt(true, true, equipBtnFrameName));

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

	//TODO切片处理
	vector<GoodInterface*> content(8);
	int i = 0;
	for (i = 0; i < 4; i++)
	{
		int index = (m_nCurPage - 1) * 4 + i;

		if (index >= size)
			break;
		Good* good = vec.at(index);
		content[i] = dynamic_cast<GoodInterface*>(good);
	}
	//更新装备
	auto player = m_pAttributeLayer->getSelectedPlayer();
	auto dynamicData = DynamicData::getInstance();

	for (; i < content.size() && player != nullptr; i++)
	{
		auto equipmentType = static_cast<EquipmentType>(i - 4);
		content[i] = dynamicData->getEquipment(player->getChartletName(), equipmentType);
	}
	//更新页码并填充物品
	m_pGoodLayer->updateShowingPage(m_nCurPage, totalPage);
	m_pGoodLayer->updateShowingGoods(content);
}
