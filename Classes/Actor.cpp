#include "Actor.h"
#include "StaticData.h"
#include "Good.h"
#include "GameScene.h"
#include "DynamicData.h"

Actor::Actor()
{
}

Actor::~Actor()
{
}

Actor* Actor::create(const string& chartletName, int uniqueID)
{
	auto actor = new Actor();

	if (actor && actor->init(chartletName, uniqueID))
		actor->autorelease();
	else
		SDL_SAFE_DELETE(actor);

	return actor;
}

bool Actor::init(const string& chartletName, int uniqueID)
{
	m_chartletName = chartletName;
	m_nUniqueID = uniqueID;
	m_fightState = FightState::Walk;
	//获取动画
	auto animation = StaticData::getInstance()->getSVAnimation(chartletName, m_fightState);
	auto animate = Animate::create(animation);
	animate->setTag(ANIMATION_TAG);

	this->bindSpriteWithAnimate(animate);

	return true;
}

void Actor::ready()
{
	m_lastFightState = FightState::Walk;

	this->changeFightState(FightState::Wait);
}

void Actor::escape()
{
	Fighter::escape();

	this->changeFightState(FightState::Escape);
}

void Actor::guard()
{
	Fighter::guard();
	this->changeFightState(FightState::Guard);
}

int Actor::getFighterID() const
{
	return m_nUniqueID;
}

string Actor::getFighterName() const
{
	//TODO
	return DynamicData::getInstance()->getName(m_chartletName);
}

string Actor::getTurnFilename() const
{
	return StaticData::getInstance()->getTurnFilename(m_chartletName);
}

void Actor::execute(Fighter* fighter)
{
	//获取装备着的武器
	auto weapon = DynamicData::getInstance()->getEquipment(m_chartletName, EquipmentType::Weapon);
	//不同的武器对应不同的出手类型
	FightState fightState = FightState::Thrust;
	//执行对应的武器脚本 默认为空手武器
	string tableName = "BareHands";

	if (weapon != nullptr)
	{
		tableName = weapon->getParentTableName();
		fightState = weapon->getFightState();
	}
	//执行对应的脚本
	auto gameScene = GameScene::getInstance();

	gameScene->getLuaGlobal(tableName.c_str());
	//获取 execute 函数
	gameScene->getLuaField(-1, "execute");
	//移除table
	gameScene->removeLuaIndex(-2);
	//放入参数
	gameScene->getLuaGlobal(tableName.c_str());
	gameScene->pushInt(this->getFighterID());
	gameScene->pushInt(fighter->getFighterID());
	//执行函数
	int ret = gameScene->resumeFunction(3);
	//不同的武器对应不同的出手类型
	//this->changeFightState(fightState);
}

void Actor::good(Good* good)
{
	//对应的物品是否有getFightState函数，有，则获取
	FightState fightState = good->getFightState();

	this->changeFightState(fightState);
}

void Actor::victory()
{
	this->changeFightState(FightState::Victory);
}

void Actor::clean()
{
}

PropertyStruct Actor::getTotalProperties() const
{
	return DynamicData::getInstance()->getTotalProperties(m_chartletName);
}

int Actor::getMaxHitPoint() const
{
	return DynamicData::getInstance()->getMaxHitPoint(m_chartletName);
}

void Actor::setMaxHitPoint(int var)
{
	DynamicData::getInstance()->setMaxHitPoint(m_chartletName, var);
}

int Actor::getMaxManaPoint() const
{
	return DynamicData::getInstance()->getMaxManaPoint(m_chartletName);
}

void Actor::setMaxManaPoint(int var)
{
	DynamicData::getInstance()->setMaxManaPoint(m_chartletName, var);
}

int Actor::getProperty(PropertyType type) const
{
	return DynamicData::getInstance()->getProperty(m_chartletName, type);
}

void Actor::setProperty(PropertyType type, int value)
{
	DynamicData::getInstance()->setProperty(m_chartletName, type, value);
}

float Actor::getHitRate() const
{
	return 0.95f;
}

float Actor::getEvasionRate() const
{
	return 0.05f;
}

float Actor::getCriticalRate() const
{
	return 0.04f;
}

void Actor::onHurt(int afterDamage)
{
	FightState fightState = m_fightState;
	
	if (afterDamage == 0)
		fightState = FightState::Evade;
	else
		fightState = FightState::Damage;
	//获取动画
	auto animate = this->changeFightState(fightState);

	if (afterDamage != 0)
	{
		SoundManager::getInstance()->playEffect(STATIC_DATA_STRING("actor_damage_se"), 0);
	}
}
