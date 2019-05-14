#include "DynamicData.h"
#include "UserRecord.h"

//--------------------------------------------DynamicData---------------------------------------
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
	auto fileUtil = FileUtils::getInstance();
	//获取存档路径
	string path = fileUtil->getWritablePath();
	//对应的存档完整路径
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
	m_pUserRecord->readFromXML(filepath);

	return true;
}

bool DynamicData::save(const string&map,const Point& tilePos, int nDir)
{
	return true;
}

const string& DynamicData::getMapFilename() const
{
	return m_pUserRecord->mapFilename;
}

const Point& DynamicData::getTileCoordinate() const
{
	return m_pUserRecord->tileCoordinate;
}
