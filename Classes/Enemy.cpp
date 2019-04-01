#include "Enemy.h"
#include "GameScene.h"
#include "DynamicData.h"
#include "StaticData.h"

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
	m_parentTableName = enemyName;
	
	string variableName;
	//获取唯一id
	m_nUniqueID = this->getUniqueID(true);
	//获取变量名
	variableName = StringUtils::format("%s%02d",enemyName.c_str(), m_nUniqueID);

	string codes = StringUtils::format("%s=%s:new()", variableName.c_str(), enemyName.c_str());
	//执行代码
	GameScene::getInstance()->executeString(codes.c_str());
	//设置变量名称
	m_tableName = variableName;
	//设置透明级联
	this->setCascadeOpacityEnabled(true);

	return true;
}

bool Enemy::initialize()
{
	auto gameScene = GameScene::getInstance();
	//获取函数
	gameScene->getLuaGlobal(m_tableName.c_str());
	gameScene->getLuaField(-1, "initialize");
	//放入参数
	gameScene->getLuaGlobal(m_tableName.c_str());
	gameScene->pushInt(Object::getUniqueID());
	//执行
	gameScene->executeFunction(2, 0);

	gameScene->pop(1);
	//如果在初始化函数中没有赋值，则默认赋值为对应的table
	if (m_chartletName.empty())
	{
		m_chartletName = m_parentTableName;
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
	auto duration = 0.f;
	Animation* animation = nullptr;
	animation = StaticData::getInstance()->getSVAnimation(m_chartletName, state);
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
			m_fightState = FightState::Walk;
			animation = StaticData::getInstance()->getSVAnimation(m_chartletName, m_fightState);
		}
	}

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
	FightState state = FightState::Walk;
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
		state = FightState::Walk;
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

string Enemy::getTurnFilename() const
{
	return this->getValueForKey("turnName", Value::Type::STRING).asString();
}

void Enemy::execute(Fighter* fighter)
{
	auto gameScene = GameScene::getInstance();

	gameScene->getLuaGlobal(m_tableName.c_str());
	gameScene->getLuaField(-1, "execute");
	//移除table
	gameScene->removeLuaIndex(-2);
	//放入参数
	gameScene->getLuaGlobal(m_tableName.c_str());
	//执行
	int ret = gameScene->resumeFunction(1);
}

void Enemy::good(Good* good)
{
}

void Enemy::victory()
{
	this->changeFightState(FightState::Victory);
}

void Enemy::clean()
{
	auto gameScene = GameScene::getInstance();

	if (gameScene != nullptr)
	{
		//重置唯一ID
		this->setUniqueID(0);
		//获得函数
		gameScene->getLuaGlobal(m_tableName.c_str());
		gameScene->getLuaField(-1, "clean");
		gameScene->removeLuaIndex(-2);
		//放入参数
		gameScene->getLuaGlobal(m_tableName.c_str());
		//执行函数
		gameScene->executeFunction(1, 0);
	}
}

PropertyStruct Enemy::getTotalProperties() const
{
	//TODO
	PropertyStruct propStruct;

	propStruct.hp = this->getProperty(PropertyType::Hp);
	propStruct.mp = this->getProperty(PropertyType::Mp);
	propStruct.attack = this->getProperty(PropertyType::Attack);
	propStruct.defense = this->getProperty(PropertyType::Defense);
	propStruct.magicAttack = this->getProperty(PropertyType::MagicAttack);
	propStruct.magicDefense = this->getProperty(PropertyType::MagicDefense);
	propStruct.agility = this->getProperty(PropertyType::Agility);
	propStruct.luck = this->getProperty(PropertyType::Luck);

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

	auto gameScene = GameScene::getInstance();
	auto value = this->getValueForKey(key, Value::Type::INTEGER);
	

	return value.asInt();
}

void Enemy::setProperty(PropertyType type, int value)
{
	auto& array = STATIC_DATA_ARRAY("enemy_prop_func");
	int index = static_cast<int>(type);
	auto funcName = array.at(index).asString();

	auto gameScene = GameScene::getInstance();
	//获取函数
	gameScene->getLuaGlobal(m_tableName.c_str());
	gameScene->getLuaField(-1, funcName.c_str());
	//放入参数
	gameScene->getLuaGlobal(m_tableName.c_str());
	gameScene->pushInt(value);
	//执行
	gameScene->executeFunction(2, 0);

	gameScene->pop(1);
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
	auto gameScene = GameScene::getInstance();

	gameScene->getLuaGlobal(m_tableName.c_str());
	gameScene->getLuaField(-1, "hurt");
	//放入参数
	gameScene->getLuaGlobal(m_tableName.c_str());
	gameScene->pushInt(afterDamage);
	//执行
	gameScene->executeFunction(2, 1);

	gameScene->pop(1);

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
	auto gameScene = GameScene::getInstance();

	gameScene->getLuaGlobal(m_parentTableName.c_str());
	gameScene->getLuaField(-1, "getUniqueID");
	//放入参数
	gameScene->getLuaGlobal(m_parentTableName.c_str());
	gameScene->pushBoolean(bAdding);
	//执行
	gameScene->executeFunction(2, 1);
	//获取结果
	int uniqueID = gameScene->checkLuaInt(-1);

	gameScene->pop(2);

	return uniqueID;
}

void Enemy::setUniqueID(int uniqueID)
{
	auto gameScene = GameScene::getInstance();

	gameScene->getLuaGlobal(m_parentTableName.c_str());
	gameScene->getLuaField(-1, "setUniqueID");
	//放入参数
	gameScene->getLuaGlobal(m_parentTableName.c_str());
	gameScene->pushInt(uniqueID);
	//执行
	gameScene->executeFunction(2, 0);

	gameScene->pop(1);
}

Value Enemy::getValueForKey(const string& key, Value::Type type) const
{
	auto gameScene = GameScene::getInstance();
	Value value;

	gameScene->getLuaGlobal(m_tableName.c_str());
	int ret = gameScene->getLuaField(-1, key.c_str());
	//存在对应的字段，才获取
	if (ret != LUA_TNIL)
	{
		switch (type)
		{
		case Value::Type::BOOLEAN: value = Value(gameScene->toLuaBoolean(-1)); break;
		case Value::Type::INTEGER: value = Value(gameScene->checkLuaInt(-1)); break;
		case Value::Type::FLOAT: value = Value(gameScene->checkLuaFloat(-1)); break;
		case Value::Type::STRING: value = Value(gameScene->checkLuaString(-1)); break;
		}
	}

	gameScene->pop(2);

	return value;
}