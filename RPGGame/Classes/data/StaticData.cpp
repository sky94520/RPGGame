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
	Json::CharReaderBuilder readerBuilder;
	unique_ptr<Json::CharReader>const jsonReader(readerBuilder.newCharReader());

	unique_ptr<char> uniqueStr = std::move(FileUtils::getInstance()->getUniqueDataFromFile(STATIC_DATA_PATH));
	const char* text = uniqueStr.get();

	Json::String errorMsg;
	//解析失败
	if (!jsonReader->parse(text, text + strlen(text), &m_jsonData, &errorMsg)) {
		LOG(errorMsg.c_str());
		return false;
	}
	//角色的战斗图、立绘等
	m_pCharacterData = CharacterData::create();
	SDL_SAFE_RETAIN(m_pCharacterData);
	m_pCharacterData->loadCharacterFile("data/character.json");
	
	return true;
}

string StaticData::toString(PropertyType type)
{
	//根据属性的名称获取属性的字符串
	const Json::Value& data = this->getValueForKey("prop_array");
	int index = static_cast<int>(type);

	return data[index].asString();
}

SpriteFrame* StaticData::getIconSpriteFrame(int id) const
{
	//获取IconSet
	Texture*texture = Director::getInstance()->getTextureCache()->getTextureForKey("img/system/IconSet.png");

	int width = 32;
	int height = 32;
	//row cols
	int row = id / 16;
	int col = id % 16;

	int x = width * col;
	int y = height * row;

	Rect rect = Rect(x, y, width, height);

	return SpriteFrame::createWithTexture(texture, rect);
}

SpriteFrame* StaticData::getFaceSpriteFrame(const string& chartletName)
{
	return m_pCharacterData->getFaceSpriteFrame(chartletName);
}

const Json::Value& StaticData::getValueForKey(const string& key)
{
	Json::Value& data = m_jsonData[key];
	return data;
}

Point StaticData::getPointForKey(const string& key)
{
	Point point;
	const Json::Value& value = this->getValueForKey(key);

	if (!value.isNull())
	{
		point = PointFromString(value.asString());
	}
	return point;
}

Size StaticData::getSizeForKey(const string& key)
{
	Size size;
	const Json::Value& value = this->getValueForKey(key);

	if (!value.isNull())
	{
		size = SizeFromString(value.asString());
	}
	return size;
}
