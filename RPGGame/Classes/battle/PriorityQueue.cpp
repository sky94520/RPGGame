#include "HpBar.h"
#include "Fighter.h"
#include "PriorityQueue.h"

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
		auto position = pos + Point(size.width / 2, size.height / 2);
		atlas->setPosition(position);
		atlas->setAnchorPoint(Point(1.f, 1.f));
	}
}

Point Turn::getPosition()const
{
	return showingSprite->getPosition();
}

//-------------------PriorityQueue------------------------
PriorityQueue::PriorityQueue()
	:m_nQueueLength(8)
	,m_nFront(0)
	,m_nRear(-1)
	,m_nOurNumber(0)
	,m_nEnemyNumber(0)
{
	memset(m_turnQueue, 0, sizeof(m_turnQueue));
}

PriorityQueue::~PriorityQueue()
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
	for (auto it = m_deadTurnList.begin(); it != m_deadTurnList.end();)
	{
		auto turn = *it;
		auto fighter = turn->fighter;

		if (turn->type != TurnType::Enemy)
			fighter->removeFromParent();
		delete turn;

		it = m_deadTurnList.erase(it);
	}
}

bool PriorityQueue::init()
{
	return true;
}

Turn* PriorityQueue::getTop()
{
	auto top = m_turnQueue[m_nFront];
	return top;
}

Turn* PriorityQueue::popTopTurn()
{
	//获取队首
	auto turn = m_turnQueue[m_nFront];

	m_turnQueue[m_nFront] = nullptr;
	m_nFront = (m_nFront + 1) % m_nQueueLength;

	switch (turn->type)
	{
	case TurnType::Enemy:m_nEnemyNumber--; break;
	case TurnType::Friend:
	case TurnType::Player:m_nOurNumber--; break;
	default:
		break;
	}
	return turn;
}

Turn* PriorityQueue::getTurnByID(int uniqueID)
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

Fighter* PriorityQueue::searchFighter(TurnType turnType, SearchType searchType)
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

Turn* PriorityQueue::getClickedTurn(const Point& pos)
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

void PriorityQueue::roundOver()
{
	if (m_nEnemyNumber == 0 || m_nOurNumber == 0)
		return;

	//弹出队首
	auto turn = m_turnQueue[m_nFront];

	m_turnQueue[m_nFront] = nullptr;
	m_nFront = (m_nFront + 1) % m_nQueueLength;

	switch (turn->type)
	{
	case TurnType::Enemy:m_nEnemyNumber--; break;
	case TurnType::Friend:
	case TurnType::Player:m_nOurNumber--; break;
	default:
		break;
	}
	//TODO:重新插入 速度
	turn->speed -= 1;
	this->insertTurnOrderly(turn);
}

vector<string> PriorityQueue::endBattle()
{
	//敌人全部死亡，战斗胜利
	bool victory = (m_nEnemyNumber == 0);
	//返回已经死亡的友军的名字
	vector<string> results;
	for (auto it = m_deadTurnList.begin(); it != m_deadTurnList.end(); it++)
	{
		auto turn = *it;
		//设置死亡友军的血量为1
		if (turn->type != TurnType::Enemy)
		{
			results.push_back(turn->fighter->getFighterName());
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

	return results;
}

void PriorityQueue::clear()
{
	for (auto it = m_deadTurnList.begin(); it != m_deadTurnList.end();)
	{
		auto turn = *it;
		auto fighter = turn->fighter;

		fighter->clean();

		delete turn;
		it = m_deadTurnList.erase(it);
	}
}

void PriorityQueue::insertTurnOrderly(Turn* turn)
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
	Point pos(25, 75);

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

void PriorityQueue::insertTurnOrderly(TurnType turnType, Fighter* fighter, LabelBMFont* label, HpBar* hpBar, LabelAtlas* atlas)
{
	Turn* turn = new Turn();
	auto turnFilename = fighter->getThumbnail();

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

void PriorityQueue::fighterDead(Fighter* fighter)
{
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
}
