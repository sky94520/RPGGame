#include "Fighter.h"
#include "DynamicData.h"
#include "BattleLayer.h"
#include "StaticData.h"

const float Fighter::SPEED = 550.f;

Fighter::Fighter()
	:m_attackScale(1.f)
	,m_defenseRate(0.f)
	,m_bGuarding(false)
	,m_bDead(false)
	,m_nUniqueID(0)
	,m_fightState(FightState::None)
	,m_lastFightState(FightState::None)
{
}

Fighter::~Fighter()
{
}

int Fighter::hurt(int attack, AttributeType attrType = AttributeType::None, HurtType hurtType = HurtType::Attack)
{
	//获取当前防御力
	int defense = 0;
	int afterAttack = 0;
	//护甲
	if (hurtType == HurtType::Attack || hurtType == HurtType::CriticalAttack)
	{
		defense = getProperty(PropertyType::Defense);
		int baseAttack = (attack * m_attackScale / 2) - (defense * m_defenseRate / 4);
		int attackFloat = baseAttack / 16 + 1;
		int ret = rand() % 2;

		afterAttack = (ret == 1 ? baseAttack + attackFloat : baseAttack - attackFloat);

		if (afterAttack < 0)
			afterAttack = 0;
		//当前正在防御 防御仅能挡住一次攻击
		else if (m_bGuarding)
		{
			m_bGuarding = false;

			afterAttack = int(afterAttack * m_defenseRate);
		}
	}//魔抗
	else if (hurtType == HurtType::MagicAttack)
	{
		defense = getProperty(PropertyType::MagicDefense);
		if (defense != 0)
			afterAttack = attack * attack / defense;

		if (afterAttack <= 0)
			afterAttack = 1;
	}
	//受伤
	int hp = getProperty(PropertyType::Hp);

	int afterHp = hp - afterAttack;

	if (afterHp < 0)
	{
		afterHp = 0;
		SoundManager::getInstance()->playEffect("sound/se/Miss.ogg", 0);
	}
	//设置血量
	setProperty(PropertyType::Hp, afterHp);
	//回调受伤函数
	onHurt(afterAttack);
	//返回实际的受伤值
	return afterAttack;
}

float Fighter::moveToTarget(Fighter* fighter)
{
	//设置位置
	auto pos1 = this->getPosition();
	auto size1 = this->getContentSize();

	auto pos2 = fighter->getPosition();
	auto size2 = fighter->getContentSize();

	Point targetPos;
	Point delta;
	//此人物在左边，目标在右边
	if (pos1.x < pos2.x)
	{
		targetPos.x = pos2.x - size2.width * .5f - size1.width * .5f;
	}
	//此人物在右边，目标在左边
	else
	{
		targetPos.x = pos2.x + size2.width * .5f + size1.width * .5f;
	}
	targetPos.y = pos2.y;
	//调整人物y位置
	auto height = (size1.height - size2.height ) / 2;
	targetPos.y -= height > 0 ? height : height;

	delta = targetPos - pos1;
	auto length = delta.length();
	auto duration = length / SPEED;

	MoveTo* move = MoveTo::create(duration, targetPos);

	//this->stopActionByTag(ACTION_TAG);
	this->runAction(move);

	m_lastPos = pos1;

	return duration;
}

float Fighter::backTo()
{
	//设置位置
	auto pos1 = this->getPosition();

	auto delta = pos1 - m_lastPos;
	auto length = delta.length();
	auto duration = length / SPEED;

	MoveTo* move = MoveTo::create(duration, m_lastPos);

	//this->stopActionByTag(ACTION_TAG);
	this->runAction(move);

	return duration;
}

int Fighter::getSpeed() const
{
	return getProperty(PropertyType::Agility) * 5;
}

bool Fighter::isDying() const
{
	return getProperty(PropertyType::Hp) <= 0;
}

bool Fighter::isDead() const
{
	return m_bDead;
}

FiniteTimeAction* Fighter::changeFightState(FightState state)
{
	if (m_fightState == state)
		return nullptr;
	//设置状态
	m_lastFightState = m_fightState;
	m_fightState = state;
	//获得动画
	Animation* animation = nullptr;
	animation = StaticData::getInstance()->getSVAnimation(m_chartletName, state);
	//先停止原先的动画
	this->getSprite()->stopActionByTag(ANIMATION_TAG);

	if (animation == nullptr)
		return nullptr;

	auto animate = Animate::create(animation);
	animate->setTag(ANIMATION_TAG);
	this->getSprite()->runAction(animate);
	//进行回归动作
	if (m_fightState == FightState::Thrust || m_fightState == FightState::Swing
		|| m_fightState == FightState::Missile || m_fightState == FightState::Item
		|| m_fightState == FightState::Skill || m_fightState == FightState::Damage)
	{
		auto duration = animation->getDuration();
	
		DelayTime* delayTime = DelayTime::create(duration);
		CallFunc* end = CallFunc::create([this]()
		{ 
			this->updateSelf();
		});

		auto seq = Sequence::createWithTwoActions(delayTime, end);
		seq->setTag(ACTION_TAG);

		this->stopActionByTag(ACTION_TAG);
		this->runAction(seq);
	}
	return animate;
}

void Fighter::updateSelf()
{
	//获取血量
	auto hp = this->getProperty(PropertyType::Hp);
	//当前血量比例
	int maxHp = DynamicData::getInstance()->getMaxHitPoint(m_chartletName);
	auto ratio = (float)hp / maxHp;
	FightState state = FightState::Walk;
	//血量为0且还没死亡=》 死亡
	if (m_bDead || hp == 0)
	{
		state = FightState::Dead;
		this->dead();
	}
	else if (m_lastFightState == FightState::Guard)
	{
		state = FightState::Guard;
	}
	//进入即死状态
	else if (ratio <= 0.3f)
	{
		state = FightState::Dying;
	}
	else
	{
		state = FightState::Walk;
	}
	//改变状态
	this->changeFightState(state);
}

void Fighter::escape()
{
	SoundManager::getInstance()->playEffect(STATIC_DATA_STRING("escape_se"), 0);
}

void Fighter::guard()
{
	m_bGuarding = true;
	m_defenseRate = 0.5f;
}

void Fighter::dead()
{
	//发送死亡事件
	auto eventDispatcher = Director::getInstance()->getEventDispatcher();
	
	eventDispatcher->dispatchCustomEvent(BattleLayer::BATTLE_FIGHTER_DEAD_EVENT, this);
	m_bDead = true;
}