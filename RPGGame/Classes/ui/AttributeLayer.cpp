#include "AttributeLayer.h"

#include "../GameScene.h"
#include "../GameMacros.h"

#include "../entity/Character.h"

#include "../data/StaticData.h"
#include "../data/DynamicData.h"
#include "../data/CharacterData.h"

AttributeLayer::AttributeLayer()
	:m_pStatusNode(nullptr)
	,m_pPlayerGroup(nullptr)
	,m_pToggleBtn(nullptr)
{
}

AttributeLayer::~AttributeLayer()
{
}

AttributeLayer* AttributeLayer::create(const string& xmlPath, const ccWidgetClickCallback& callback)
{
	auto layer = new AttributeLayer();
	if (layer && layer->init(xmlPath, callback))
		layer->autorelease();
	else
		SDL_SAFE_DELETE(layer);
	return layer;
}

bool AttributeLayer::init(const string& xmlPath, const ccWidgetClickCallback& callback)
{
	auto manager = ui::UIWidgetManager::getInstance();
	m_pStatusNode = manager->createWidgetsWithXml(xmlPath);
	this->addChild(m_pStatusNode);
	//物品 技能切换按钮
	m_pToggleBtn = m_pStatusNode->getChildByName<Button*>("toggle_btn");
	m_pToggleBtn->addClickEventListener(callback);
	this->initializeUI(m_pStatusNode);

	this->updateLabelOfDeltaProp(Properties());
	return true;
}

Character* AttributeLayer::getSelectedPlayer() const
{
	if (m_pPlayerGroup == nullptr)
		return nullptr;
	auto radioBtn = m_pPlayerGroup->getSelectedButton();
	Character* player = nullptr;

	if (radioBtn != nullptr)
		player = static_cast<Character*>(radioBtn->getUserObject());

	return player;
}

void AttributeLayer::updatePlayerGroup()
{
	vector<Character*> players = GameScene::getInstance()->getCharacterList();
	auto callback = SDL_CALLBACK_2(AttributeLayer::updateRadioButton, this);
	GoodLayer::updateRadioButtons<Character*>(m_pPlayerGroup, players, callback);
}

void AttributeLayer::changeToggleBtnFrame(const string& spriteFrame)
{
	m_pToggleBtn->loadNormalImage(spriteFrame);
}

void AttributeLayer::updateLabelOfProp(Character* player)
{
	auto dynamicData = DynamicData::getInstance();
	auto chartletName = player->getChartletName();
	//获取属性
	auto propStruct = dynamicData->getTotalProperties(chartletName);
	//更新
	this->updatePropLabels(chartletName, propStruct, true);
}

void AttributeLayer::updateLabelOfDeltaProp(const Properties& deltaProp)
{
	this->updatePropLabels("", deltaProp, false);
}

void AttributeLayer::lockPlayer(int uniqueID)
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

void AttributeLayer::unlockPlayer()
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

void AttributeLayer::updateRadioButton(RadioButton* radioBtn, Character* player)
{
	bool ret = (player != nullptr);
	radioBtn->setUserObject(player);
	radioBtn->setVisible(ret);
	radioBtn->setTouchEnabled(ret);
	if (!ret)
		return;
	//设置精灵
	auto sprite = radioBtn->getChildByName<Sprite*>("sprite");
	auto characterData = StaticData::getInstance()->getCharacterData();
	auto animation = characterData->getWalkingAnimation(player->getChartletName(), Direction::Down);
	auto spriteFrame = animation->getFrames().at(1)->getSpriteFrame();

	sprite->setSpriteFrame(spriteFrame);
}


void AttributeLayer::initializeUI(Node* pXmlNode)
{
	//人物单选
	m_pPlayerGroup = ui::RadioButtonGroup::create();
	m_pPlayerGroup->addEventListener(SDL_CALLBACK_3(AttributeLayer::selectPlayer, this));

	this->addChild(m_pPlayerGroup);
	//获取列表
	auto& children = pXmlNode->getChildByName("bag_player_list")->getChildren();
	//添加人物单选按钮
	for (unsigned i = 0; i < children.size(); i++)
	{
		auto radioBtn = static_cast<RadioButton*>(children.at(i));
		m_pPlayerGroup->addRadioButton(radioBtn);
	}
	//获取各种属性
	auto propertyNode = pXmlNode->getChildByName("bag_player_property");
	const Json::Value& props = StaticData::getInstance()->getValueForKey("prop_array");

	for (const auto&value : props)
	{
		auto key1 = value.asString();
		auto label1 = propertyNode->getChildByName<LabelBMFont*>(key1);
		m_propLabels.push_back(label1);
		//获取delta属性
		auto key2 = StringUtils::format("delta-%s", key1.c_str());
		auto label2 = propertyNode->getChildByName<LabelBMFont*>(key2);
		m_deltaPropLabels.push_back(label2);
	}
}

void AttributeLayer::selectPlayer(RadioButton* radioBtn, int index, RadioButtonGroup::EventType)
{
	//获取选中的角色
	Character* player = this->getSelectedPlayer();
	if (player == nullptr) {
		LOG("BagLayer selectPlayer not bind player");
		return;
	}
	auto chartletName = player->getChartletName();
	//设置立绘
	auto spriteFrame = StaticData::getInstance()->getCharacterData()->getFaceSpriteFrame(chartletName);
	m_pStatusNode->getChildByName<Sprite*>("face")->setSpriteFrame(spriteFrame);
	//更新名称
	string name = player->getChartletName();
	m_pStatusNode->getChildByName<LabelBMFont*>("player_name")->setString(name);
	//更新属性值
	this->updateLabelOfProp(player);
	//更新经验值
	this->updateShownOfExp();
	/*
	//如果是处于技能下，才更新
	if (m_type == Type::Skill)
	{
		//this->updateShownOfGoods();
	}
	*/
}

void AttributeLayer::updatePropLabels(const string& chartletName, const Properties& properties, bool bPlayerProperties)
{
	auto dynamicData = DynamicData::getInstance();

	vector<LabelBMFont*>* labels = nullptr;

	labels = bPlayerProperties ? &m_propLabels : &m_deltaPropLabels;

	vector<int> props;

	props.push_back(properties.hp);
	props.push_back(properties.mp);
	props.push_back(properties.attack);
	props.push_back(properties.defense);
	props.push_back(properties.magicAttack);
	props.push_back(properties.magicDefense);
	props.push_back(properties.agility);
	props.push_back(properties.luck);

	for (unsigned i = 0; i < props.size(); i++)
	{
		auto label = labels->at(i);
		auto prop = props.at(i);
		string text;

		//主角属性值
		if (bPlayerProperties && (i == 0 || i == 1))
		{
			int value = (i == 0 ? dynamicData->getMaxHitPoint(chartletName)
				: dynamicData->getMaxManaPoint(chartletName));

			text = StringUtils::format("%d/%d", prop, value);
		}//属性差值
		else if (!bPlayerProperties)
		{
			auto operate = prop > 0 ? '+' : '-';
			text = StringUtils::format("%c%d", operate, prop);
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

void AttributeLayer::updateShownOfExp()
{
	auto player = this->getSelectedPlayer();

	if (player == nullptr)
		return;
	auto chartletName = player->getChartletName();

	//更新经验值 等级上限
	auto level = DynamicData::getInstance()->getLevel(chartletName);
	CharacterData* characterData = StaticData::getInstance()->getCharacterData();
	auto &nextLvSt = characterData->getDataByLevel(chartletName, level + 1);

	auto exp = DynamicData::getInstance()->getExp(chartletName);
	auto format = STATIC_DATA_STRING("level_exp_format");
	auto text = StringUtils::format(format.c_str(), level, exp, nextLvSt.exp);

	m_pStatusNode->getChildByName<LabelBMFont*>("exp")->setString(text);
}
