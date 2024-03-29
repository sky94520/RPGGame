#include "Good.h"
#include "GameScene.h"
#include "DynamicData.h"
#include "StaticData.h"
#include "Actor.h"

Good::Good()
{
}

Good::~Good()
{
}

Good* Good::create(const string& tableName)
{
	auto good = new Good();

	if (good && good->init(tableName))
		good->autorelease();
	else
		SDL_SAFE_DELETE(good);

	return good;
}

Good* Good::create(const string& tableName, int number)
{
	auto good = new Good();

	if (good && good->init(tableName, number))
		good->autorelease();
	else
		SDL_SAFE_DELETE(good);

	return good;
}

bool Good::init(const string& tableName)
{
	return Good::init(tableName, 1);
}

bool Good::init(const string& tableName, int number)
{
	m_parentTableName = tableName;

	int uniqueID = this->getUniqueID(true);
	//获取变量名
	m_tableName = StringUtils::format("%s%02d", m_parentTableName.c_str(), uniqueID);
	//执行获得table
	string codes = StringUtils::format("%s=%s:new()", m_tableName.c_str(), m_parentTableName.c_str());
	//执行代码
	GameScene::getInstance()->executeString(codes.c_str());
	//设置个数
	this->setNumber(number);

	return true;
}

int Good::execute(int userID, int targetID)
{
	auto gameScene = GameScene::getInstance();
	//获取函数
	gameScene->getLuaGlobal(m_tableName.c_str());
	gameScene->getLuaField(-1, "execute");
	//移除table
	gameScene->removeLuaIndex(-2);
	//放入参数
	gameScene->getLuaGlobal(m_tableName.c_str());
	gameScene->pushInt(userID);
	gameScene->pushInt(targetID);
	//执行函数
	int ret = gameScene->resumeFunction(3);

	return ret;
}

void Good::clean()
{
	auto gameScene = GameScene::getInstance();
	//获取函数
	gameScene->getLuaGlobal(m_tableName.c_str());
	gameScene->getLuaField(-1, "clean");
	//放入参数
	gameScene->getLuaGlobal(m_tableName.c_str());

	gameScene->executeFunction(2, 0);
}

int Good::getIconID() const
{
	Value value = this->getValueForKey("iconID", Value::Type::INTEGER);

	return value.asInt();
}

string Good::getName() const
{
	Value value = this->getValueForKey("name", Value::Type::STRING);

	return value.asString();
}

string Good::getDescription() const
{
	Value value = this->getValueForKey("descrition", Value::Type::STRING);

	return value.asString();
}

bool Good::isDeleption() const
{
	Value value = this->getValueForKey("deleption", Value::Type::BOOLEAN);

	return value.asBool();
}

int Good::getNumber() const
{
	Value value = this->getValueForKey("number", Value::Type::INTEGER);

	return value.asInt();
}

void Good::setNumber(int number)
{
	auto gameScene = GameScene::getInstance();
	//获取函数
	gameScene->getLuaGlobal(m_tableName.c_str());
	gameScene->getLuaField(-1, "setNumber");
	//压入参数
	gameScene->getLuaGlobal(m_tableName.c_str());
	gameScene->pushInt(number);
	//执行函数
	gameScene->executeFunction(2, 0);

	gameScene->pop(1);
}

int Good::getCost() const
{
	Value value = this->getValueForKey("cost", Value::Type::INTEGER);

	return value.asInt();
}

GoodType Good::getGoodType() const
{
	Value value = this->getValueForKey("goodType", Value::Type::INTEGER);
	GoodType type = static_cast<GoodType>(value.asInt());

	return type;
}

UsageRange Good::getUsageRange() const
{
	Value value = this->getValueForKey("usageRange", Value::Type::INTEGER);
	UsageRange type = static_cast<UsageRange>(value.asInt());

	return type;
}

UsageOccasion Good::getUsageOccasion() const
{
	Value value = this->getValueForKey("usageOccasion", Value::Type::INTEGER);
	UsageOccasion type = static_cast<UsageOccasion>(value.asInt());

	return type;
}
/*-------------------装备相关---------------------*/
EquipmentType Good::getEquipmentType() const
{
	auto value = this->getValueForKey("equipmentType",Value::Type::INTEGER);
	auto type = static_cast<EquipmentType>(value.asInt());

	return type;
}

OffHandType Good::getOffHandType() const
{
	auto value = this->getValueForKey("offHandType",Value::Type::INTEGER);
	auto type = static_cast<OffHandType>(value.asInt());

	return type;
}

void Good::equip(int ownerID)
{
	auto gameScene = GameScene::getInstance();
	//获取函数
	gameScene->getLuaGlobal(m_tableName.c_str());
	gameScene->getLuaField(-1, "equip");
	//放入参数
	gameScene->getLuaGlobal(m_tableName.c_str());
	gameScene->pushInt(ownerID);
	//执行函数
	gameScene->executeFunction(2, 0);

	gameScene->pop(1);
}

void Good::unequip()
{
	auto gameScene = GameScene::getInstance();
	//获取函数
	gameScene->getLuaGlobal(m_tableName.c_str());
	gameScene->getLuaField(-1, "equip");
	//放入参数
	gameScene->getLuaGlobal(m_tableName.c_str());
	//执行函数
	gameScene->executeFunction(1, 0);

	gameScene->pop(1);
}

bool Good::isEquiped() const
{
	//获取键
	auto value = this->getValueForKey("ownerID", Value::Type::INTEGER);
	auto type = value.getType();

	if (type == Value::Type::NONE)
		return false;
	else if(value.asInt() == -1)
		return false;

	return true;
}

int Good::getProperty(PropertyType type) const
{
	//获取键
	auto key = STATIC_DATA_TOSTRING(type);
	auto value = this->getValueForKey(key,Value::Type::INTEGER);

	return value.asInt();
}

PropertyStruct Good::getPropertyStruct() const
{
	//TODO
	PropertyStruct propStruct;
	//只有在是装备时才会获取
	auto goodType = this->getGoodType();

	if (goodType != GoodType::Equipment)
		return propStruct;

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

WeaponType Good::getWeaponType()const
{
	auto value = this->getValueForKey("weaponType",Value::Type::INTEGER);
	auto type = static_cast<WeaponType>(value.asInt());

	return type;
}

FightState Good::getFightState()const
{
	auto gameScene = GameScene::getInstance();
	//出手状态 默认为item
	FightState fightState = FightState::Item;
	//获取函数
	gameScene->getLuaGlobal(m_parentTableName.c_str());
	int ret = gameScene->getLuaField(-1, "getFightState");
	//存在该函数
	if (ret == LUA_TFUNCTION)
	{
		//放入参数
		gameScene->getLuaGlobal(m_parentTableName.c_str());

		gameScene->executeFunction(1, 1);
		//获取id
		fightState = static_cast<FightState>(gameScene->checkLuaInt(-1));
	}
	gameScene->pop(2);

	return fightState;
}

int Good::getUniqueID(bool bAdding)
{
	auto gameScene = GameScene::getInstance();
	//唯一ID
	int uniqueID = 0;
	//获取函数
	gameScene->getLuaGlobal(m_parentTableName.c_str());
	gameScene->getLuaField(-1, "getUniqueID");
	//放入函数
	gameScene->getLuaGlobal(m_parentTableName.c_str());
	gameScene->pushBoolean(true);

	gameScene->executeFunction(2, 1);
	//获取id
	uniqueID = gameScene->checkLuaInt(-1);

	gameScene->pop(2);

	return uniqueID;
}

Value Good::getValueForKey(const string& key, Value::Type type) const
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