#ifndef __StaticData_H__
#define __StaticData_H__
#include <string>
#include <vector>
#include <functional>
#include <sstream>

#include "SDL_Engine/SDL_Engine.h"
#include "DynamicData.h"

using namespace std;
USING_NS_SDL;
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

/*方向,跟贴图有关*/
enum class Direction
{
	Down = 0,
	Left,
	Right,
	Up,
};
/*优先级 */
#define PRIORITY_SAME 0 /*与人物能产生碰撞*/
#define PRIORITY_LOW  1 /*NPC显示在下*/
#define PRIORITY_HIGH 2 /*NPC显示在上*/

class AStar;
enum class PropertyType;
enum class FightState;

struct LevelUpCsv
{
	int lv;
	int exp;
	PropertyStruct properties;
	string skill;
public:
	LevelUpCsv()
	:lv(0),exp(0){}
};

class StaticData : public Object
{
private:
	static StaticData* s_pInstance;
public:
	static StaticData* getInstance();
	static void purge();
private:
	//键值对
	ValueMap m_valueMap;
	//角色键值对
	ValueMap m_characterMap;
	//A*寻路算法
	AStar* m_pAStar;
	//角色升级属性值改变
	map<string,vector<LevelUpCsv> > m_levelUpCsvMap;
private:
	StaticData();
	~StaticData();
	bool init();
public:
	/**
	@brief 根据键获取值
	@key 要查询的键
	@return 返回的值，如果不存在对应的值，则返回空Value
	*/
	Value* getValueForKey(const string& key);
	Point getPointForKey(const string& key);
	Size getSizeForKey(const string& key);
	//加载角色数据以及加载所需要的图片并解析
	bool loadCharacterFile(const string& filename);
	//获取人物行走动画
	Animation* getWalkingAnimation(const string& chartletName, Direction direction);
	Animation* getWalkingAnimation(const string& filename, int index, Direction dir, float delay, int loops, bool restoreOriginalFrame);
	//获取立绘
	SpriteFrame* getFaceSpriteFrame(const string& filename, int index);
	SpriteFrame* getFaceSpriteFrame(const string& chartletName);
	//获取icon 图标
	SpriteFrame* getIconSpriteFrame(int id);
	//获取sv 战斗图
	Animation* getSVAnimation(const string& chartletName,FightState fightState);
	//获得动画
	Animation* getAnimation(const string& animationName);
	//获取turn name
	string getTurnFilename(const string& chartletName) const;
	//获取升级数据
	LevelUpCsv& getDataByLevel(const string& chartletName,int level);
	//获取A星算法
	AStar* getAStar() { return m_pAStar; }
	bool direction(Direction dir,string* sDir,Point* delta,Direction* oppsite);

	string toString(PropertyType type) const;
private:
	//添加角色战斗图并生成16状态动画
	bool addSVAnimation(const string& chartletName, const string& filename);
	//添加角色升级文件
	bool addLevelUpData(const string& chartletName, const string& filename);
	/*在纹理指定区域rect按照宽度切割,并返回*/
	vector<SpriteFrame*> splitTexture(Texture* texture, const Rect& rect ,float width);

};
#endif
