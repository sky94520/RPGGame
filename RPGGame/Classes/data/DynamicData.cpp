#include "DynamicData.h"
#include "UserRecord.h"
#include "../GameMacros.h"

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
