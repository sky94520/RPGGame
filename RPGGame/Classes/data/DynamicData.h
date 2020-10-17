#ifndef __DynamicData_H__
#define __DynamicData_H__

#include <unordered_map>
#include <string>
#include <vector>
#include <algorithm>
#include <SDL_Engine/SDL_Engine.h>

#include "UserRecord.h"

USING_NS_SDL;
using namespace std;

class Good;
struct PlayerData;

class DynamicData : public Object
{
public:
	static DynamicData* getInstance();
	static void purge();
public:
	//读取存档
	bool initializeSaveData(int idx);
	//保存数据
	bool save(const string& map,const Point& tilePos, int nDir);
	
	int getMaxHitPoint(const string& playerName)const;
	int getMaxManaPoint(const string& playerName)const;
	Properties getTotalProperties(const string& playerName);
	int getLevel(const string& playerName) const;
	int getExp(const string& playerName) const;
	vector<Good*>& getSkills(const string& playerName);
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