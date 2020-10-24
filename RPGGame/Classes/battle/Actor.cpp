#include "Actor.h"
#include "../entity/Good.h"

#include "../data/StaticData.h"
#include "../data/DynamicData.h"
#include "../data/CharacterData.h"

#include "../GameScene.h"
#include "../GameMacros.h"

#include "../manager/ScriptManager.h"
#include "../script/LuaStack.h"

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
	m_fightState = FightState::Idle;
	//获取动画
	CharacterData* characterData = StaticData::getInstance()->getCharacterData();
	auto animation = characterData->getSVAnimation(chartletName, m_fightState);
	auto animate = Animate::create(animation);
	animate->setTag(ANIMATION_TAG);
	this->bindSpriteWithAnimate(animate);

	return true;
}
void Actor::updateSelf()
{
	//获取血量
	auto hp = this->getProperty(PropertyType::Hp);
	//当前血量比例
	PlayerData* data = DynamicData::getInstance()->getPlayerData(m_chartletName);
	auto ratio = (float)hp / data->maxHp;
	FightState state = FightState::Idle;
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
		state = FightState::Idle;
	}
	//改变状态
	this->changeFightState(state);
}

void Actor::ready()
{
	m_lastFightState = FightState::Idle;

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
	return m_chartletName;
	//return DynamicData::getInstance()->getName(m_chartletName);
}

string Actor::getThumbnail() const
{
	CharacterData* characterData = StaticData::getInstance()->getCharacterData();
	return characterData->getTurnFilename(m_chartletName);
}

void Actor::execute(Fighter* fighter)
{
	//获取装备的武器
	PlayerData* data = DynamicData::getInstance()->getPlayerData(m_chartletName);
	Good* weapon = DynamicData::getInstance()->getEquipment(m_chartletName, EquipmentType::Weapon);
	//不同的武器对应不同的出手类型
	FightState fightState = FightState::Thrust;
	//执行对应的武器脚本 默认为空手武器
	string tableName = "BareHands";

	if (weapon != nullptr)
	{
		tableName = weapon->getPrototype();
		fightState = weapon->getFightState();
	}
	//执行对应的脚本
	LuaStack* luaStack = GameScene::getInstance()->getScriptManager()->getLuaStack();

	luaStack->getLuaGlobal(tableName.c_str());
	//获取 execute 函数
	luaStack->getLuaField(-1, "execute");
	//移除table
	luaStack->removeLuaIndex(-2);
	//放入参数
	luaStack->getLuaGlobal(tableName.c_str());
	luaStack->pushInt(this->getFighterID());
	luaStack->pushInt(fighter->getFighterID());
	//执行函数
	int ret = luaStack->resumeFunction(3);
	//不同的武器对应不同的出手类型
	//this->changeFightState(fightState);
}

void Actor::useItem(Good* good)
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

Properties* Actor::getTotalProperties() const
{
	PlayerData* data = DynamicData::getInstance()->getPlayerData(m_chartletName);
	if (data == nullptr)
		return nullptr;
	return &data->properties;
}

int Actor::getMaxHitPoint() const
{
	PlayerData* data = DynamicData::getInstance()->getPlayerData(m_chartletName);
	if (data == nullptr)
		return -1;
	return data->maxHp;
}

void Actor::setMaxHitPoint(int var)
{
	PlayerData* data = DynamicData::getInstance()->getPlayerData(m_chartletName);
	if (data == nullptr)
	{
		printf("Actor::setMaxHitPoint error: data[%s]=null\n", m_chartletName.c_str());
		return ;
	}
	data->maxHp = var;
}

int Actor::getMaxManaPoint() const
{
	PlayerData* data = DynamicData::getInstance()->getPlayerData(m_chartletName);
	if (data == nullptr)
		return -1;
	return data->maxMp;
}

void Actor::setMaxManaPoint(int var)
{
	PlayerData* data = DynamicData::getInstance()->getPlayerData(m_chartletName);
	if (data == nullptr)
	{
		printf("Actor::setMaxManaPoint error: data[%s]=null\n", m_chartletName.c_str());
		return ;
	}
	data->maxMp = var;
}

int Actor::getProperty(PropertyType type) const
{
	PlayerData* data = DynamicData::getInstance()->getPlayerData(m_chartletName);
	Properties& properties = data->properties;
	int value = 0;

	switch (type)
	{
	case PropertyType::Hp:
		value = properties.hp;
		break;
	case PropertyType::Mp:
		value = properties.mp;
		break;
	case PropertyType::Attack:
		value = properties.attack;
		break;
	case PropertyType::Defense:
		value = properties.defense;
		break;
	case PropertyType::MagicAttack:
		value = properties.magicAttack;
		break;
	case PropertyType::MagicDefense:
		value = properties.magicDefense;
		break;
	case PropertyType::Agility:
		value = properties.agility;
		break;
	case PropertyType::Luck:
		value = properties.luck;
		break;
	default:
		break;
	}

	return value;
}

void Actor::setProperty(PropertyType type, int value)
{
	PlayerData* data = DynamicData::getInstance()->getPlayerData(m_chartletName);
	Properties& properties = data->properties;
	string propName;

	switch (type)
	{
	case PropertyType::Hp:
	{
		auto maxHp = this->getMaxHitPoint();
		//设置最大值限制
		if (maxHp < value)
			value = maxHp;
		properties.hp = value;

		propName = "hp";
	}break;
	case PropertyType::Mp:
	{
		auto maxMp = this->getMaxManaPoint();
		//设置最大值限制
		if (maxMp < value)
			value = maxMp;
		properties.mp = value;
		propName = "mp";
	}break;
	case PropertyType::Attack:
		properties.attack = value;
		propName = "attack";
		break;
	case PropertyType::Defense:
		properties.defense = value;
		propName = "defense";
		break;
	case PropertyType::MagicAttack:
		properties.magicAttack = value;
		propName = "magic_attack";
		break;
	case PropertyType::MagicDefense:
		properties.magicDefense = value;
		propName = "magic_defense";
		break;
	case PropertyType::Agility:
		properties.agility = value;
		propName = "agility";
		break;
	case PropertyType::Luck:
		properties.luck = value;
		propName = "luck";
		break;
	default:
		break;
	}
	//this->updateSaveData(playerName, propName, value);
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