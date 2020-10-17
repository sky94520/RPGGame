#include "Enemy.h"
#include "../GameScene.h"
#include "../data/DynamicData.h"
#include "../data/StaticData.h"
#include "../data/CharacterData.h"
#include "../manager/ScriptManager.h"
#include "../script/LuaStack.h"

Enemy::Enemy()
{
}

Enemy::~Enemy()
{
}

Enemy* Enemy::create(const string& enemyName)
{
	auto enemy = new Enemy();

	if (enemy && enemy->init(enemyName))
		enemy->autorelease();
	else
		SDL_SAFE_DELETE(enemy);

	return enemy;
}

bool Enemy::init(const string& enemyName)
{
	m_prototype = enemyName;

	//获取实际变量名(唯一)
	string variableName;
	m_nUniqueID = this->getUniqueID(true);
	variableName = StringUtils::format("%s%02d", enemyName.c_str(), m_nUniqueID);

	//执行代码
	string codes = StringUtils::format("%s=%s:new()", variableName.c_str(), enemyName.c_str());
	LuaStack* luaStack = GameScene::getInstance()->getScriptManager()->getLuaStack();
	luaStack->executeString(codes.c_str());
	//设置变量名称
	m_tableName = variableName;
	//设置透明级联
	this->setCascadeOpacityEnabled(true);

	return true;
}

bool Enemy::initialize()
{
	LuaStack* luaStack = GameScene::getInstance()->getScriptManager()->getLuaStack();
	//获取函数
	luaStack->getLuaGlobal(m_tableName.c_str());
	luaStack->getLuaField(-1, "initialize");
	//放入参数
	luaStack->getLuaGlobal(m_tableName.c_str());
	luaStack->pushInt(Object::getUniqueID());
	//执行
	luaStack->executeFunction(2, 0);
	luaStack->pop(1);
	//如果在初始化函数中没有赋值，则默认赋值为对应的table
	if (m_chartletName.empty())
	{
		m_chartletName = m_prototype;
	}
	return true;
}

char Enemy::getTurnChar() const
{
	return 'A' + m_nUniqueID - 1;
}

void Enemy::dead()
{
	Fighter::dead();

	this->getSprite()->setOpacity(255);
	this->setVisible(true);
}

FiniteTimeAction* Enemy::changeFightState(FightState state)
{
	if (m_fightState == state)
		return nullptr;
	//设置状态
	m_lastFightState = m_fightState;
	m_fightState = state;
	//获得动画
	CharacterData* characterData = StaticData::getInstance()->getCharacterData();
	Animation* animation = characterData->getSVAnimation(m_chartletName, state);
	ActionInterval* action = nullptr;
	//额外动作，对于受伤 死亡，有一个额外动作
	if (animation == nullptr)
	{
		if (m_fightState == FightState::Damage)
		{
			//显示受伤动画
			action = Blink::create(0.6f, 3);
		}
		else if (m_fightState == FightState::Dead)
		{
			action = FadeTo::create(0.6f, 0);
		}
		//不存在的动画，则默认跳转到walk状态中
		else
		{
			m_fightState = FightState::Idle;
			animation = characterData->getSVAnimation(m_chartletName, m_fightState);
		}
	}
	auto duration = 0.f;
	if (animation != nullptr)
	{
		duration = animation->getDuration();
		action = Animate::create(animation);
	}
	else
	{
		duration = action->getDuration();
	}

	if (action == nullptr)
		return nullptr;
	action->setTag(ANIMATION_TAG);
	//智能更新
	if (this->getSprite() == nullptr)
	{
		this->bindSpriteWithAnimate(static_cast<Animate*>(action));
	}
	else
	{
		//先停止原先的动画
		this->getSprite()->stopActionByTag(ANIMATION_TAG);
		//显示贴图
		this->getSprite()->setVisible(true);

		this->getSprite()->runAction(action);
	}
	//进行回归动作
	if (m_fightState == FightState::Thrust || m_fightState == FightState::Swing
		|| m_fightState == FightState::Missile || m_fightState == FightState::Item
		|| m_fightState == FightState::Skill || m_fightState == FightState::Damage
		|| m_fightState == FightState::Dead)
	{
		DelayTime* delayTime = DelayTime::create(duration);
		CallFunc* end = nullptr;

		if (m_fightState == FightState::Dead)
		{
			end = CallFunc::create(SDL_CALLBACK_0(Enemy::dead, this));
		}
		else
		{
			end = CallFunc::create([this]()
			{
				this->updateSelf();
			});
		}

		auto seq = Sequence::createWithTwoActions(delayTime, end);
		seq->setTag(ACTION_TAG);

		//this->stopActionByTag(ACTION_TAG);
		this->runAction(seq);
	}
	return action;
}

void Enemy::updateSelf()
{
	//获取血量
	auto hp = this->getProperty(PropertyType::Hp);
	//当前血量比例
	int maxHp = this->getMaxHitPoint();
	auto ratio = (float)hp / maxHp;
	FightState state = FightState::Idle;
	//血量为0且还没死亡=》 死亡
	if (m_bDead || hp == 0)
	{
		state = FightState::Dead;
		//this->dead();
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

int Enemy::getFighterID() const
{
	return Object::getUniqueID();
}

string Enemy::getFighterName() const
{
	auto name = this->getValueForKey("name", Value::Type::STRING).asString();
	auto ch = this->getTurnChar();

	return StringUtils::format("%s%c", name.c_str(), ch);
}

string Enemy::getThumbnail() const
{
	return this->getValueForKey("turnName", Value::Type::STRING).asString();
}

void Enemy::execute(Fighter* fighter)
{
	LuaStack* luaStack = GameScene::getInstance()->getScriptManager()->getLuaStack();

	luaStack->getLuaGlobal(m_tableName.c_str());
	luaStack->getLuaField(-1, "execute");
	//移除table
	luaStack->removeLuaIndex(-2);
	//放入参数
	luaStack->getLuaGlobal(m_tableName.c_str());
	//执行
	int ret = luaStack->resumeFunction(1);
}

void Enemy::useItem(Good* good)
{
}

void Enemy::victory()
{
	this->changeFightState(FightState::Victory);
}

void Enemy::clean()
{
	LuaStack* luaStack = GameScene::getInstance()->getScriptManager()->getLuaStack();

	if (luaStack != nullptr)
	{
		//重置唯一ID
		this->setUniqueID(0);
		//获得函数
		luaStack->getLuaGlobal(m_tableName.c_str());
		luaStack->getLuaField(-1, "clean");
		luaStack->removeLuaIndex(-2);
		//放入参数
		luaStack->getLuaGlobal(m_tableName.c_str());
		//执行函数
		luaStack->executeFunction(1, 0);
	}
}

Properties* Enemy::getTotalProperties() const
{
	//需要自己手动释放内存
	Properties* propStruct = new Properties();

	propStruct->hp = this->getProperty(PropertyType::Hp);
	propStruct->mp = this->getProperty(PropertyType::Mp);
	propStruct->attack = this->getProperty(PropertyType::Attack);
	propStruct->defense = this->getProperty(PropertyType::Defense);
	propStruct->magicAttack = this->getProperty(PropertyType::MagicAttack);
	propStruct->magicDefense = this->getProperty(PropertyType::MagicDefense);
	propStruct->agility = this->getProperty(PropertyType::Agility);
	propStruct->luck = this->getProperty(PropertyType::Luck);

	return propStruct;
}

int Enemy::getMaxHitPoint() const
{
	return this->getValueForKey("maxHp", Value::Type::INTEGER).asInt();
}

void Enemy::setMaxHitPoint(int var)
{
	//TODO
}

int Enemy::getMaxManaPoint() const
{
	return this->getValueForKey("maxMp", Value::Type::INTEGER).asInt();
}

void Enemy::setMaxManaPoint(int var)
{
}

int Enemy::getProperty(PropertyType type) const
{
	//获取键
	auto key = StaticData::getInstance()->toString(type);

	auto luaStack = GameScene::getInstance();
	auto value = this->getValueForKey(key, Value::Type::INTEGER);


	return value.asInt();
}

void Enemy::setProperty(PropertyType type, int value)
{
	auto& array = StaticData::getInstance()->getValueForKey("enemy_prop_func");
	int index = static_cast<int>(type);
	auto funcName = array[index].asString();

	LuaStack* luaStack = GameScene::getInstance()->getScriptManager()->getLuaStack();
	//获取函数
	luaStack->getLuaGlobal(m_tableName.c_str());
	luaStack->getLuaField(-1, funcName.c_str());
	//放入参数
	luaStack->getLuaGlobal(m_tableName.c_str());
	luaStack->pushInt(value);
	//执行
	luaStack->executeFunction(2, 0);

	luaStack->pop(1);
}

float Enemy::getHitRate() const
{
	return this->getValueForKey("hitRate", Value::Type::FLOAT).asFloat();
}

float Enemy::getEvasionRate() const
{
	return this->getValueForKey("evasionRate", Value::Type::FLOAT).asFloat();
}

float Enemy::getCriticalRate() const
{
	return this->getValueForKey("criticalRate", Value::Type::FLOAT).asFloat();
}

void Enemy::onHurt(int afterDamage)
{
	LuaStack* luaStack = GameScene::getInstance()->getScriptManager()->getLuaStack();

	luaStack->getLuaGlobal(m_tableName.c_str());
	luaStack->getLuaField(-1, "hurt");
	//放入参数
	luaStack->getLuaGlobal(m_tableName.c_str());
	luaStack->pushInt(afterDamage);
	//执行
	luaStack->executeFunction(2, 1);

	luaStack->pop(1);

	FightState fightState = m_fightState;
	//伤害为0，表示没有受伤
	if (afterDamage == 0)
		fightState = FightState::Evade;
	else
	{
		fightState = FightState::Damage;
		SoundManager::getInstance()->playEffect(STATIC_DATA_STRING("enemy_damage_se"), 0);
	}
	//改变动画
	this->changeFightState(fightState);
}

int Enemy::getUniqueID(bool bAdding)const
{
	LuaStack* luaStack = GameScene::getInstance()->getScriptManager()->getLuaStack();

	luaStack->getLuaGlobal(m_prototype.c_str());
	luaStack->getLuaField(-1, "getUniqueID");
	//放入参数
	luaStack->getLuaGlobal(m_prototype.c_str());
	luaStack->pushBoolean(bAdding);
	//执行
	luaStack->executeFunction(2, 1);
	//获取结果
	int uniqueID = luaStack->checkLuaInt(-1);

	luaStack->pop(2);

	return uniqueID;
}

void Enemy::setUniqueID(int uniqueID)
{
	LuaStack* luaStack = GameScene::getInstance()->getScriptManager()->getLuaStack();

	luaStack->getLuaGlobal(m_prototype.c_str());
	luaStack->getLuaField(-1, "setUniqueID");
	//放入参数
	luaStack->getLuaGlobal(m_prototype.c_str());
	luaStack->pushInt(uniqueID);
	//执行
	luaStack->executeFunction(2, 0);

	luaStack->pop(1);
}

Value Enemy::getValueForKey(const string& key, Value::Type type) const
{
	LuaStack* luaStack = GameScene::getInstance()->getScriptManager()->getLuaStack();
	Value value;

	luaStack->getLuaGlobal(m_tableName.c_str());
	int ret = luaStack->getLuaField(-1, key.c_str());
	//存在对应的字段，才获取
	if (ret != LUA_TNIL)
	{
		switch (type)
		{
		case Value::Type::BOOLEAN: value = Value(luaStack->toLuaBoolean(-1)); break;
		case Value::Type::INTEGER: value = Value(luaStack->checkLuaInt(-1)); break;
		case Value::Type::FLOAT: value = Value(luaStack->checkLuaFloat(-1)); break;
		case Value::Type::STRING: value = Value(luaStack->checkLuaString(-1)); break;
		}
	}

	luaStack->pop(2);

	return value;
}