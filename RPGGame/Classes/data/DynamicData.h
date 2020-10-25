#ifndef __DynamicData_H__
#define __DynamicData_H__

#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <SDL_Engine/SDL_Engine.h>

#include "UserRecord.h"

USING_NS_SDL;
using namespace std;

class Good;
struct PlayerData;

class DynamicData : public Object
{
public:
	static DynamicData* getInstance()
	{
		if (s_pInstance == nullptr)
		{
			s_pInstance = new DynamicData();
			s_pInstance->init();
		}
		return s_pInstance;
	}
	static void purge()
	{
		SDL_SAFE_RELEASE_NULL(s_pInstance);
	}
public:
	//读取存档
	bool initializeSaveData(int idx);
	//保存数据
	bool save(const string& map,const Point& tilePos, int nDir);
	
	int getMaxHitPoint(const string& playerName)const;
	void setMaxHitPoint(const string& playerName, int var);

	int getMaxManaPoint(const string& playerName)const;
	void setMaxManaPoint(const string& playerName, int var);

	Properties getTotalProperties(const string& playerName);
	void setTotalProperties(const string& playerName, const Properties& properties);

	int getProperty(const string& playerName, PropertyType type);
	void setProperty(const string& playerName, PropertyType type, unsigned int value);

	int getLevel(const string& playerName) const;
	void setLevel(const string& playerName, int var);

	int getExp(const string& playerName) const;
	void setExp(const string& playerName, int var);

	int getGoldNumber() const;
	void setGoldNumber(int number);

	float getSellRatio() const;

	bool studySkill(const string& playerName, const string& skillName);
	vector<Good*>& getSkills(const string& playerName);

	//获取玩家的装备
	Good* getEquipment(const string& playerName, EquipmentType equipmentType);
	bool removeEquipment(const string& playerName, const string& goodName, int number);
	//拆分装备
	bool splitEquipment(const string& playerName, EquipmentType type, Good* good, int number);
	void overlyingEquipment(const string& playerName, Good* good, int number);
	//添加物品到背包
	Good* addGood(const string& goodName, int number);
	void updateGood(Good* good);
	//获取背包中物品
	Good* getGood(const string& goodName);
	//移除背包中的物品
	bool removeGood(const string& goodName, int number);
	bool removeGood(Good* good, int number);
	//装备
	void equip(const string&playerName, int uniqueId, Good* good);
	void unequip(const string& playerName, EquipmentType equipmentType);

	//获取存档的所在地图名
	const string& getMapFilename() const;
	//获取角色的位置
	const Point& getTileCoordinate() const;
	//获取角色属性结构体
	const unordered_map<string, PlayerData*>& getTotalPlayerData() const;
	PlayerData* getPlayerData(const string& name) const;
	//获取背包物品
	const vector<Good*>& getBagGoodList() const { return m_pUserRecord->m_bagGoodList; }
private:
	DynamicData();
	~DynamicData();
	bool init();
private:
	static DynamicData* s_pInstance;
private:
	//存档名称
	string m_filename;
	//存档索引
	int m_nSaveDataIndex;
	//是否第一次进入游戏
	bool m_bFirstGame;
	UserRecord* m_pUserRecord;
};
#endif