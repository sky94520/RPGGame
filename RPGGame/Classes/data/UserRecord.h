#ifndef __UserRecord_H__
#define __UserRecord_H__

#include <vector>
#include <string>
#include <cstring>
#include <unordered_map>
#include <SDL_Engine/SDL_Engine.h>

#include "../GameMacros.h"

USING_NS_SDL;
using namespace std;

class Good;
class Character;
struct PlayerData;
class Archive;

class UserRecord : public Object
{
public:
	UserRecord();
	~UserRecord();
	CREATE_FUNC(UserRecord);

	//从文件中读取数据
	bool read(const string& filename, bool bFirstGame);
	bool write(const string& filename, const string&mapName, const Point& tilePos);

	Properties getTotalProperties(const string& playerName);
	//装备
	void equip(const string&playerName, int uniqueId, Good* good);
	void unequip(const string& playerName, EquipmentType equipmentType);
	//技能
	bool studySkill(const string& playerName, const string& skillName);
	//属性
	int getProperty(const string& playerName, PropertyType type);
	void setProperty(const string& playerName, PropertyType type, unsigned int value);
	//获取玩家的装备
	Good* getEquipment(const string& playerName, EquipmentType equipmentType);
	bool removeEquipment(const string& playerName, const string& goodName, int number);
	//拆分和添加装备
	bool splitEquipment(const string& playerName, EquipmentType type, Good* good, int number);
	void overlyingEquipment(const string& playerName, Good* good, int number);
	//获取物品
	Good* getGood(const string& goodName);
	//添加物品到背包
	Good* addGood(const string& goodName, int number);
	void updateGood(Good* good);
	//移除背包中的物品
	bool removeGood(const string& goodName, int number);
	bool removeGood(Good* good, int number);
public:
	//金币数量
	int goldNumber;
	//出售比例
	float sellRatio;
	//地图
	string mapFilename;
	//玩家位置
	Point tileCoordinate;
	//玩家数据
	unordered_map<string, PlayerData*> players;
	//玩家背包物品
	vector<Good*> bagGoodList;
private:
	Archive* m_pArchive;
};
#endif