#ifndef __StaticData_H__
#define __StaticData_H__
#include <string>
#include <vector>
#include <functional>
#include <sstream>
#include <SDL_Engine/SDL_Engine.h>

#include "lua.hpp"
#include "../entity/AStar.h"
#include "../dist/json/json.h"

using namespace std;
USING_NS_SDL;

class CharacterData;
enum class PropertyType;

//定义一些常用的宏
#define STATIC_DATA_PATH "data/static_data.json"
#define STATIC_DATA_STRING(key) (StaticData::getInstance()->getValueForKey(key).asString())

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
	const Json::Value& getValueForKey(const string& key);
	Point getPointForKey(const string& key);
	Size getSizeForKey(const string& key);

	Animation* getAnimation(const string& animationName);

	string toString(PropertyType type);
	//icon
	SpriteFrame* getIconSpriteFrame(int iconId)const;
	SpriteFrame* getFaceSpriteFrame(const string& chartletName);
private:
	StaticData();
	~StaticData();
	bool init();
private:
	static StaticData* s_pInstance;
private:
	//键值对
	Json::Value m_jsonData;
	//A星算法
	SDL_SYNTHESIZE_RETAIN(AStar*, m_pAStar, AStar);
	SDL_SYNTHESIZE_READONLY(CharacterData*, m_pCharacterData, CharacterData);
};
#endif
