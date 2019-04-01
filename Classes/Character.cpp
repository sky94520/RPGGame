#include "Character.h"
#include "StaticData.h"
#include "AStar.h"
#include "ShortestPathStep.h"
#include "GameScene.h"
#include "MapLayer.h"
#include "NonPlayerCharacter.h"

Character::Character()
	:m_durationPerGrid(0.3f)
	,m_dir(Direction::Down)
	,m_state(State::None)
	,m_bDirty(true)
	,m_pFollowCharacter(nullptr)
	,m_nStepIndex(0)
	,m_lastStep(nullptr)
	,m_bHavePendingMove(false)
	,m_bMoving(false)
	,m_pTriggerNPC(nullptr)
{
}

Character::~Character()
{
	//清除路径
	this->clearShortestPath();
	SDL_SAFE_RELEASE(m_lastStep);
}

Character* Character::create(const string& chartletName)
{
	auto character = new Character();

	if (character && character->init(chartletName))
		character->autorelease();
	else
		SDL_SAFE_DELETE(character);

	return character;
}

Character* Character::create(const string& chartletName, Direction direction)
{
	auto character = new Character();

	if (character && character->init(chartletName, direction))
		character->autorelease();
	else
		SDL_SAFE_DELETE(character);

	return character;
}

bool Character::init(const string& chartletName)
{
	return Character::init(chartletName, Direction::Down);
}

bool Character::init(const string& chartletName, Direction direction)
{
	m_chartletName = chartletName;
	m_dir = direction;
	//设置当前为站立状态
	this->changeState(State::Idle);

	return true;
}

void Character::clear()
{
	m_chartletName = "";
	m_durationPerGrid = 0.3f;
	m_bDirty = true;

	m_dir = Direction::Down;
	m_state = State::None;
	m_pFollowCharacter = nullptr;

	if (m_lastStep != nullptr)
		SDL_SAFE_RELEASE_NULL(m_lastStep);
	m_bHavePendingMove = false;
	m_bMoving = false;
	m_pTriggerNPC = nullptr;
}

void Character::follow(Character* character)
{
	character->m_pFollowCharacter = this;
	
	this->setPosition(character->getPosition());
}

void Character::sit()
{
	this->changeState(State::Idle);
	m_bMoving = false;
	//停止运动
	//this->stopActionByTag(Entity::ACTION_TAG);
	this->getSprite()->stopActionByTag(Entity::ANIMATION_TAG);
	//停止行走
	this->clearShortestPath();
}

void Character::setTriggerNPC(NonPlayerCharacter* npc)
{
	m_pTriggerNPC = npc;
}

Direction Character::getDirection() const
{
	return m_dir;
}

void Character::setDirection(Direction direction)
{
	if (m_dir != direction)
	{
		m_dir = direction;
		m_bDirty = true;

		this->changeState(m_state);
	}
}

float Character::moveToward(const Point& tilePos)
{
	float duration = 0.f;
	//当前角色正在运动，则更改待到达目的地
	if (m_bMoving)
	{
		m_bHavePendingMove = true;
		m_pendingMove = tilePos;

		return duration;
	}
	auto fromTile = GameScene::getInstance()->getMapLayer()->convertToTileCoordinate(this->getPosition());
	//A*算法解析路径
	AStar* pAStar = StaticData::getInstance()->getAStar();
	auto pTail = pAStar->parse(fromTile, tilePos);
	//目标可达,做运动前准备
	if (pTail != nullptr)
	{
		duration = this->constructPathAndStartAnimation(pTail);
	}
	return duration;
}

bool Character::moveOneStep(ShortestPathStep* step)
{
	//当前角色正在运动.先停止运动
	if (!m_shortestPath.empty())
	{
		this->clearShortestPath();
	}
	SDL_SAFE_RETAIN(step);
	this->m_shortestPath.push_back(step);
	this->popStepAndAnimate();

	return true;
}

void Character::popStepAndAnimate()
{
	m_bMoving = false;
	//存在待到达目的点，转入
	if (m_bHavePendingMove)
	{
		m_bHavePendingMove = false;

		this->clearShortestPath();
		//滞后改变
		this->moveToward(m_pendingMove);

		return ;
	}//运动结束
	else if (m_nStepIndex >= m_shortestPath.size())
	{
		this->clearShortestPath();
		//站立动画
		this->changeState(State::Idle);

		return ;
	}//点击了NPC，且将要到达
	else if (m_pTriggerNPC != nullptr && m_nStepIndex == m_shortestPath.size() - 1)
	{
		auto delta = m_shortestPath.back()->getTilePos() - m_lastStep->getTilePos();
		auto newDir = this->getDirection(delta);
		//改变方向
		if (newDir != m_dir)
		{
			m_bDirty = true;
			m_dir = newDir;
		}
		this->clearShortestPath();
		this->changeState(State::Idle);

		m_pTriggerNPC->execute(this->getUniqueID());
		m_pTriggerNPC = nullptr;
		return ;
	}
	//存在跟随角色，设置跟随
	if (m_pFollowCharacter != nullptr)
	{
		m_pFollowCharacter->moveOneStep(m_lastStep);
	}
	SDL_SAFE_RELEASE(m_lastStep);
	m_lastStep = m_shortestPath.at(m_nStepIndex);
	SDL_SAFE_RETAIN(m_lastStep);

	auto tileSize = GameScene::getInstance()->getMapLayer()->getTiledMap()->getTileSize();
	//开始新的运动
	auto step = m_shortestPath.at(m_nStepIndex++);
	auto tilePos = step->getTilePos();
	Point pos = Point((tilePos.x + 0.5f) * tileSize.width,(tilePos.y + 0.5f) * tileSize.height);
	//开始运动
	MoveTo* move = MoveTo::create(m_durationPerGrid, pos);
	CallFunc* moveCallback = CallFunc::create([this]()
	{
		//发送事件
		_eventDispatcher->dispatchCustomEvent(GameScene::CHARACTER_MOVE_TO_TILE, this);
		//this->popStepAndAnimate();
	});
	//运行动作
	auto seq = Sequence::createWithTwoActions(move,moveCallback);
	seq->setTag(Entity::ACTION_TAG);

	this->runAction(seq);
	//引擎原因，需要先调用一次
	seq->step(1.f/60);
	//是否改变方向
	auto delta = pos - this->getPosition();
	Direction newDir = this->getDirection(delta);

	if (newDir != m_dir)
	{
		m_dir = newDir;
		m_bDirty = true;
	}
	//改为运动状态
	this->changeState(State::Walking);

	m_bMoving = true;
}

void Character::changeState(State state)
{
	if (m_state == state && !m_bDirty)
		return;

	m_state = state;
	m_bDirty = false;

	auto animation = StaticData::getInstance()->getWalkingAnimation(m_chartletName, m_dir);

	switch (state)
	{
	case State::None:
		break;
	case State::Walking:
		{
		}break;
	case State::Idle://人物站立图跟素材相关
		{
			auto frame = animation->getFrames().at(1)->getSpriteFrame();

			if (m_pSprite == nullptr)
				this->bindSpriteWithSpriteFrame(frame);
			else
			{
				m_pSprite->stopActionByTag(ANIMATION_TAG);
				m_pSprite->setSpriteFrame(frame);
			}
			animation = nullptr;
		}
		break;
	default:
		break;
	}

	if (animation == nullptr)
		return;
	//运行动画
	auto animate = Animate::create(animation);
	animate->setTag(ANIMATION_TAG);

	if (m_pSprite == nullptr)
	{
		this->bindSpriteWithAnimate(animate);
	}
	else
	{
		m_pSprite->stopActionByTag(ANIMATION_TAG);
		m_pSprite->runAction(animate);
	}
}

float Character::constructPathAndStartAnimation(ShortestPathStep* pHead)
{
	int number = 0;
	//此时的角色一定不在运动中
	//构建运动列表
	while (pHead != nullptr && pHead->getParent() != nullptr)
	{
		auto it = m_shortestPath.begin();
		m_shortestPath.insert(it,pHead);

		SDL_SAFE_RETAIN(pHead);
		pHead = pHead->getParent();

		number++;
	}
	//此位置为主角当前tile 位置
	SDL_SAFE_RELEASE(m_lastStep);
	m_lastStep = pHead;
	SDL_SAFE_RETAIN(m_lastStep);

	this->popStepAndAnimate();

	return number * this->getDurationPerGrid();
}

void Character::clearShortestPath()
{
	for (auto it = m_shortestPath.begin();it != m_shortestPath.end();)
	{
		auto step = *it;

		SDL_SAFE_RELEASE_NULL(step);
		it = m_shortestPath.erase(it);
	}
	m_nStepIndex = 0;
}

Direction Character::getDirection(const Point& delta) const
{
	Direction dir = Direction::Down;

	if (delta.x > 0.f)
	{
		dir = Direction::Right;
	}
	else if (delta.x < 0.f)
	{
		dir = Direction::Left;
	}
	else if (delta.y > 0)
	{
		dir = Direction::Down;
	}
	else if (delta.y < 0)
	{
		dir = Direction::Up;
	}
	return dir;
}