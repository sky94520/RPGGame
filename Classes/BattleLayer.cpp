#include "BattleLayer.h"
#include "HpBar.h"
#include "Fighter.h"
#include "Actor.h"
#include "StaticData.h"
#include "Enemy.h"
#include "BattleScene.h"
#include "GameScene.h"
#include "GoodLayer.h"

Turn::Turn()
	:type(TurnType::Player)
	,speed(0)
	,fighter(nullptr)
	,hpBar(nullptr)
	,label(nullptr)
	,showingSprite(nullptr)
	,atlas(nullptr)
{
}

Turn::~Turn()
{
	if (this->atlas != nullptr)
	{
		atlas->removeFromParent();
		atlas = nullptr;
	}
	if (this->showingSprite != nullptr)
	{
		showingSprite->removeFromParent();
		showingSprite = nullptr;
	}
	label = nullptr;
	hpBar = nullptr;
	//删除对应的战斗者
	if (fighter != nullptr && Director::getInstance()->isRunning())
	{
		fighter->removeFromParent();
	}
}

void Turn::setVisible(bool visible)
{
	//设置显示
	this->showingSprite->setVisible(visible);

	if (this->type == TurnType::Enemy)
	{
		if (this->hpBar != nullptr)
			hpBar->setVisible(visible);
		if (this->label != nullptr)
			label->setVisible(visible);
		if (atlas != nullptr)
			atlas->setVisible(visible);
	}
}

void Turn::setPosition(const Point& pos)
{
	auto size = showingSprite->getContentSize();
	showingSprite->setPosition(pos);

	if (atlas != nullptr)
	{
		auto position = pos + Point(size.width/2,size.height/2);
		atlas->setPosition(position);
		atlas->setAnchorPoint(Point(1.f,1.f));
	}
}

Point Turn::getPosition()const
{
	return showingSprite->getPosition();
}
//-------------------BattleLayer------------------------
const string BattleLayer::BATTLE_FIGHTER_DEAD_EVENT = "fighter dead";

BattleLayer::BattleLayer()
	:m_nQueueLength(8)
	,m_nFront(0)
	,m_nRear(-1)
	,m_nOurNumber(0)
	,m_nEnemyNumber(0)
	,m_pCurInitialFighter(nullptr)
	,m_bRoundOver(true)
	,m_pDelegate(nullptr)
{
	memset(m_turnQueue, 0, sizeof(m_turnQueue));
}

BattleLayer::~BattleLayer()
{
	//删除turn
	int count = m_nOurNumber + m_nEnemyNumber;

	while (count--)
	{
		auto turn = m_turnQueue[m_nFront];
		delete turn;

		m_nFront = (m_nFront + 1) % m_nQueueLength;
	}
	//直接删除死掉的turn
	for (auto it = m_deadTurnList.begin();it != m_deadTurnList.end();)
	{
		auto turn = *it;
		auto fighter = turn->fighter;

		if (turn->type != TurnType::Enemy)
			fighter->removeFromParent();
		delete turn;

		it = m_deadTurnList.erase(it);
	}
}

bool BattleLayer::init()
{
	//添加监听 fighter死亡监听
	_eventDispatcher->addEventCustomListener(BATTLE_FIGHTER_DEAD_EVENT, SDL_CALLBACK_1(BattleLayer::fighterDeadCallback, this), this);
	
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
	auto top = m_turnQueue[m_nFront];
	int n = lua_gettop(GameScene::getInstance()->getLuaState());
	//等待动作完成
	if (top->fighter->getActionByTag(Fighter::ACTION_TAG) != nullptr)
		return ;
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
		//出现行动按钮
		m_pDelegate->setVisibilityOfActionBtns(true);
		m_pDelegate->setVisibilityOfUndoBtn(false);
	}
}

void BattleLayer::addOur(Actor* actor)
{
	//获得位置
	auto& arr = STATIC_DATA_ARRAY("battle_our_pos_arr");
	auto pos = PointFromString(arr[m_nOurNumber].asString());
	//设置位置
	actor->setPosition(pos);
	//添加到层中
	this->addChild(actor);
	//插入到循环队列中
	this->insertTurnOrderly(TurnType::Player, actor);
}

void BattleLayer::addEnemy(const string& name, int number)
{
	Enemy* enemy = nullptr;
	//获得位置
	auto& arr = STATIC_DATA_ARRAY("battle_enemy_pos_arr");
	Point pos;

	for (int i = 0;i < number;i++)
	{
		enemy = Enemy::create(name);
		m_pCurInitialFighter = enemy;
		//调用脚本的初始化函数
		enemy->initialize();
		//设置位置
		pos = PointFromString(arr[m_nEnemyNumber].asString());
		auto size = enemy->getContentSize();

		enemy->setPosition(pos);
		this->addChild(enemy);
		//设置label
		auto label = LabelBMFont::create(enemy->getFighterName(), "fonts/1.fnt", 300, Color3B(0, 0, 0));
		label->setPosition(Point(size.width / 2, size.height + label->getContentSize().height * 0.5f));
		enemy->addChild(label);
		//创建hp
		int hp = enemy->getProperty(PropertyType::Hp);
		int maxHp =enemy->getMaxHitPoint();
		auto hpBar = HpBar::create("hpBar1.png", "hpBarBG.png", maxHp);
		hpBar->setPosition(Point(size.width / 2, -hpBar->getContentSize().height * 0.5f));
		enemy->addChild(hpBar);
		//创建
		auto fighterChar = enemy->getTurnChar();
		
		auto atlas = LabelAtlas::create(StringUtils::toString(fighterChar), "AtlasAlpha.png", 18, 24, 'A');
		this->addChild(atlas);
		//插入
		this->insertTurnOrderly(TurnType::Enemy, enemy, label, hpBar, atlas);

		m_pCurInitialFighter = nullptr;
	}
}

Turn* BattleLayer::popTopTurn()
{
	//获取队首
	auto turn = m_turnQueue[m_nFront];

	m_turnQueue[m_nFront] = nullptr;
	m_nFront = (m_nFront + 1) % m_nQueueLength;

	switch (turn->type)
	{
	case TurnType::Enemy:m_nEnemyNumber--;break;
	case TurnType::Friend:
	case TurnType::Player:m_nOurNumber--;break;
	default:
		break;
	}

	return turn;
}

Turn* BattleLayer::getTurnByID(int uniqueID)
{
	int count = m_nOurNumber + m_nEnemyNumber;
	int front = m_nFront;
	//遍历寻找
	while (count--)
	{
		auto turn = m_turnQueue[front];
		//获取turn
		auto fighter = turn->fighter;
		
		if (fighter->getFighterID() == uniqueID)
			return turn;

		front = (front + 1) % m_nQueueLength;
	}
	return nullptr;
}

Fighter* BattleLayer::getFighterByID(int uniqueID)
{
	auto turn = this->getTurnByID(uniqueID);
	Fighter* fighter = nullptr;

	if (turn != nullptr)
		fighter = turn->fighter;
	else if (m_pCurInitialFighter->getFighterID() == uniqueID)
		fighter = m_pCurInitialFighter;

	return fighter;
}

Fighter* BattleLayer::searchFighter(TurnType turnType, SearchType searchType)
{
	//是否搜索成功
	bool bOver = false;
	Fighter*fighter = nullptr;
	//SearchType::Random 辅助
	int number = -1;
	int count = m_nEnemyNumber + m_nOurNumber;

	for (int i = m_nFront; count > 0 && !bOver; count--, i = (i + 1) % m_nQueueLength)
	{
		auto turn = m_turnQueue[i];
		auto temp = turn->fighter;
		//满足搜索条件
		if (((turnType == TurnType::Player || turnType == TurnType::Friend)
			&& (turn->type == TurnType::Player || turn->type == TurnType::Friend))
			|| (turnType == TurnType::Enemy && turn->type == turnType))
			;
		else
			continue;

		if (fighter == nullptr)
		{
			fighter = temp;
			continue;
		}
		switch (searchType)
		{
		case SearchType::HpMax:
			if (temp->getProperty(PropertyType::Hp) > fighter->getProperty(PropertyType::Hp))
				fighter = temp;
			break;
		case SearchType::HpMin:
			if (temp->getProperty(PropertyType::Hp) < fighter->getProperty(PropertyType::Hp))
				fighter = temp;
			break;
		case SearchType::Random:
			if (number == -1)
			{
				if (turnType == TurnType::Enemy)
					number = rand() % m_nEnemyNumber;
				else
					number = rand() % m_nOurNumber;
			}

			if (number-- == 0)
			{
				fighter = temp;
				bOver = true;
			}
			break;
		default:
			break;
		}
	}
	return fighter;
}

Turn* BattleLayer::getClickedTurn(const Point& pos)
{
	int count = m_nEnemyNumber + m_nOurNumber;
	int front = m_nFront;
	//遍历寻找
	while (count--)
	{
		auto turn = m_turnQueue[front];
		auto fighter = turn->fighter;
		
		if (fighter->getBoundingBox().containsPoint(pos))
			return turn;

		front = (front + 1) % m_nQueueLength;
	}

	return nullptr;
}

Turn* BattleLayer::getTopTurn()
{
	return m_turnQueue[m_nFront];
}

void BattleLayer::roundOver()
{
	if (m_nEnemyNumber == 0 || m_nOurNumber == 0)
		return ;

	this->setRoundOver(true);
	//弹出队首
	auto turn = m_turnQueue[m_nFront];

	m_turnQueue[m_nFront] = nullptr;
	m_nFront = (m_nFront + 1) % m_nQueueLength;

	switch (turn->type)
	{
	case TurnType::Enemy:m_nEnemyNumber--;break;
	case TurnType::Friend:
	case TurnType::Player:m_nOurNumber--;break;
	default:
		break;
	}
	//TODO:重新插入 速度
	turn->speed -= 1;
	this->insertTurnOrderly(turn);
}

void BattleLayer::setRoundOver(bool var)
{
	m_bRoundOver = var;
}

void BattleLayer::endBattle()
{
	//敌人全部死亡，战斗胜利
	bool victory = ( m_nEnemyNumber == 0 );
	//设置
	for (auto it = m_deadTurnList.begin(); it != m_deadTurnList.end();it++)
	{
		auto turn = *it;
		//设置死亡友军的血量为1
		if (turn->type != TurnType::Enemy)
		{
			DynamicData::getInstance()->setProperty(turn->fighter->getFighterName(), PropertyType::Hp, 1);
			GameScene::getInstance()->getGoodLayer()->updateShownOfProps();
			continue;
		}
		//调用对应脚本的清除函数
		turn->fighter->clean();
	}
	int count = m_nEnemyNumber + m_nOurNumber;
	int front = m_nFront;
	//清空战斗队列，并全部交给deadTurn列表
	while (count--)
	{
		auto turn = m_turnQueue[front];
		auto fighter = turn->fighter;

		if (turn->type == TurnType::Enemy)
		{
			m_nEnemyNumber--;
			if (!victory)
			{
				fighter->victory();
			}
		}
		else
		{
			m_nOurNumber--;
			if (victory)
				fighter->victory();
		}
		m_deadTurnList.push_back(turn);
		//清空战斗队列
		m_turnQueue[front] = nullptr;

		front = (front + 1) % m_nQueueLength;
	}
	m_nFront = 0;
	m_nRear = -1;
	//战斗结束
	m_pDelegate->battleResult(victory);
}

void BattleLayer::clear()
{
	for (auto it = m_deadTurnList.begin();it != m_deadTurnList.end();)
	{
		auto turn = *it;
		auto fighter = turn->fighter;

		fighter->clean();

		delete turn;
		it = m_deadTurnList.erase(it);
	}
}

void BattleLayer::setDelegate(BattleDelegate* pDelegate)
{
	m_pDelegate = pDelegate;
}

void BattleLayer::insertTurnOrderly(Turn* turn)
{
	int front = m_nFront;
	int count = m_nOurNumber + m_nEnemyNumber;

	if (turn->type == TurnType::Player
		|| turn->type == TurnType::Friend)
	{
		m_nOurNumber++;
	}
	else
	{
		m_nEnemyNumber++;
	}

	//对出手进行排序
	while (count-- > 0)
	{
		auto temp = m_turnQueue[front];
		//交换
		if (temp->speed < turn->speed)
		{
			m_turnQueue[front] = turn;
			turn = temp;
		}
		front = (front + 1) % m_nQueueLength;
	}
	//插入
	m_nRear = (m_nRear + 1) % m_nQueueLength;
	m_turnQueue[m_nRear] = turn;
	//设置showing turn 的位置
	count = m_nOurNumber + m_nEnemyNumber;
	front = m_nFront;

	//重新设置turn的位置
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point pos(25,75);
	
	while (count--)
	{
		auto temp = m_turnQueue[front];
		Size size = temp->showingSprite->getContentSize();
		//重新设置位置
		temp->setPosition(pos);

		pos.y += size.height;
		front = (front + 1) % m_nQueueLength;
	}
}

void BattleLayer::insertTurnOrderly(TurnType turnType, Fighter* fighter, LabelBMFont* label, HpBar* hpBar, LabelAtlas* atlas)
{
	Turn* turn = new Turn();
	auto turnFilename = fighter->getTurnFilename();

	turn->type = turnType;
	turn->fighter = fighter;
	turn->label = label;
	turn->hpBar = hpBar;
	turn->atlas = atlas;
	//TODO:设置速度
	turn->speed = 1;//fighter->getSpeed();

	turn->showingSprite = Sprite::createWithSpriteFrameName(turnFilename);
	this->addChild(turn->showingSprite, -1);
	//如果hp == 0，则放入死亡列表中
	if (turn->type != TurnType::Enemy && fighter->getProperty(PropertyType::Hp) == 0)
	{
		turn->setVisible(false);
		m_deadTurnList.push_back(turn);
	}
	else//放入战斗列表中
	{
		this->insertTurnOrderly(turn);
	}
}

void BattleLayer::fighterDeadCallback(EventCustom* eventCustom)
{
	auto fighter = static_cast<Fighter*>(eventCustom->getUserData());
	//遍历获取对应的turn，并移除
	int count = m_nEnemyNumber + m_nOurNumber;
	int front = m_nFront;
	//是否移动元素
	bool bMoving = false;
	//遍历寻找
	while (count--)
	{
		auto turn = m_turnQueue[front];
		//判断是否相同 后面的移入
		if (turn->fighter == fighter && !bMoving)
		{
			bMoving = true;
			//放入死亡对象数组中
			m_deadTurnList.push_back(turn);
			//隐藏
			turn->setVisible(false);

			if (turn->type == TurnType::Enemy)
				m_nEnemyNumber--;
			else
				m_nOurNumber--;
		}
		//元素向前移动一格
		if (bMoving)
		{
			auto next = (front + 1) % m_nQueueLength;
			if (count > 0)
			{
				auto pos1 = m_turnQueue[front]->getPosition();
				auto pos2 = m_turnQueue[next]->getPosition();
				//设置位置
				m_turnQueue[next]->setPosition(pos1);
				m_turnQueue[front]->setPosition(pos2);
				//交换
				auto temp = m_turnQueue[next];
				m_turnQueue[next] = m_turnQueue[front];
				m_turnQueue[front] = temp;
			}
			else//最后一个元素
			{
				m_turnQueue[m_nRear] = nullptr;
			}
		}
		front = (front + 1) % m_nQueueLength;
	}
	//个数少一个
	m_nRear = (m_nRear - 1) % m_nQueueLength;
	//任何一方人数为0，战斗结束
	if (m_nEnemyNumber == 0 || m_nOurNumber == 0)
	{
		this->endBattle();
	}
}