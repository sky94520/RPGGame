#include "StaticData.h"
#include "../GameMacros.h"
#include "CharacterData.h"

StaticData* StaticData::s_pInstance = nullptr;

StaticData* StaticData::getInstance()
{
	if (s_pInstance == nullptr)
	{
		s_pInstance = new StaticData();
		s_pInstance->init();
	}
	return s_pInstance;
}

void StaticData::purge()
{
	SDL_SAFE_RELEASE_NULL(s_pInstance);
}

StaticData::StaticData()
	:m_pCharacterData(nullptr)
	,m_pAStar(nullptr)
{
}

StaticData::~StaticData()
{
	SDL_SAFE_RELEASE_NULL(m_pCharacterData);
	SDL_SAFE_RELEASE_NULL(m_pAStar);
}

bool StaticData::init()
{
	//读取文件并保存键值对
	m_valueMap = FileUtils::getInstance()->getValueMapFromFile(STATIC_DATA_PATH);

	m_pCharacterData = CharacterData::create();
	SDL_SAFE_RETAIN(m_pCharacterData);
	m_pCharacterData->loadCharacterFile("data/character.plist");
	
	return true;
}

string StaticData::toString(PropertyType type) const
{
	//根据属性的名称获取属性的字符串
	auto array = STATIC_DATA_ARRAY("prop_array");
	int index = static_cast<int>(type);

	return array.at(index).asString();
}

Value* StaticData::getValueForKey(const string& key)
{
	auto iter = m_valueMap.find(key);

	if(iter != m_valueMap.end())
		return &iter->second;

	return nullptr;
}

Point StaticData::getPointForKey(const string& key)
{
	Point point;

	auto value = this->getValueForKey(key);

	if (value != nullptr)
	{
		point = PointFromString(value->asString());
	}
	return point;
}

Size StaticData::getSizeForKey(const string& key)
{
	Size size;

	auto value = this->getValueForKey(key);

	if (value != nullptr)
	{
		size = SizeFromString(value->asString());
	}
	return size;
}
