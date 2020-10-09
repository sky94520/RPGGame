#ifndef __StaticData_H__
#define __StaticData_H__
#include <string>
#include <vector>
#include <functional>
#include <sstream>

#include "SDL_Engine/SDL_Engine.h"
#include "../entity/AStar.h"

using namespace std;
USING_NS_SDL;

class CharacterData;
enum class PropertyType;

//定义一些常用的宏
#define STATIC_DATA_PATH "data/static_data.plist"
/*简化使用*/
#define STATIC_DATA_STRING(key) (StaticData::getInstance()->getValueForKey(key)->asString())
#define STATIC_DATA_INT(key) (StaticData::getInstance()->getValueForKey(key)->asInt())
#define STATIC_DATA_FLOAT(key) (StaticData::getInstance()->getValueForKey(key)->asFloat())
#define STATIC_DATA_BOOLEAN(key) (StaticData::getInstance()->getValueForKey(key)->asBool())
#define STATIC_DATA_POINT(key) (StaticData::getInstance()->getPointForKey(key))
#define STATIC_DATA_SIZE(key) (StaticData::getInstance()->getSizeForKey(key))
#define STATIC_DATA_ARRAY(key) (StaticData::getInstance()->getValueForKey(key)->asValueVector())
#define STATIC_DATA_TOSTRING(key) (StaticData::getInstance()->toString(key))

class StaticData : public Object
{
public:
	static StaticData* getInstance();
	static void purge();
public:
	/**
	@brief 根据键获取值
	@key 要查询的键
	@return 返回的值，如果不存在对应的值，则返回空Value
	*/
	Value* getValueForKey(const string& key);
	Point getPointForKey(const string& key);
	Size getSizeForKey(const string& key);

	string toString(PropertyType type) const;
	//icon
	SpriteFrame* getIconSpriteFrame(int iconId)const;
private:
	StaticData();
	~StaticData();
	bool init();
private:
	static StaticData* s_pInstance;
private:
	//键值对
	ValueMap m_valueMap;
	//A星算法
	SDL_SYNTHESIZE_RETAIN(AStar*, m_pAStar, AStar);
	SDL_SYNTHESIZE_READONLY(CharacterData*, m_pCharacterData, CharacterData);
};
#endif
