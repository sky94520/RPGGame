#include "GoodLayer.h"
#include "GameScene.h"
#include "PlayerLayer.h"
#include "Character.h"
#include "StaticData.h"
#include "DynamicData.h"
#include "Good.h"

GoodLayer::GoodLayer()
	:m_bOpened(false)
	,m_pPlayerGroup(nullptr)
	,m_pGoodGroup(nullptr)
	,m_pXmlNode(nullptr)
	,m_nCurPage(0)
	,m_pUseBtn(nullptr)
	,m_pEquipBtn(nullptr)
	,m_pToggleBtn(nullptr)
	,m_pSelectedBtn(nullptr)
	,m_elapsed(0.f)
	,m_type(Type::Bag)
	,m_bOnlyShowingBag(false)
{
	m_goods = vector<Good*>(8);
}

GoodLayer::~GoodLayer()
{
}

bool GoodLayer::init()
{
	//加载外部文件
	this->initializeUI();

	this->updateLabelOfDeltaProp(PropertyStruct());

	return true;
}

void GoodLayer::update(float dt)
{
	//进行赞时间
/*	if (m_pSelectedBtn == m_pEquipBtn)
	{
		m_elapsed += dt;

		if (m_elapsed > 0.15f)
		{
			m_elapsed -= 0.15f;
			this->equipOrUnequip(m_pSelectedBtn);
		}
	}*/
}

void GoodLayer::show(Type type)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	//显示选择界面
	MoveTo* move = MoveTo::create(0.5f,Point(0.f, 0.f));
	EaseExponentialOut*action = EaseExponentialOut::create(move);
	
	action->setTag(1);

	this->stopActionByTag(1);
	this->runAction(action);

	m_bOpened = true;
	int number = 8;

	if (m_bOnlyShowingBag)
	{
		number = 4;
	}
	else
	{

		//更新人物
		vector<Character*>& players = GameScene::getInstance()->getPlayerLayer()->getCharacterList();
		this->updateRadioButtons(m_pPlayerGroup, players, GOOD_NUMBER_PER_PAGE);
	}
	//切换类型
	this->changeType(type);
	//更新显示
	this->updateShownOfGoods(number);
}

void GoodLayer::hide()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	//动画显示选择界面
	MoveTo* move = MoveTo::create(0.5f,Point(0, -visibleSize.height));
	EaseExponentialIn* action = EaseExponentialIn::create(move);
	action->setTag(1);
	//停止原先动画并开始新动画
	this->stopActionByTag(1);
	this->runAction(action);

	m_bOpened = false;
}

void GoodLayer::updateShownOfGoods(int number)
{
	this->updatePageIndex();
	//更新物品
	auto& goods = this->getContent();

	this->updateRadioButtons(m_pGoodGroup, m_goods, number);
}

void GoodLayer::updateShownOfExp()
{
	auto player = this->getSelectedPlayer();

	if (player == nullptr)
		return;
	auto chartletName = player->getChartletName();

	//更新经验值 等级上限
	auto level = DynamicData::getInstance()->getLevel(chartletName);
	auto &nextLvSt = StaticData::getInstance()->getDataByLevel(chartletName, level + 1);

	auto exp = DynamicData::getInstance()->getExp(chartletName);
	auto format = STATIC_DATA_STRING("level_exp_format");
	auto text = StringUtils::format(format.c_str(),level,exp,nextLvSt.exp);

	m_pXmlNode->getChildByName<LabelBMFont*>("exp")->setString(text);
}

void GoodLayer::updateShownOfGold(int number)
{
	auto text = StringUtils::toString(number);
	m_pXmlNode->getChildByName<LabelAtlas*>("gold_label")->setString(text);
}

void GoodLayer::updateShownOfProps()
{
	auto player = this->getSelectedPlayer();

	if (player != nullptr)
		this->updateLabelOfProp(player);
}

bool GoodLayer::isOpened() const
{
	return m_bOpened;
}

Character* GoodLayer::getSelectedPlayer() const
{
	if (m_pPlayerGroup == nullptr)
		return nullptr;

	auto radioBtn = m_pPlayerGroup->getSelectedButton();
	Character* player = nullptr;

	if (radioBtn != nullptr)
		player = static_cast<Character*>(radioBtn->getUserObject());
	
	return player;
}

void GoodLayer::lockPlayer(int uniqueID)
{
	auto& list = m_pPlayerGroup->getRadioButtonList();

	for (auto radioBtn : list)
	{
		auto player = static_cast<Character*>(radioBtn->getUserObject());
		//没有绑定角色，则跳过
		if (player == nullptr)
			continue;

		if (player->getUniqueID() == uniqueID)
		{
			radioBtn->setTouchEnabled(true);
			m_pPlayerGroup->setSelectedButton(radioBtn);
		}
		else
		{
			radioBtn->setTouchEnabled(false);
		}
	}
}

void GoodLayer::unlockPlayer()
{
	if (m_pPlayerGroup == nullptr)
		return;
	auto& list = m_pPlayerGroup->getRadioButtonList();

	for (auto radioBtn : list)
	{
		auto player = static_cast<Character*>(radioBtn->getUserObject());
		//没有绑定角色，则跳过
		if (player == nullptr)
			continue;
		//设置按钮可选择
		radioBtn->setTouchEnabled(true);
	}
}

void GoodLayer::initializeUI()
{
	m_pXmlNode = ui::UIWidgetManager::getInstance()->createWidgetsWithXml("scene/bag_layer.xml");

	this->addChild(m_pXmlNode);
	//关闭按钮回调
	auto closeBtn = m_pXmlNode->getChildByName<ui::Button*>("close_btn");
	closeBtn->addClickEventListener(SDL_CALLBACK_1(GoodLayer::closeBag,this));
	//页面切换
	auto pPrePageBtn = m_pXmlNode->getChildByName<ui::Button*>("pre_page_btn");
	auto pNextPageBtn = m_pXmlNode->getChildByName<ui::Button*>("next_page_btn");
	//绑定页数按钮
	pPrePageBtn->addClickEventListener(SDL_CALLBACK_1(GoodLayer::turnPage, this, false));
	pNextPageBtn->addClickEventListener(SDL_CALLBACK_1(GoodLayer::turnPage, this, true));
	//使用按钮
	m_pUseBtn = m_pXmlNode->getChildByName<Button*>("use_btn");
	m_pUseBtn->addClickEventListener(SDL_CALLBACK_1(GoodLayer::useGood, this));
	//装备按钮
	m_pEquipBtn = m_pXmlNode->getChildByName<Button*>("equip_btn");
	m_pEquipBtn->addTouchEventListener(SDL_CALLBACK_2(GoodLayer::equipOrUnequipCallback, this));
	//m_pEquipBtn->addClickEventListener(SDL_CALLBACK_1(GoodLayer::equipOrUnequip, this));
	//切换按钮
	m_pToggleBtn = m_pXmlNode->getChildByName<Button*>("toggle_btn");
	m_pToggleBtn->addClickEventListener(SDL_CALLBACK_1(GoodLayer::toggle, this));
	//获取各种属性 获取属性结点
	auto propertyNode = m_pXmlNode->getChildByName("bag_player_property");
	auto props = STATIC_DATA_ARRAY("prop_array");

	for (const auto&value : props)
	{
		auto key1 = value.asString();
		auto label1 = propertyNode->getChildByName<LabelBMFont*>(key1);

		m_propLabels.push_back(label1);
		//获取差值属性
		auto key2 = StringUtils::format("delta-%s",key1.c_str());
		auto label2 = propertyNode->getChildByName<LabelBMFont*>(key2);

		m_deltaPropLabels.push_back(label2);
	}
	//初始化背包按键
	m_pGoodGroup = ui::RadioButtonGroup::create();
	m_pGoodGroup->addEventListener(SDL_CALLBACK_3(GoodLayer::selectGood, this));
	this->addChild(m_pGoodGroup);

	auto &children1 = m_pXmlNode->getChildByName("bag_good_list")->getChildren();
	//添加物品单选按钮
	for (unsigned i = 0;i < children1.size();i++)
	{
		auto radioBtn = static_cast<RadioButton*>(children1.at(i));
		m_pGoodGroup->addRadioButton(radioBtn);
	}
	//人物单选
	m_pPlayerGroup = RadioButtonGroup::create();
	m_pPlayerGroup->addEventListener(SDL_CALLBACK_3(GoodLayer::selectPlayer,this));

	this->addChild(m_pPlayerGroup);
	//获取列表
	auto& children2 = m_pXmlNode->getChildByName("bag_player_list")->getChildren();
	//添加人物单选按钮
	for (unsigned i = 0;i < children2.size();i++)
	{
		auto radioBtn = static_cast<RadioButton*>(children2.at(i));
		m_pPlayerGroup->addRadioButton(radioBtn);
	}
}

void GoodLayer::updatePageIndex()
{
	auto &goods = this->getContent(1);
	//获取总页数
	int nTotalPage = goods.size() / GOOD_NUMBER_PER_PAGE;
	nTotalPage += (goods.size() % GOOD_NUMBER_PER_PAGE == 0 ? 0 : 1);
	//超过页面
	if (m_nCurPage >= nTotalPage)
	{
		m_nCurPage = 0;
	}
	else if (m_nCurPage < 0)
	{
		m_nCurPage = nTotalPage - 1;
	}
	//更新label
	auto text = StringUtils::format("%d/%d", m_nCurPage + 1, nTotalPage);

	auto label = m_pXmlNode->getChildByName<LabelAtlas*>("page_label");
	label->setString(text);
}

bool GoodLayer::changeType(Type type)
{
	if (m_type == type)
		return false;
	m_type = type;

	auto& useBtnArray = STATIC_DATA_ARRAY("use_btn_array");
	int useIndex = static_cast<int>(type);
	string useFrameName = useBtnArray.at(useIndex).asString();
	string title;
	//是否显示装备按钮
	bool bVisible = false;
	//不同的类型显示不同
	if (m_type == Type::Bag)
	{
		bVisible = true;
		title = "bag_title_txt1.png";

		m_pToggleBtn->loadNormalImage("sbt2_1.png");
	}
	else if (m_type == Type::Skill)
	{
		bVisible = false;
		title = "bag_title_txt2.png";
		m_pToggleBtn->loadNormalImage("sbt2_2.png");
	}
	else if (m_type == Type::ShopBuy)
	{
		title = "bag_title_txt3.png";
		bVisible = false;
	}
	else if (m_type == Type::ShopSale)
	{
		title = "bag_title_txt4.png";
		bVisible = false;
	}
	else if (m_type == Type::Seed)
	{
		title = "bag_title_txt1.png";
		bVisible = false;
	}
	//设置使用
	m_pUseBtn->getChildByName<Sprite*>("sprite")->setSpriteFrame(useFrameName);
	//设置文本
	m_pXmlNode->getChildByName<Sprite*>("title_text")->setSpriteFrame(title);
	//装备按钮 显示or隐藏
	m_pEquipBtn->setTouchEnabled(bVisible);
	m_pEquipBtn->setVisible(bVisible);

	return true;
}

void GoodLayer::closeBag(Object* sender)
{
	GameScene::getInstance()->closeBag();
}

void GoodLayer::selectPlayer(RadioButton*,int,RadioButtonGroup::EventType)
{
	//获取选中的角色
	auto player = this->getSelectedPlayer();
	auto chartletName = player->getChartletName();
	//设置立绘
	auto spriteFrame = StaticData::getInstance()->getFaceSpriteFrame(chartletName);
	m_pXmlNode->getChildByName<Sprite*>("face")->setSpriteFrame(spriteFrame);
	//更新名称
	auto name = player->getChartletName();
	m_pXmlNode->getChildByName<LabelBMFont*>("player_name")->setString(name);
	//更新属性值
	this->updateLabelOfProp(player);
	//更新经验值
	this->updateShownOfExp();
	//如果是处于技能下，才更新
	if (m_type == Type::Skill)
	{
		this->updateShownOfGoods();
	}
}

void GoodLayer::selectGood(RadioButton* radioBtn,int index,RadioButtonGroup::EventType)
{
	//获取按钮对应的good
	auto good = static_cast<Good*>(radioBtn->getUserObject());
	auto goodType = good->getGoodType();
	const auto& desc = good->getDescription();
	//更改描述
	auto descLabel = m_pXmlNode->getChildByName<LabelBMFont*>("good_desc_label");
	descLabel->setString(desc);
	//是否可以装备
	auto dynamicData = DynamicData::getInstance();
	bool bEquipTouchEnable = false;
	//是否不可点击
	bool bUseBtnEnabled = true;

	PropertyStruct propStruct = good->getPropertyStruct();
	auto player = this->getSelectedPlayer();
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
		string equipFrameName;
		//点击了已经装备的武器，显示卸下 并设置装备属性相反
		if (good == oldEquip)
		{
			equipFrameName = STATIC_DATA_STRING("unequip_frame_name");

			propStruct = PropertyStruct() - propStruct;
			//当前为购买商品且选中了装备栏，则购买按钮不可点击
			bUseBtnEnabled = m_type == Type::ShopBuy ? false : true;
		}
		else//获取装备之间的差值
		{
			equipFrameName = STATIC_DATA_STRING("equip_frame_name");

			if (oldEquip != nullptr)
				propStruct -= oldEquip->getPropertyStruct();
		}
		m_pEquipBtn->getChildByName<Sprite*>("sprite")->setSpriteFrame(equipFrameName);
	}
	//设置装备按钮 和更新属性差值
	m_pEquipBtn->setTouchEnabled(bEquipTouchEnable);
	this->updateLabelOfDeltaProp(propStruct);
	//当前为购买商品且选中了装备栏，则购买按钮不可点击
	m_pUseBtn->setTouchEnabled(bUseBtnEnabled);
}

void GoodLayer::turnPage(Object* sender, bool bNext)
{
	m_nCurPage = bNext ? m_nCurPage + 1 : m_nCurPage - 1;
	//更新页面索引
	this->updatePageIndex();	
	auto& goods = this->getContent();

	this->updateRadioButtons(m_pGoodGroup, m_goods, GOOD_NUMBER_PER_PAGE);	
}

void GoodLayer::useGood(Object* sender)
{
	//获取选中的按钮
	auto radioBtn = m_pGoodGroup->getSelectedButton();
	if (radioBtn == nullptr)
		return;

	auto good = static_cast<Good*>(radioBtn->getUserObject());
	auto gameScene = GameScene::getInstance();
	//根据不同类型 =》不同行为
	switch (m_type)
	{
	//使用 在战斗状态下会消耗此次行动
	case GoodLayer::Type::Bag:
	case GoodLayer::Type::Skill:
		gameScene->useGood(good);
		break;
	case GoodLayer::Type::ShopBuy:
		gameScene->buyGood(good);
		break;
	case GoodLayer::Type::ShopSale:
		gameScene->sellGood(good);
		break;
	case Type::Seed:
		//gameScene->sowSeed(good);
		break;
	default:
		break;
	}
}

void GoodLayer::equipOrUnequip(Object* sender)
{
	auto dynamicData = DynamicData::getInstance();
	//获取当前选中的装备
	auto selectedBtn = m_pGoodGroup->getSelectedButton();
	auto good = static_cast<Good*>(selectedBtn->getUserObject());
	auto goodNum = good->getNumber();

	auto equipmentType = good->getEquipmentType();
	//获取当前选中角色
	auto player = this->getSelectedPlayer();
	auto chartletName = player->getChartletName();
	//获取当前对应部件的装备
	auto oldGood = dynamicData->getEquipment(chartletName, equipmentType);
	bool isAdding = false;
	bool bSuccess = false;
	//表示可叠加
	if (oldGood != nullptr 
		&& good->getParentTableName() == oldGood->getParentTableName()
		&& equipmentType == EquipmentType::OffHand
		&& oldGood->getOffHandType() == OffHandType::Arrow)
	{
		isAdding = true;
	}
	SDL_SAFE_RETAIN(oldGood);
	//卸下装备 此时oldGood == good
	if (nullptr != oldGood)
	{
		//可叠加且相等 表示卸下
		if (isAdding && good == oldGood)
		{
			dynamicData->splitEquipment(chartletName, equipmentType, oldGood, 1);
			bSuccess = true;
		}
		else if (!isAdding)
		{
			//添加装备到背包中
			dynamicData->addGood(oldGood->getParentTableName(), oldGood->getNumber());
			dynamicData->unequip(chartletName, equipmentType);
			bSuccess = true;
		}
	}
	//进行装备
	if (good != oldGood)
	{
		//副手存在且为弓箭 则表示可以叠加
		if (isAdding)
		{
			dynamicData->overlyingEquipment(chartletName, oldGood, 1);
		}
		else
		{
			dynamicData->equip(chartletName,player->getUniqueID(),Good::create(good->getParentTableName()));
		}
		bSuccess = true;
		//从背包中移除一个项
		dynamicData->subGood(good->getParentTableName(), 1);
	}
	SDL_SAFE_RELEASE(oldGood);
	//更新属性
	this->updateLabelOfProp(player);
	this->updateShownOfGoods(8);
	//只有在当前的物品个数大于1时才会主动刷新delta属性值
	if (goodNum > 1)
	{
		m_pGoodGroup->unselectedButton();
		m_pGoodGroup->setSelectedButton(selectedBtn);
	}
	//操作成功才会播放音效
	if (bSuccess)
	{
		SoundManager::getInstance()->playEffect(STATIC_DATA_STRING("equip_se"), 0);
	}
}

void GoodLayer::equipOrUnequipCallback(Object* sender,TouchEventType type)
{
	switch (type)
	{
	case SDL::ui::TouchEventType::BEGAN:
		this->equipOrUnequip(sender);
		m_pSelectedBtn = m_pEquipBtn;
		break;
	case SDL::ui::TouchEventType::MOVED:
		break;
	case SDL::ui::TouchEventType::ENDED:
	case SDL::ui::TouchEventType::CANCELLED:
		m_pSelectedBtn = nullptr;
		break;
	default:
		break;
	}
}

void GoodLayer::toggle(Object* sender)
{
	if (m_type == Type::ShopBuy || m_type == Type::ShopSale 
		|| m_type == Type::Seed)
		return;

	if (m_type == Type::Bag)
	{
		this->changeType(Type::Skill);
	}
	else if (m_type == Type::Skill)
	{
		this->changeType(Type::Bag);
	}
	this->updateShownOfGoods(4);
}

void GoodLayer::updateRadioButton(RadioButton* radioBtn,Good* good)
{
	bool ret = (good != nullptr);

	radioBtn->setUserObject(good);
	radioBtn->setVisible(ret);
	radioBtn->setTouchEnabled(ret);

	if (good == nullptr)
		return;

	auto iconID = good->getIconID();
	auto name = good->getName();
	auto number = good->getNumber();
	auto cost = good->getCost();
	//更新radio button的显示
	auto pIconSprite = radioBtn->getChildByName<Sprite*>("icon");
	auto pNameLabel = radioBtn->getChildByName<LabelBMFont*>("name");
	auto pCostLabel = radioBtn->getChildByName<LabelAtlas*>("cost");
	auto pNumberLabel = radioBtn->getChildByName<LabelAtlas*>("number");
	//更新
	pIconSprite->setSpriteFrame(StaticData::getInstance()->getIconSpriteFrame(iconID));
	pNameLabel->setString(name);
	pNumberLabel->setString(StringUtils::toString(number));
	//价钱的更新受到当前的出售比例和打开类型有关
		if (m_type == Type::Bag || m_type == Type::Skill)
	{
		pCostLabel->setVisible(false);
	}
	else
	{
		auto cost = good->getCost();
		auto ratio = DynamicData::getInstance()->getSellRatio();
		//在出售时才会有出售的比率
		if (m_type == Type::ShopSale)
			cost = int(cost * ratio);

		pCostLabel->setVisible(true);
		pCostLabel->setString(StringUtils::toString(cost));
	}
}

void GoodLayer::updateRadioButton(RadioButton* radioBtn,Character* player)
{
	bool ret = (player != nullptr);

	radioBtn->setUserObject(player);
	radioBtn->setVisible(ret);
	radioBtn->setTouchEnabled(ret);

	if (player == nullptr)
		return;
	//设置精灵
	auto sprite = radioBtn->getChildByName<Sprite*>("sprite");
	auto animation = StaticData::getInstance()->getWalkingAnimation(player->getChartletName(), Direction::Down);
	auto spriteFrame = animation->getFrames().at(1)->getSpriteFrame();

	sprite->setSpriteFrame(spriteFrame);
}

void GoodLayer::updateLabelOfProp(Character* player)
{
	auto dynamicData = DynamicData::getInstance();
	auto chartletName = player->getChartletName();
	//获取属性
	auto propStruct = dynamicData->getTotalProperties(chartletName);
	//更新
	this->updatePropLabels(chartletName,propStruct,true);
}

void GoodLayer::updateLabelOfDeltaProp(const PropertyStruct& deltaProp)
{
	this->updatePropLabels("", deltaProp, false);
}

vector<Good*>& GoodLayer::getContent()
{
	//为前四位赋值
	vector<Good*>& goods = this->getContent(1);
	
	int startIndex = m_nCurPage * GOOD_NUMBER_PER_PAGE;
	unsigned i =0;
	for (;i < GOOD_NUMBER_PER_PAGE;i++)
	{
		Good* good = (goods.size() > startIndex + i ? goods[startIndex + i] : nullptr);

		m_goods[i] = good;
	}
	//更新装备
	auto player = this->getSelectedPlayer();
	auto dynamicData = DynamicData::getInstance();
	
	for (;i < m_goods.size() && player != nullptr; i++)
	{
		auto equipmentType = static_cast<EquipmentType>(i - GOOD_NUMBER_PER_PAGE);

		m_goods[i] = dynamicData->getEquipment(player->getChartletName(), equipmentType);
	}
	return goods;
}

vector<Good*>& GoodLayer::getContent(int)
{
	switch (m_type)
	{
	case GoodLayer::Type::Bag:
	case GoodLayer::Type::ShopSale:
		return DynamicData::getInstance()->getBagList();
		break;
	case GoodLayer::Type::Skill:
		{
			auto player = this->getSelectedPlayer();
			auto chartletName = player->getChartletName();

			return DynamicData::getInstance()->getSkills(chartletName);
		}
		break;
	case GoodLayer::Type::ShopBuy:
		return DynamicData::getInstance()->getShopList();
		break;
	case Type::Seed:
		return DynamicData::getInstance()->getSeedList();
		break;
	default:
		break;
	}
	return DynamicData::getInstance()->getBagList();
}

void GoodLayer::updatePropLabels(const string& chartletName, const PropertyStruct& propStruct, bool bPlayerProperties)
{
	auto dynamicData = DynamicData::getInstance();

	vector<LabelBMFont*>* labels = nullptr;

	labels = bPlayerProperties ? &m_propLabels : &m_deltaPropLabels;

	vector<int> props;

	props.push_back(propStruct.hp);
	props.push_back(propStruct.mp);
	props.push_back(propStruct.attack);
	props.push_back(propStruct.defense);
	props.push_back(propStruct.magicAttack);
	props.push_back(propStruct.magicDefense);
	props.push_back(propStruct.agility);
	props.push_back(propStruct.luck);

	for (unsigned i = 0;i < props.size();i++)
	{
		auto label = labels->at(i);
		auto prop = props.at(i);
		string text;

		//主角属性值
		if (bPlayerProperties && (i == 0 || i == 1))
		{
			int value = (i == 0? dynamicData->getMaxHitPoint(chartletName)
				:dynamicData->getMaxManaPoint(chartletName));

			text = StringUtils::format("%d/%d",prop,value);
		}//属性差值
		else if (!bPlayerProperties)
		{
			auto operate = prop > 0? '+':'-';
			text = StringUtils::format("%c%d",operate,prop);
		}
		else
		{
			text = StringUtils::toString(prop);
		}
		label->setString(text);

		if (!bPlayerProperties)
		{
			label->setVisible(prop != 0);
			//设置颜色
			if (prop > 0)
				label->setColor(Color3B(0, 0, 255));
			else if (prop < 0)
				label->setColor(Color3B(255, 0, 0));
		}
	}
}
