#include "DynamicData.h"
#include "UserRecord.h"

#include "../GameMacros.h"
#include "../entity/Good.h"

DynamicData* DynamicData::s_pInstance = nullptr;

DynamicData::DynamicData()
	:m_bFirstGame(true)
	,m_nSaveDataIndex(0)
	,m_pUserRecord(nullptr)
{
}

DynamicData::~DynamicData()
{
	SDL_SAFE_RELEASE_NULL(m_pUserRecord);
}

bool DynamicData::init()
{
	m_pUserRecord = UserRecord::create();
	SDL_SAFE_RETAIN(m_pUserRecord);

	return true;
}

bool DynamicData::initializeSaveData(int idx)
{
	//获取存档路径
	auto fileUtil = FileUtils::getInstance();
	string path = fileUtil->getWritablePath();
	string filepath = m_filename = StringUtils::format("%ssave%d.xml", path.c_str(), idx);
	//不存在对应存档，则使用默认存档
	if ( !fileUtil->isFileExist(m_filename))
	{
		filepath = "data/default_data.xml";
		m_bFirstGame = true;
	}
	else
		m_bFirstGame = false;

	m_nSaveDataIndex = idx;
	//读取文件 获取数据
	m_pUserRecord->readFromXML(filepath, m_bFirstGame);

	return true;
}

bool DynamicData::save(const string&map,const Point& tilePos, int nDir)
{
	return true;
}

int DynamicData::getMaxHitPoint(const string& playerName)const
{
	PlayerData* data = m_pUserRecord->players.at(playerName);
	return data->maxHp;
}

void DynamicData::setMaxHitPoint(const string& playerName, int var)
{
	PlayerData* data = m_pUserRecord->players.at(playerName);
	data->maxHp = var;
}

int DynamicData::getMaxManaPoint(const string& playerName)const
{
	PlayerData* data = m_pUserRecord->players.at(playerName);
	return data->maxMp;
}

void DynamicData::setMaxManaPoint(const string& playerName, int var)
{
	PlayerData* data = m_pUserRecord->players.at(playerName);
	data->maxMp = var;
}

Properties DynamicData::getTotalProperties(const string& playerName)
{
	Properties&& properties = m_pUserRecord->getTotalProperties(playerName);
	return properties;
}

void DynamicData::setTotalProperties(const string& playerName, const Properties& properties)
{
	PlayerData* data = m_pUserRecord->players.at(playerName);
	data->properties = properties;
}

int DynamicData::getProperty(const string& playerName, PropertyType type)
{
	return m_pUserRecord->getProperty(playerName, type);
}

void DynamicData::setProperty(const string& playerName, PropertyType type, unsigned int value)
{
	m_pUserRecord->setProperty(playerName, type, value);
}

int DynamicData::getLevel(const string& playerName) const
{
	PlayerData* data = m_pUserRecord->players.at(playerName);
	return data->level;
}

void DynamicData::setLevel(const string& playerName, int var)
{
	PlayerData* data = m_pUserRecord->players.at(playerName);
	data->level = var;
}

int DynamicData::getExp(const string& playerName) const
{
	PlayerData* data = m_pUserRecord->players.at(playerName);
	return data->exp;
}

void DynamicData::setExp(const string& playerName, int var)
{
	PlayerData* data = m_pUserRecord->players.at(playerName);
	data->exp = var;
}

int DynamicData::getGoldNumber() const
{
	return m_pUserRecord->goldNumber;
}

void DynamicData::setGoldNumber(int number)
{
	m_pUserRecord->goldNumber = number;
}

float DynamicData::getSellRatio() const
{
	return m_pUserRecord->sellRatio;
}

bool DynamicData::studySkill(const string& playerName, const string& skillName)
{
	return m_pUserRecord->studySkill(playerName, skillName);
}

vector<Good*>& DynamicData::getSkills(const string& playerName)
{
	PlayerData* data = m_pUserRecord->players.at(playerName);
	vector<Good*>& skills = data->skills;
	return skills;
}

Good* DynamicData::getEquipment(const string& playerName, EquipmentType equipmentType)
{
	PlayerData* data = m_pUserRecord->players.at(playerName);
	auto& equipments = data->equipments;
	//获取用户
	auto iter = equipments.find(equipmentType);

	if (iter == equipments.end())
		return nullptr;
	else
		return iter->second;
}

bool DynamicData::removeEquipment(const string& playerName, const string& goodName, int number)
{
	bool ret = false;
	PlayerData* data = m_pUserRecord->players[playerName];
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

bool DynamicData::splitEquipment(const string& playerName, EquipmentType type, Good* good, int number)
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

void DynamicData::overlyingEquipment(const string& playerName, Good* good, int number)
{
	good->setNumber(good->getNumber() + number);
}

Good* DynamicData::addGood(const string& goodName, int number)
{
	Good* good = nullptr;
	vector<Good*>& goodList = m_pUserRecord->m_bagGoodList;
	//是否存在该物品
	auto it = find_if(goodList.begin(), goodList.end(), [&goodName](Good* good)
	{
		return good->getPrototype() == goodName;
	});
	//背包中存在该物品
	if (it != goodList.end())
	{
		good = *it;
		good->setNumber(good->getNumber() + number);
	}//背包中不存在该物品，创建
	else
	{
		good = Good::create(goodName, number);
		SDL_SAFE_RETAIN(good);
		goodList.push_back(good);
	}
	return good;
}

void DynamicData::updateGood(Good* good)
{
	if (good->getNumber() > 0)
		return;
	//移除
	vector<Good*>& goodList = m_pUserRecord->m_bagGoodList;
	auto it = find(goodList.begin(), goodList.end(), good);
	goodList.erase(it);
	SDL_SAFE_RELEASE(good);
}

Good* DynamicData::getGood(const string& goodName)
{
	return m_pUserRecord->getGood(goodName);
}

bool DynamicData::removeGood(const string& goodName, int number)
{
	bool ret = false;
	vector<Good*>& goodList = m_pUserRecord->m_bagGoodList;
	//背包中是否存在该物品
	auto it = find_if(goodList.begin(), goodList.end(), [goodName](Good*good)
	{
		return good->getPrototype() == goodName;
	});
	//背包中不存在该物品
	if (it == goodList.end())
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
		goodList.erase(it);

		SDL_SAFE_RELEASE(good);
		ret = true;
	}
	//操作成功，才进行存档更新
	SDL_SAFE_RELEASE(good);
	return ret;
}

bool DynamicData::removeGood(Good* good, int number)
{
	vector<Good*>& goodList = m_pUserRecord->m_bagGoodList;
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
		auto it = find_if(goodList.begin(), goodList.end(), [good](Good* g)
		{
			return good == g;
		});
		if (it != goodList.end())
		{
			goodList.erase(it);
			SDL_SAFE_RELEASE(good);

			ret = true;
		}
	}
	return ret;
}

void DynamicData::equip(const string&playerName, int uniqueId, Good* good)
{
	m_pUserRecord->equip(playerName, uniqueId, good);
}

void DynamicData::unequip(const string& playerName, EquipmentType equipmentType)
{
	m_pUserRecord->unequip(playerName, equipmentType);
}

const string& DynamicData::getMapFilename() const
{
	return m_pUserRecord->mapFilename;
}

const Point& DynamicData::getTileCoordinate() const
{
	return m_pUserRecord->tileCoordinate;
}

const unordered_map<string, PlayerData*>&  DynamicData::getTotalPlayerData() const
{
	return m_pUserRecord->players;
}

PlayerData* DynamicData::getPlayerData(const string& name) const
{
	auto iter = m_pUserRecord->players.find(name);
	if (iter == m_pUserRecord->players.cend())
		return nullptr;
	else
		return iter->second;
}
