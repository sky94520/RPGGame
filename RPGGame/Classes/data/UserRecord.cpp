#include "UserRecord.h"
#include "Archive.h"

#include "../entity/Good.h"
#include "../entity/Character.h"

#include "../data/StaticData.h"

UserRecord::UserRecord()
	:goldNumber(0)
	,sellRatio(0.f)
{
	m_pArchive = new XMLArchive();
	m_pArchive->setUserRecord(this);
}

UserRecord::~UserRecord()
{
	auto it = players.begin();

	while (it != players.end())
	{
		PlayerData* data = it->second;
		//释放技能
		for (auto it = data->skills.begin(); it != data->skills.end(); ) {
			Good* good = *it;
			SDL_SAFE_RELEASE(good);
			it = data->skills.erase(it);
		}
		delete data;
		it = players.erase(it);
	}
	//释放背包物品
	for (auto it = bagGoodList.begin(); it != bagGoodList.end();)
	{
		auto good = *it;
		it = bagGoodList.erase(it);
		SDL_SAFE_RELEASE(good);
	}
	SDL_SAFE_DELETE(m_pArchive);
}

bool UserRecord::read(const string& filename, bool bFirstGame)
{
	return m_pArchive->read(filename, bFirstGame);
}

bool UserRecord::write(const string& filename)
{
	return m_pArchive->write(filename);
}

Properties UserRecord::getTotalProperties(const string& playerName)
{
	PlayerData* playerData = players.at(playerName);

	Properties result = playerData->properties;
	auto &equipmentMap = playerData->equipments;

	for (auto iterator : equipmentMap)
	{
		auto equipment = iterator.second;

		if (equipment == nullptr)
			continue;

		auto temp = equipment->getProperties();
		result = result + temp;
	}
	return result;
}

void UserRecord::equip(const string&playerName, int uniqueId, Good* good)
{
	PlayerData* playerData = players[playerName];

	auto equipmentType = good->getEquipmentType();
	auto& equipments = playerData->equipments;
	//原来装备不为空,卸载
	auto iter = equipments.find(equipmentType);
	if (iter != equipments.end())
	{
		auto oldGood = iter->second;
		oldGood->unequip();
		SDL_SAFE_RELEASE(oldGood);

		equipments.erase(iter);
	}
	//更换装备
	good->equip(uniqueId);
	SDL_SAFE_RETAIN(good);
	equipments.insert(make_pair(equipmentType, good));
}

void UserRecord::unequip(const string& playerName, EquipmentType equipmentType)
{
	PlayerData* data = players[playerName];
	auto &equipments = data->equipments;
	//获取用户
	auto iter = equipments.find(equipmentType);

	if (iter == equipments.end())
		return;
	auto good = iter->second;
	good->unequip();
	good->setNumber(good->getNumber() - 1);
	SDL_SAFE_RELEASE(good);

	equipments.erase(iter);
}

bool UserRecord::studySkill(const string& playerName, const string& skillName)
{
	//检测是否学习了该技能
	PlayerData* data = players[playerName];
	auto &skills = data->skills;

	auto it = find_if(skills.begin(), skills.end(), [skillName](Good* good)
	{
		return good->getPrototype() == skillName;
	});
	if (it != skills.end())
	{
		printf("the player has studied the skill\n");
		return false;
	}
	//学习技能
	Good* good = Good::create(skillName);
	skills.push_back(good);
	SDL_SAFE_RETAIN(good);
	return true;
}

int UserRecord::getProperty(const string& playerName, PropertyType type)
{
	auto properties = this->getTotalProperties(playerName);
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

void UserRecord::setProperty(const string& playerName, PropertyType type, unsigned int value)
{
	PlayerData* data = players[playerName];
	auto& properties = data->properties;

	switch (type)
	{
	case PropertyType::Hp:
	{
		//设置最大值限制
		if (data->maxHp < value)
			value = data->maxHp;
		properties.hp = value;
	}break;
	case PropertyType::Mp:
	{
		//设置最大值限制
		if (data->maxMp < value)
			value = data->maxMp;
		properties.mp = value;
	}break;
	case PropertyType::Attack:
		properties.attack = value;
		break;
	case PropertyType::Defense:
		properties.defense = value;
		break;
	case PropertyType::MagicAttack:
		properties.magicAttack = value;
		break;
	case PropertyType::MagicDefense:
		properties.magicDefense = value;
		break;
	case PropertyType::Agility:
		properties.agility = value;
		break;
	case PropertyType::Luck:
		properties.luck = value;
		break;
	default:
		break;
	}
}

Good* UserRecord::getEquipment(const string& playerName, EquipmentType equipmentType)
{
	PlayerData* data = this->players.at(playerName);
	auto& equipments = data->equipments;
	//获取用户
	auto iter = equipments.find(equipmentType);

	if (iter == equipments.end())
		return nullptr;
	else
		return iter->second;
}

bool UserRecord::removeEquipment(const string& playerName, const string& goodName, int number)
{
	bool ret = false;
	PlayerData* data = this->players[playerName];
	auto& equipments = data->equipments;

	auto it = find_if(equipments.begin(), equipments.end(), [&goodName](const pair<EquipmentType, Good*>& it)
	{
		return (it.second)->getTableName() == goodName;
	});
	//找到对应的装备
	if (it != equipments.end())
	{
		auto good = it->second;
		auto num = good->getNumber() - number;

		SDL_SAFE_RETAIN(good);

		if (num > 0)
		{
			good->setNumber(num);
		}
		else if (num == 0)
		{
			good->unequip();
			good->setNumber(0);
			SDL_SAFE_RELEASE(good);

			equipments.erase(it);
		}
		SDL_SAFE_RELEASE(good);
	}
	return ret;
}

bool UserRecord::splitEquipment(const string& playerName, EquipmentType type, Good* good, int number)
{
	SDL_SAFE_RETAIN(good);
	auto count = good->getNumber();
	bool ret = false;

	if (count > number)
	{
		good->setNumber(count - number);
		ret = true;
	}
	else if (count == number)
	{
		this->unequip(playerName, type);
		ret = true;
	}
	//添加拆分的物品到背包
	if (ret)
		this->addGood(good->getPrototype(), number);
	SDL_SAFE_RELEASE(good);

	return ret;
}

void UserRecord::overlyingEquipment(const string& playerName, Good* good, int number)
{
	good->setNumber(good->getNumber() + number);
}

Good* UserRecord::getGood(const string& goodName)
{
	Good* good = nullptr;
	auto it = find_if(bagGoodList.begin(), bagGoodList.end(), [&goodName](Good* good)
	{
		return good->getPrototype() == goodName;
	});
	if (it != bagGoodList.end())
	{
		good = *it;
	}
	return good;
}

Good* UserRecord::addGood(const string& goodName, int number)
{
	Good* good = nullptr;
	//是否存在该物品
	auto it = find_if(bagGoodList.begin(), bagGoodList.end(), [&goodName](Good* good)
	{
		return good->getPrototype() == goodName;
	});
	//背包中存在该物品
	if (it != bagGoodList.end())
	{
		good = *it;
		good->setNumber(good->getNumber() + number);
	}//背包中不存在该物品，创建
	else
	{
		good = Good::create(goodName, number);
		SDL_SAFE_RETAIN(good);
		bagGoodList.push_back(good);
	}
	return good;
}

void UserRecord::updateGood(Good* good)
{
	if (good->getNumber() > 0)
		return;
	//移除
	auto it = find(bagGoodList.begin(), bagGoodList.end(), good);
	bagGoodList.erase(it);
	SDL_SAFE_RELEASE(good);
}

bool UserRecord::removeGood(const string& goodName, int number)
{
	bool ret = false;
	//背包中是否存在该物品
	auto it = find_if(bagGoodList.begin(), bagGoodList.end(), [goodName](Good*good)
	{
		return good->getPrototype() == goodName;
	});
	//背包中不存在该物品
	if (it == bagGoodList.end())
	{
		return false;
	}
	auto good = *it;
	auto oldNumber = good->getNumber();
	SDL_SAFE_RETAIN(good);
	//存在足够的物品 扣除
	if (good->getNumber() > number)
	{
		good->setNumber(oldNumber - number);
		ret = true;
	}//直接删除
	else if (oldNumber == number)
	{
		good->setNumber(0);
		bagGoodList.erase(it);

		SDL_SAFE_RELEASE(good);
		ret = true;
	}
	//操作成功，才进行存档更新
	SDL_SAFE_RELEASE(good);
	return ret;
}

bool UserRecord::removeGood(Good* good, int number)
{
	bool ret = false;
	auto goodNum = good->getNumber();
	//个数足够
	if (goodNum > number)
	{
		good->setNumber(goodNum - number);
		ret = true;
	}
	else if (goodNum == number)
	{
		auto it = find_if(bagGoodList.begin(), bagGoodList.end(), [good](Good* g)
		{
			return good == g;
		});
		if (it != bagGoodList.end())
		{
			bagGoodList.erase(it);
			SDL_SAFE_RELEASE(good);

			ret = true;
		}
	}
	return ret;
}
