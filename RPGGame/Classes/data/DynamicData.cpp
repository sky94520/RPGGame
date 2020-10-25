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
	m_pUserRecord->read(filepath, m_bFirstGame);

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
	return m_pUserRecord->getEquipment(playerName, equipmentType);
}

bool DynamicData::removeEquipment(const string& playerName, const string& goodName, int number)
{
	return m_pUserRecord->removeEquipment(playerName, goodName, number);
}

bool DynamicData::splitEquipment(const string& playerName, EquipmentType type, Good* good, int number)
{
	return m_pUserRecord->splitEquipment(playerName, type, good, number);
}

void DynamicData::overlyingEquipment(const string& playerName, Good* good, int number)
{
	m_pUserRecord->overlyingEquipment(playerName, good, number);
}

Good* DynamicData::addGood(const string& goodName, int number)
{
	return m_pUserRecord->addGood(goodName, number);
}

void DynamicData::updateGood(Good* good)
{
	m_pUserRecord->updateGood(good);
}

Good* DynamicData::getGood(const string& goodName)
{
	return m_pUserRecord->getGood(goodName);
}

bool DynamicData::removeGood(const string& goodName, int number)
{
	return m_pUserRecord->removeGood(goodName, number);
}

bool DynamicData::removeGood(Good* good, int number)
{
	return m_pUserRecord->removeGood(good, number);
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
