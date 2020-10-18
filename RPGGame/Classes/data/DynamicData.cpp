#include "DynamicData.h"
#include "UserRecord.h"

#include "../GameMacros.h"
#include "../entity/Good.h"

DynamicData* DynamicData::s_pInstance = nullptr;

DynamicData* DynamicData::getInstance()
{
	if (s_pInstance == nullptr)
	{
		s_pInstance = new DynamicData();
		s_pInstance->init();
	}
	return s_pInstance;
}

void DynamicData::purge()
{
	SDL_SAFE_RELEASE_NULL(s_pInstance);
}

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

int DynamicData::getMaxManaPoint(const string& playerName)const
{
	PlayerData* data = m_pUserRecord->players.at(playerName);
	return data->maxMp;
}

Properties DynamicData::getTotalProperties(const string& playerName)
{
	return m_pUserRecord->getTotalProperties(playerName);
}

int DynamicData::getLevel(const string& playerName) const
{
	PlayerData* data = m_pUserRecord->players.at(playerName);
	return data->level;
}

int DynamicData::getExp(const string& playerName) const
{
	PlayerData* data = m_pUserRecord->players.at(playerName);
	return data->exp;
}

float DynamicData::getSellRatio() const
{
	return m_pUserRecord->sellRatio;
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
	PlayerData* data = m_pUserRecord->players[playerName];
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
