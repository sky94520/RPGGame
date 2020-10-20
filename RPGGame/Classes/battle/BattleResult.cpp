#include "BattleResult.h"

#include "../entity/Good.h"

#include "../data/StaticData.h"
#include "../data/DynamicData.h"

BattleResult::BattleResult()
	:m_bBattleOver(true)
	,m_nExp(0)
	,m_nGold(0)
	,m_bVisibility(false)
	,m_pXmlNode(nullptr)
{
}

BattleResult::~BattleResult()
{
}

BattleResult* BattleResult::create(const string& xmlPath)
{
	auto node = new BattleResult();
	if (node && node->init(xmlPath))
		node->autorelease();
	else
		SDL_SAFE_DELETE(node);
	return node;
}

bool BattleResult::init(const string& xmlPath)
{
	//获取战斗结果节点，并隐藏
	m_pXmlNode = UIWidgetManager::getInstance()->createWidgetsWithXml(xmlPath);
	this->addChild(m_pXmlNode);
	//m_pResultLayer = m_pXmlNode->getChildByName("battle_result_layer");
	//m_pResultLayer->setPositionY(-visibleSize.height);
	return true;
}

void BattleResult::showSummary(int victory)
{
	//获取战斗对应图片
	const Json::Value& array = StaticData::getInstance()->getValueForKey("battle_result_sp");
	string resultStr = array[victory].asString();
	//战斗结果图片 成功/失败
	auto resultSprite = m_pXmlNode->getChildByName<Sprite*>("battle_result_sp");
	resultSprite->setSpriteFrame(resultStr);
	//显示结算面板
	this->setVisible(true);
	//战斗结束
	this->setBattleOver(true);
	//获取有用控件
	auto goldLabel = m_pXmlNode->getChildByName<LabelAtlas*>("gold_label");
	auto expLabel =  m_pXmlNode->getChildByName<LabelAtlas*>("exp_label");
	auto dropLabel = m_pXmlNode->getChildByName<LabelBMFont*>("drop_label");

	if (victory)
	{
		this->battleWin(goldLabel, expLabel, dropLabel);
	}
	else
	{
		this->battleFailure(goldLabel, expLabel, dropLabel);
	}
}

void BattleResult::clear()
{
	m_nExp = 0;
	m_nGold = 0;
	m_dropGoods.clear();
}

void BattleResult::setBattleOver(bool var)
{
	m_bBattleOver = var;
}

void BattleResult::setVisible(bool var)
{
	if (m_bVisibility == var)
		return;
	Size visibleSize = Director::getInstance()->getVisibleSize();
	//出现/隐藏动作
	m_bVisibility = var;
	float height = m_bVisibility ? visibleSize.height : -visibleSize.height;
	//运行动作
	MoveBy* move = MoveBy::create(0.5f, Point(0, height));
	Action* action = nullptr;

	if (m_bVisibility)
		action = EaseExponentialOut::create(move);
	else
		action = EaseExponentialIn::create(move);

	this->runAction(action);
}

void BattleResult::addReward(int exp, int gold)
{
	m_nExp += exp;
	m_nGold += gold;
}

void BattleResult::addReward(const string& name, int number)
{
	//是否发现
	auto it = m_dropGoods.find(name);

	if (it != m_dropGoods.end())
	{
		it->second += number;
	}
	else
	{
		m_dropGoods[name] = number;
	}
}

void BattleResult::battleWin(LabelAtlas* goldLabel, LabelAtlas* expLabel, LabelBMFont* dropLabel)
{
	//增加金币 和经验
	goldLabel->setString(StringUtils::toString(m_nGold));
	expLabel->setString(StringUtils::toString(m_nExp));

	//添加掉落物品TODO
	stringstream stream;
	int length = 0;
	int i = 0;

	for (auto it = m_dropGoods.begin(); it != m_dropGoods.end(); it++, i++)
	{
		auto goodName = it->first;
		auto number = it->second;
		//添加物品
		auto good = DynamicData::getInstance()->addGood(goodName, number);
		auto name = good->getName();

		stream << name << "*" << number;
		//当为最后一个元素时，不再添加逗号
		if (i + 1 != m_dropGoods.size())
			stream << ",";
		length += name.size();
		//当作换行使用TODO
		if (length >= 30)
		{
			stream << " ";
		}
	}
	if (!stream.str().empty())
	{
		dropLabel->setString(stream.str());
	}
	else
	{
		auto text = STATIC_DATA_STRING("battle_result_default_text");
		dropLabel->setString(text);
	}
	//胜利音效
	SoundManager::getInstance()->playEffect(STATIC_DATA_STRING("battle_victory"), 0);
}

void BattleResult::battleFailure(LabelAtlas* goldLabel, LabelAtlas* expLabel, LabelBMFont* dropLabel)
{
	//惩罚条件 TODO 暂无
	goldLabel->setString(StringUtils::toString("0"));
	expLabel->setString(StringUtils::toString("0"));

	auto text = STATIC_DATA_STRING("battle_result_default_text");
	dropLabel->setString(text);
	//失败音效
	SoundManager::getInstance()->playEffect(STATIC_DATA_STRING("battle_defeat"), 0);
}
