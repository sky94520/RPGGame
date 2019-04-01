#ifndef __DynamicData_H__
#define __DynamicData_H__
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include "SDL_Engine/SDL_Engine.h"
using namespace std;
USING_NS_SDL;

class Good;
enum class EquipmentType;
class Character;

/*属性类型*/
enum class PropertyType
{
	Hp,
	Mp,
	Attack,
	Defense,
	MagicAttack,
	MagicDefense,
	Agility,
	Luck,
};
/*属性结构体*/
typedef struct PropertyStruct
{
	int hp;
	int mp;
	int attack;
	int defense;
	int magicAttack;
	int magicDefense;
	int agility;
	int luck;
public:
	PropertyStruct();
	PropertyStruct(int hp,int mp,int attack,int defense,int magicAttack,int magicDefense,int agility,int luck);

	PropertyStruct operator-(const PropertyStruct& propStruct);
	PropertyStruct& operator-=(const PropertyStruct& propStruct);
	PropertyStruct& operator+=(const PropertyStruct& propStruct);
	PropertyStruct operator+(const PropertyStruct& propStruct);
}PropertyStruct;

struct PlayerStruct
{
	map<EquipmentType, Good*> equipments;
	vector<Good*> skills;
	//属性
	PropertyStruct properties;
	//名称
	string name;
	int maxHp;
	int maxMp;
	int level;
	int exp;
public:
	PlayerStruct();
	PlayerStruct(const PropertyStruct& properties);
};
//--------------------------------------------DynamicData---------------------------------------
class DynamicData : public Object
{
private:
	static DynamicData* s_pInstance;
public:
	static DynamicData* getInstance();
	static void purge();
private:
	DynamicData();
	~DynamicData();
private:
	//存档
	ValueMap m_valueMap;
	//描述存档数据
	ValueMap m_descMap;
	//存档名称
	string m_filename;
	string m_descFilename;
	//存档索引
	int m_nSaveDataIndex;
	//是否第一次进入游戏
	bool m_bFirstGame;
	//金币数目
	int m_nGoldNumber;
	//出售比例
	float m_sellRatio;
	//背包物品列表
	vector<Good*> m_bagGoodList;
	vector<Good*> m_shopGoodList;
	vector<Good*> m_seedGoodList;
	//玩家数据
	map<string, PlayerStruct> m_playerStructs;
private:
	bool init();
public:
	//读取存档
	bool initializeSaveData(int idx);
	//保存数据
	bool save(const string& map,const Point& tilePos, int nDir);
	//--------------------------物品相关---------------------------
	//在背包中添加物品
	Good* addGood(const string& goodName,int number);
	//在背包中减少物品，如果不存在则不操作，并返回false。成功返回true
	bool subGood(const string& goodName,int number);
	bool subGood(Good* good, int number);
	//减少当前装备
	bool subEquipment(const string& playerName, const string& goodName, int number);
	//获取物品
	Good* getGood(const string& goodName);
	//更新物品，如果物品个数小于等于0，则删除
	void updateGood(Good* good);
	//金币
	int getGoldNumber() const;
	void setGoldNumber(int var);
	//添加商店物品
	void addShopGood(const string& goodName, int number);
	bool subShopGood(const string& goodName, int number);
	void subShopGood(Good* good,int number);
	//清除商店物品
	void clearShopGoods();
	//获取背包物品列表
	vector<Good*>& getBagList() { return m_bagGoodList; }
	vector<Good*>& getShopList() { return m_shopGoodList; }
	vector<Good*>& getSeedList() { return m_seedGoodList; }

	float getSellRatio() const { return m_sellRatio; }
	//--------------------------玩家相关---------------------------
	//设置玩家结构体
	void setPlayerStruct(Character* player, const ValueMap& valueMap);
	//血量
	int getMaxHitPoint(const string& playerName) const;
	void setMaxHitPoint(const string& playerName, int var);
	//魔法值
	int getMaxManaPoint(const string& playerName) const;
	void setMaxManaPoint(const string& playerName, int var);
	//获取经验值
	int getExp(const string& playerName) const;
	//设置经验值
	void setExp(const string& playerName,int var);
	//获取等级
	int getLevel(const string& playerName) const;
	void setLevel(const string& playerName,int var);
	//获取名称
	string& getName(const string& playerName);
	//获取对应的装备
	Good* getEquipment(const string& playerName, EquipmentType equipmentType);
	//装备
	void equip(const string& playerName, int uniqueId, Good* good);
	//卸下
	void unequip(const string& playerName, EquipmentType equipmentType);
	//装备叠加
	void overlyingEquipment(const string& playerName, Good* good, int number);
	//拆分装备
	bool splitEquipment(const string& playerName, EquipmentType type, Good* good, int number);
	//获取总属性 包括装备的加成
	PropertyStruct getTotalProperties(const string& playerName);
	//获取属性值
	PropertyStruct& getProperties(const string& playerName);
	//设置属性值
	void setProperties(const string& playerName, const PropertyStruct& st);
	//获得属性值 包括装备的属性
	int getProperty(const string& playerName,PropertyType type);
	//设置基础属性值
	void setProperty(const string& playerName,PropertyType type,unsigned int value);
	//学习技能
	bool studySkill(const string& playerName, const string& skillName);
	//获取对应角色的所以技能
	vector<Good*>& getSkills(const string& playerName);
	//--------------------------数据相关---------------------------
	//获取角色map
	ValueMap& getTotalValueMapOfCharacter();
	//获取地图文件路径
	string getMapFilePath() const;
	//获取角色对应位置
	Point getTileCoordinateOfPlayer() const;
	//获取管理存档数据
	ValueMap& getDescValueMap();
	//--------------------------数据库相关---------------------------
	//获取数据
	Value* getValueOfKey(const string& key);
	//设置数据
	void setValueOfKey(const string& key, Value& value);
	//移除数据
	bool removeKeyValue(const string& key);
	//--------------------------任务相关---------------------------
	//是否接受了任务
	bool isHaveQuest(const string& taskName) const;
	//添加任务
	bool addQuest(const string& taskName);
	//完成任务
	bool completeQuest(const string& taskName);
	//是否完成了任务
	bool isCompletedQuest(const string& taskName);
	vector<Value>& getAcceptedTaskList();
private:
	//更新存档 物品以及是否是技能
	void updateSaveData(ValueVector& array, Good* good, bool bSkill);
	//更新存档 角色数据
	void updateSaveData(const string& playerName, const string& propName, int value);
	//设置属性值
	void setProperties(const string& playerName, const ValueMap& propertyMap);
	//装备
	void equip(Character* player, const ValueVector& valueVector);
	//学习技能
	void studySkill(const string& playerName, const ValueVector& vec);
	//移除已接收任务
	bool removeAcceptedTask(const string& taskName);
};
#endif