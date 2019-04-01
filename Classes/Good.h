#ifndef __Good_H__
#define __Good_H__
#include <string>
#include "SDL_Engine/SDL_Engine.h"

using namespace std;
USING_NS_SDL;

struct PropertyStruct;
enum class PropertyType;
enum class FightState;
/*物品类型*/
enum class GoodType
{
	Item,/*道具*/
	Equipment,/*装备*/
	Skill,/*技能*/
	Seed,/*种子*/
};
/*装备类型*/
enum class EquipmentType
{
	Weapon,/*武器*/
	OffHand,/*副手*/
	Cloth,/*衣服*/
	Ornament,/*饰品*/
};
/*副手类型*/
enum class OffHandType
{
	Shield,/*盾牌*/
	Arrow,/*箭*/
};
//武器类型
enum class WeaponType
{
	BareHands,/*空手*/
	Dagger,/*匕首*/
	Sword,/*剑*/
	Flail,/*流星锤*/
	Axe,/*斧*/
	Whip,/*鞭*/
	Cane,/*杖*/
	Bow,/*弓箭*/
	Crossbow,/*弩*/
	Claw,/*爪*/
	Glove,/*拳套*/
	Spear,/*矛*/
};
/*使用范围*/
enum class UsageRange
{
	Self,/*对使用者*/
	SingleAlly,/*友方单体*/
	TotalAlly,/*友方全体*/
	SingleEnemy,/*敌方单体*/
	TotalEnemy,/*敌方全体*/
};
/*使用场合*/
enum class UsageOccasion
{
	Normal,/*在正常状态下*/
	Fighting,/*战斗时*/
	All,/*随时可用*/
};

class Good : public Object
{
	SDL_SYNTHESIZE_READONLY(string, m_tableName, TableName);//本物品所对应的table名
	SDL_SYNTHESIZE_READONLY(string, m_parentTableName, ParentTableName);//本物品所对应的父类table名
public:
	Good();
	virtual ~Good();

	static Good* create(const string& tableName);
	static Good* create(const string& tableName, int number);

	bool init(const string& tableName);
	bool init(const string& tableName, int number);
	//执行函数
	int execute(int userID, int targetID);
	//清除函数
	void clean();
	//获取物品对应的图标ID
	int getIconID() const;
	//获取物品名称
	string getName() const;
	//获取物品描述
	string getDescription() const;
	//是否是消耗品
	bool isDeleption() const;
	//获取/设置个数
	int getNumber() const;
	void setNumber(int number);
	//获取价钱
	int getCost() const;
	//获取物品类型
	GoodType getGoodType() const;
	//获取使用范围
	UsageRange getUsageRange() const;
	//获取使用场合
	UsageOccasion getUsageOccasion() const;
	/*-------------------装备相关---------------------*/
	EquipmentType getEquipmentType() const;
	//获取副手装备的类型
	OffHandType getOffHandType() const;
	//装备
	void equip(int ownerID);
	//卸下
	void unequip();
	//是否装备
	bool isEquiped() const;
	//获取属性
	int getProperty(PropertyType type) const;
	//获取该物品的属性结构体
	PropertyStruct getPropertyStruct() const;
	/*----------------武器相关--------------------------*/
	WeaponType getWeaponType()const;
	//获取武器对应的出手
	FightState getFightState()const;
	/*----------------技能相关--------------------------*/

private:
	int getUniqueID(bool bAdding);
	Value getValueForKey(const string& key, Value::Type type) const;
};
#endif