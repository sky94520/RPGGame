#include "HpBar.h"
#include "Actor.h"
#include "Enemy.h"
#include "Fighter.h"
#include "BattleLayer.h"
#include "BattleScene.h"
#include "PriorityQueue.h"

#include "../GameScene.h"
#include "../data/StaticData.h"
#include "../script/LuaStack.h"
#include "../ui/GoodLayer.h"

BattleLayer::BattleLayer()
	:m_pCurInitialFighter(nullptr)
	,m_bRoundOver(true)
	,m_pPriorityQueue(nullptr)
{
	memset(m_battleBacks, NULL, sizeof(m_battleBacks));
}

BattleLayer::~BattleLayer()
{
}

bool BattleLayer::init()
{
	m_pPriorityQueue = PriorityQueue::create();
	this->addChild(m_pPriorityQueue);

	return true;
}

void BattleLayer::update(float dt)
{
	//一回合还没结束，直接退出
	if (!m_bRoundOver)
	{
		//弹出队列头，并重新插入TODO
		return;
	}
	//获取队列头
	auto top = m_pPriorityQueue->getTop();
	//int n = lua_gettop(GameScene::getInstance()->getLuaStack()->getLuaState());
	//等待动作完成
	if (top->fighter->getActionByTag(Fighter::ACTION_TAG) != nullptr)
		return;
	m_bRoundOver = false;
	//是敌人或是友军，则执行对应的AI
	if (top->type == TurnType::Enemy || top->type == TurnType::Friend)
	{
		top->fighter->execute(nullptr);
	}
	else
	{
		//改变主角状态
		auto player = static_cast<Actor*>(top->fighter);
		player->ready();
		//发送事件
		auto eventDispatcher = Director::getInstance()->getEventDispatcher();
		eventDispatcher->dispatchCustomEvent(BATTLE_PLAYER_OPERATION, this);
	}
}

void BattleLayer::addOur(Actor* actor)
{
	//获得位置
	const Json::Value& arr = StaticData::getInstance()->getValueForKey("battle_our_pos_arr");
	int nOurNumber = m_pPriorityQueue->getOurNumber();
	auto pos = PointFromString(arr[nOurNumber].asString());
	//设置位置
	actor->setPosition(pos);
	//添加到层中
	this->addChild(actor);
	//插入到循环队列中
	m_pPriorityQueue->insertTurnOrderly(TurnType::Player, actor);
}

void BattleLayer::addEnemy(const string& name, int number)
{
	Enemy* enemy = nullptr;
	//获得位置
	const Json::Value& arr = StaticData::getInstance()->getValueForKey("battle_enemy_pos_arr");
	Point pos;

	for (int i = 0; i < number; i++)
	{
		enemy = Enemy::create(name);
		m_pCurInitialFighter = enemy;
		//调用脚本的初始化函数
		enemy->initialize();
		//设置位置
		int nEnemyNumber = m_pPriorityQueue->getEnemyNumber();
		pos = PointFromString(arr[nEnemyNumber].asString());
		auto size = enemy->getContentSize();

		enemy->setPosition(pos);
		this->addChild(enemy);
		//设置label
		auto label = LabelBMFont::create(enemy->getFighterName(), "fonts/1.fnt", 300, Color3B(0, 0, 0));
		label->setPosition(Point(size.width / 2, size.height + label->getContentSize().height * 0.5f));
		enemy->addChild(label);
		//创建hp
		int hp = enemy->getProperty(PropertyType::Hp);
		int maxHp = enemy->getMaxHitPoint();
		auto hpBar = HpBar::create("hpBar1.png", "hpBarBG.png", maxHp);
		hpBar->setPosition(Point(size.width / 2, -hpBar->getContentSize().height * 0.5f));
		enemy->addChild(hpBar);
		//创建
		auto fighterChar = enemy->getTurnChar();

		auto atlas = LabelAtlas::create(StringUtils::toString(fighterChar), "AtlasAlpha.png", 18, 24, 'A');
		this->addChild(atlas);
		//插入
		m_pPriorityQueue->insertTurnOrderly(TurnType::Enemy, enemy, label, hpBar, atlas);

		m_pCurInitialFighter = nullptr;
	}
}

Fighter* BattleLayer::getFighterByID(int uniqueID)
{
	auto turn = m_pPriorityQueue->getTurnByID(uniqueID);
	Fighter* fighter = nullptr;

	if (turn != nullptr)
		fighter = turn->fighter;
	else if (m_pCurInitialFighter->getFighterID() == uniqueID)
		fighter = m_pCurInitialFighter;

	return fighter;
}

Turn* BattleLayer::getTopTurn()
{
	return m_pPriorityQueue->getTop();
}

Turn* BattleLayer::getClickedTurn(const Point& pos)
{
	return m_pPriorityQueue->getClickedTurn(pos);
}

void BattleLayer::roundOver()
{
	m_bRoundOver = true;
	m_pPriorityQueue->roundOver();
}

vector<string> BattleLayer::endBattle()
{
	return m_pPriorityQueue->endBattle();
}

void BattleLayer::fighterDead(Fighter* fighter)
{
	m_pPriorityQueue->fighterDead(fighter);
}

int BattleLayer::getOurNumber() const
{
	return m_pPriorityQueue->getOurNumber();
}

int BattleLayer::getEnemyNumber() const
{
	return m_pPriorityQueue->getEnemyNumber();
}

void BattleLayer::setBattleBack(const string& back, int index)
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

void BattleLayer::clear()
{
	m_pPriorityQueue->clear();
}

