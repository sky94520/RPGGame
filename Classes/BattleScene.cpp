#include "BattleScene.h"
#include "BattleLayer.h"
#include "Fighter.h"
#include "DynamicData.h"
#include "Actor.h"
#include "GameScene.h"
#include "PlayerLayer.h"
#include "HpBar.h"
#include "BattleLayer.h"
#include "GoodLayer.h"
#include "Good.h"
#include "StaticData.h"

BattleScene::BattleScene()
	:m_pBattleLayer(nullptr)
	,m_pXmlNode(nullptr)
	,m_pResultLayer(nullptr)
	,m_pStateLayer(nullptr)
	,m_bVisibilityOfBtns(true)
	,m_bVisibilityOfResult(false)
	,m_clickBtn(ClickButton::None)
	,m_pGood(nullptr)
	,m_bBattleOver(true)
	,m_nExp(0)
	,m_nGold(0)
{
	memset(m_battleBacks, NULL, sizeof(m_battleBacks));
}

BattleScene::~BattleScene()
{
}

bool BattleScene::init()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	//加载xml
	m_pXmlNode = UIWidgetManager::getInstance()->createWidgetsWithXml("scene/battle_scene.xml");
	this->addChild(m_pXmlNode);
	//获取战斗结果节点，并隐藏
	m_pResultLayer = m_pXmlNode->getChildByName("battle_result_layer");
	m_pResultLayer->setPositionY(-visibleSize.height);
	//状态节点
	m_pStateLayer = m_pXmlNode->getChildByName("battle_state_layer");
	//行动按钮绑定
	auto actionNode = m_pXmlNode->getChildByName("battle_action_layer");

	auto attackBtn = actionNode->getChildByName<ui::Button*>("attack_btn");
	auto magicBtn = actionNode->getChildByName<ui::Button*>("magic_btn");
	auto goodBtn = actionNode->getChildByName<ui::Button*>("good_btn");
	auto guardBtn = actionNode->getChildByName<ui::Button*>("guard_btn");
	auto escapeBtn = actionNode->getChildByName<ui::Button*>("escape_btn");

	attackBtn->addClickEventListener(SDL_CALLBACK_1(BattleScene::attackCallback, this));
	magicBtn->addClickEventListener(SDL_CALLBACK_1(BattleScene::magicCallback, this));
	goodBtn->addClickEventListener(SDL_CALLBACK_1(BattleScene::goodCallback, this));
	guardBtn->addClickEventListener(SDL_CALLBACK_1(BattleScene::guardCallback, this));
	escapeBtn->addClickEventListener(SDL_CALLBACK_1(BattleScene::escapeCallback, this));

	m_pXmlNode->getChildByName<ui::Button*>("undo_btn")->addClickEventListener(SDL_CALLBACK_1(BattleScene::undoCallback, this));

	//战斗层
	m_pBattleLayer = BattleLayer::create();
	m_pBattleLayer->setDelegate(this);
	this->addChild(m_pBattleLayer);

	return true;
}

void BattleScene::update(float dt)
{
	//只有战斗未结束，才进行出手更新
	if (!m_bBattleOver)
	{
		m_pBattleLayer->update(dt);
	}
}

void BattleScene::setBattleBack(const string& back, int index)
{
	if (m_battleBacks[index] == nullptr)
	{
		m_battleBacks[index] = Sprite::create(back);
		this->addChild(m_battleBacks[index], -1);

		Size visibleSize = Director::getInstance()->getVisibleSize();
		m_battleBacks[index]->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	}
	else
	{
		m_battleBacks[index]->setTexture(back);
	}
}

void BattleScene::setVisibilityOfActionBtns(bool visibility)
{
	if (m_bVisibilityOfBtns != visibility)
	{
		Point pos;
		auto node = m_pXmlNode->getChildByName("battle_action_layer");
		m_bVisibilityOfBtns = visibility;

		pos.x += m_bVisibilityOfBtns ?  0.f: 100.f;
		//行动按钮
		MoveTo* move = MoveTo::create(0.8f, pos);
		EaseExponentialOut*action = EaseExponentialOut::create(move);
		action->setTag(1);

		node->stopActionByTag(1);
		node->runAction(action);
	}
}

void BattleScene::setVisibilityOfUndoBtn(bool visibility)
{
	auto undoBtn = m_pXmlNode->getChildByName<ui::Button*>("undo_btn");

	undoBtn->setTouchEnabled(visibility);
	undoBtn->setVisible(visibility);
}

void BattleScene::setVisibilityOfStatePanel(int index, bool visibility)
{
	auto node = m_pStateLayer->getChildByTag(index);
	node->setVisible(visibility);
}

void BattleScene::setVisibilityOfResultPanel(bool var)
{
	if (m_bVisibilityOfResult == var)
		return ;
	Size visibleSize = Director::getInstance()->getVisibleSize();
	//出现/隐藏动作
	m_bVisibilityOfResult = var;
	float height = m_bVisibilityOfResult ? visibleSize.height : -visibleSize.height;
	//运行动作
	MoveBy* move = MoveBy::create(0.5f, Point(0, height));
	Action* action = nullptr;

	if (m_bVisibilityOfResult)
		action = EaseExponentialOut::create(move);
	else
		action = EaseExponentialIn::create(move);
	
	m_pResultLayer->runAction(action);
}

void BattleScene::updateStateOfPlayer(int i, const string& sName, int hp, int maxHp, int mp, int maxMp, SpriteFrame* frame)
{
	auto node = m_pStateLayer->getChildByTag(i);

	LabelBMFont* nameLabel = node->getChildByName<LabelBMFont*>("name");
	LoadingBar* hpLoadingBar = node->getChildByName<LoadingBar*>("hp");
	LoadingBar* mpLoadingBar = node->getChildByName<LoadingBar*>("mp");
	LabelAtlas* hpLabel = node->getChildByName<LabelAtlas*>("hp_label");
	LabelAtlas* mpLabel = node->getChildByName<LabelAtlas*>("mp_label");

	Sprite* sprite = node->getChildByName<Sprite*>("face");
	//设置名字
	nameLabel->setString(sName);
	//设置数值
	hpLoadingBar->setPercent((float)hp / maxHp);
	mpLoadingBar->setPercent((float)mp / maxMp);

	auto hpStr = StringUtils::format("%d/%d", hp, maxHp);
	auto mpStr = StringUtils::format("%d/%d", mp, maxMp);

	hpLabel->setString(hpStr);
	mpLabel->setString(mpStr);
	//设置贴图
	if (frame != nullptr)
	{
		sprite->setSpriteFrame(frame);
	}
}

void BattleScene::updateStateOfTurn(Turn* turn)
{
	auto fighter = turn->fighter;
	int hp = fighter->getProperty(PropertyType::Hp);

	//是主角或是友军
	if (turn->type == TurnType::Friend || turn->type == TurnType::Player)
	{
		int maxHp = fighter->getMaxHitPoint();
		int mp = fighter->getProperty(PropertyType::Mp);
		int maxMp = fighter->getMaxManaPoint();
		auto sName = fighter->getFighterName();

		auto playerLayer = GameScene::getInstance()->getPlayerLayer();
		int index = playerLayer->getIndexOfCharacter(fighter->getChartletName());

		this->updateStateOfPlayer(index, sName, hp, maxHp, mp, maxMp, nullptr);
	}//是敌人，仅进行血量更新
	else if (turn->type == TurnType::Enemy)
	{
		if (turn->hpBar != nullptr)
			turn->hpBar->setValue((float)hp);
	}
}

bool BattleScene::onTouchBegan(Touch* touch, SDL_Event* event)
{
	//结算面版显示，表示战斗结束
	if (m_bVisibilityOfResult)
	{
		this->setVisibilityOfResultPanel(false);
		//战斗结束
		GameScene::getInstance()->endBattle();
	}
	if (m_clickBtn == ClickButton::None)
		return false;

	auto pos = touch->getLocation();
	Turn* turn = m_pBattleLayer->getClickedTurn(pos);;
	bool bSuccess = false;

	if (turn == nullptr)
		return false;

	auto player = m_pBattleLayer->getTopTurn()->fighter;
	//点击到活着的敌人才会进行攻击
	if (m_clickBtn == ClickButton::Attack 
		&& turn != nullptr && turn->type == TurnType::Enemy
		&& turn->fighter->getProperty(PropertyType::Hp) > 0)
	{
		//主角攻击
		player->execute(turn->fighter);

		bSuccess = true;
		m_clickBtn = ClickButton::None;
	}//使用道具
	else if (m_clickBtn == ClickButton::Good && m_pGood != nullptr)
	{
		//获取使用范围
		auto usageRange = m_pGood->getUsageRange();
		//只能是自己
		if ((usageRange == UsageRange::SingleAlly && turn->type != TurnType::Enemy)
			|| (usageRange == UsageRange::SingleEnemy && turn->type == TurnType::Enemy))
		{
			auto userId = player->getFighterID();
			auto targetId = turn->fighter->getFighterID();

			m_pGood->execute(userId, targetId);
			//使用道具动画
			player->good(m_pGood);

			SDL_SAFE_RELEASE_NULL(m_pGood);
			bSuccess = true;

			SoundManager::getInstance()->playEffect(STATIC_DATA_STRING("use_good_se"), 0);
		}
		m_clickBtn = ClickButton::None;
	}

	if (bSuccess)
	{
		this->setVisibilityOfUndoBtn(false);
	}
	return true;
}

void BattleScene::setGood(Good* good)
{
	SDL_SAFE_RETAIN(good);
	SDL_SAFE_RELEASE_NULL(m_pGood);
	m_pGood = good;
}

Good* BattleScene::getGood()
{
	return m_pGood;
}

void BattleScene::battleResult(bool victory)
{
	//获取战斗对应图片
	auto& array = STATIC_DATA_ARRAY("battle_result_sp");
	string resultStr = array.at((int)victory).asString();
	//设置图片
	auto resultSp = m_pResultLayer->getChildByName<Sprite*>("battle_result_sp");
	resultSp->setSpriteFrame(resultStr);
	//显示结算面板
	this->setVisibilityOfResultPanel(true);
	//战斗结束
	this->setBattleOver(true);
	//获取有用控件
	auto goldLabel = m_pResultLayer->getChildByName<LabelAtlas*>("gold_label");
	auto expLabel = m_pResultLayer->getChildByName<LabelAtlas*>("exp_label");
	auto dropLabel = m_pResultLayer->getChildByName<LabelBMFont*>("drop_label");

	if (victory)
	{
		this->battleWin(goldLabel,expLabel,dropLabel);
	}
	else
	{
		this->battleFailure(goldLabel,expLabel,dropLabel);
	}
}

void BattleScene::setBattleOver(bool var)
{
	m_bBattleOver = var;
}

void BattleScene::clear()
{
	m_pBattleLayer->clear();
}

void BattleScene::addReward(int exp, int gold)
{
	m_nExp += exp;
	m_nGold += gold;
}

void BattleScene::addReward(const string& name, int number)
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

void BattleScene::attackCallback(Object* sender)
{
	//清空
	m_nExp = 0;
	m_nGold = 0;
	m_dropGoods.clear();
	//隐藏行动按钮，出现撤销按钮
	this->setVisibilityOfActionBtns(false);
	this->setVisibilityOfUndoBtn(true);

	m_clickBtn = ClickButton::Attack;
}

void BattleScene::magicCallback(Object* sender)
{
	//隐藏行动按钮，出现撤销按钮
	this->setVisibilityOfActionBtns(false);
	//打开背包，并锁定角色
	auto goodLayer = GameScene::getInstance()->getGoodLayer();
	auto curTurn = m_pBattleLayer->getTopTurn();
	
	goodLayer->show(GoodLayer::Type::Skill);
	goodLayer->lockPlayer(curTurn->fighter->getFighterID());

	m_clickBtn = ClickButton::Good;
}

void BattleScene::goodCallback(Object* sender)
{
	//隐藏行动按钮，出现撤销按钮
	this->setVisibilityOfActionBtns(false);
	//打开背包，并锁定角色
	auto goodLayer = GameScene::getInstance()->getGoodLayer();
	auto curTurn = m_pBattleLayer->getTopTurn();
	
	goodLayer->show(GoodLayer::Type::Bag);
	goodLayer->lockPlayer(curTurn->fighter->getFighterID());

	m_clickBtn = ClickButton::Good;
}

void BattleScene::guardCallback(Object* sender)
{
	//隐藏行动按钮
	this->setVisibilityOfActionBtns(false);
	//获取当前的turn
	auto turn = m_pBattleLayer->getTopTurn();
	//防御
	turn->fighter->guard();
	//回合结束
	m_pBattleLayer->roundOver();
}

void BattleScene::escapeCallback(Object* sender)
{
	//隐藏行动按钮
	this->setVisibilityOfActionBtns(false);
	//主角逃跑
	m_pBattleLayer->getTopTurn()->fighter->escape();
	//结束战斗
	m_pBattleLayer->endBattle();
}

void BattleScene::undoCallback(Object* sender)
{
	//隐藏行动按钮
	this->setVisibilityOfActionBtns(true);
	//存在物品，则释放引用
	if (m_pGood != nullptr)
	{
		SDL_SAFE_RELEASE_NULL(m_pGood);
	}
}

void BattleScene::battleWin(LabelAtlas* goldLabel, LabelAtlas* expLabel, LabelBMFont* dropLabel)
{
	//增加金币 和经验
	goldLabel->setString(StringUtils::toString(m_nGold));
	expLabel->setString(StringUtils::toString(m_nExp));

	auto gameScene = GameScene::getInstance();
	gameScene->addGold(m_nGold);
	gameScene->addExp(m_nExp);
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

		stream<<name<<"*"<<number;
		//当为最后一个元素时，不再添加逗号
		if (i + 1 != m_dropGoods.size())
			stream<<",";
		length += name.size();
		//当作换行使用TODO
		if (length >= 30)
		{
			stream<<" ";
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

void BattleScene::battleFailure(LabelAtlas* goldLabel, LabelAtlas* expLabel, LabelBMFont* dropLabel)
{
	//惩罚条件 TODO 暂无
	goldLabel->setString(StringUtils::toString("0"));
	expLabel->setString(StringUtils::toString("0"));

	auto text = STATIC_DATA_STRING("battle_result_default_text");
	dropLabel->setString(text);
	//失败音效
	SoundManager::getInstance()->playEffect(STATIC_DATA_STRING("battle_defeat"), 0);
}