#ifndef __GameMacros_H__
#define __GameMacros_H__

#include <string>

using namespace std;

/*方向,跟贴图有关*/
enum class Direction
{
	Down = 0,
	Left,
	Right,
	Up,
};

/*人物行走状态*/
enum class State
{
	None,
	Idle,
	Walking,
};

//游戏状态
enum class GameState
{
	Normal,/*正常状态，即行走状态*/
	Fighting,/*战斗状态*/
	Script,/*脚本状态*/
};

/*脚本等待类型*/
enum class WaitType
{
	None,
	Time, //等待时间
	Click, //等待点击
	Button, //等待按钮按下
};

//脚本触发方式
enum class TriggerType
{
	None,
	Click,/*点击NPC触发*/
	Touch,/*触摸触发*/
	All,
};

/*优先级 */
#define PRIORITY_SAME 0 /*与人物能产生碰撞*/
#define PRIORITY_LOW  -1 /*NPC显示在下*/
#define PRIORITY_HIGH 1 /*NPC显示在上*/

const int CHARACTER_LOCAL_Z_ORDER = 999;

const string BATTLE_FIGHTER_DEAD_EVENT = "fighter dead";

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
typedef struct Properties
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
	Properties()
		:hp(0)
		,mp(0)
		,attack(0)
		,defense(0)
		,magicAttack(0)
		,magicDefense(0)
		,agility(0)
		,luck(0)
		{}
	Properties(int hp,int mp,int attack,int defense,int magicAttack,int magicDefense,int agility,int luck)
		:hp(hp)
		,mp(mp)
		,attack(attack)
		,defense(defense),
		magicAttack(magicAttack)
		,magicDefense(magicDefense)
		,agility(agility)
		,luck(luck){}

	Properties operator-(const Properties& propStruct)
	{
		Properties st;

		st.hp = hp - propStruct.hp;
		st.mp = mp - propStruct.mp;
		st.attack = attack - propStruct.attack;
		st.defense = defense - propStruct.defense;
		st.magicAttack = magicAttack - propStruct.magicAttack;
		st.magicDefense = magicDefense - propStruct.magicDefense;
		st.agility = agility - propStruct.agility;
		st.luck = luck - propStruct.luck;

		return st;
	}
	Properties& operator-=(const Properties& propStruct)
	{
		this->hp -= propStruct.hp;
		this->mp -= propStruct.mp;
		this->attack -= propStruct.attack;
		this->defense -= propStruct.defense;
		this->magicAttack -= propStruct.magicAttack;
		this->magicDefense -= propStruct.magicDefense;
		this->agility -= propStruct.agility;
		this->luck -= propStruct.luck;

		return *this;
	}
	Properties& operator+=(const Properties& propStruct)
	{
		this->hp += propStruct.hp;
		this->mp += propStruct.mp;
		this->attack += propStruct.attack;
		this->defense += propStruct.defense;
		this->magicAttack += propStruct.magicAttack;
		this->magicDefense += propStruct.magicDefense;
		this->agility += propStruct.agility;
		this->luck += propStruct.luck;

		return *this;
	}
	Properties operator+(const Properties& propStruct)
	{
		Properties st;

		st.hp = hp + propStruct.hp;
		st.mp = mp + propStruct.mp;
		st.attack = attack + propStruct.attack;
		st.defense = defense + propStruct.defense;
		st.magicAttack = magicAttack + propStruct.magicAttack;
		st.magicDefense = magicDefense + propStruct.magicDefense;
		st.agility = agility + propStruct.agility;
		st.luck = luck + propStruct.luck;

		return st;
	}
}Properties;

struct LevelUp
{
	int lv;
	int exp;
	Properties properties;
	string skill;
public:
	LevelUp():lv(0), exp(0) {}
};


struct PlayerData
{
	//装备
	//map<EquipmentType, Good*> equipments;
	//技能
	//vector<Good*> skills;
	//属性
	Properties properties;
	int maxHp;
	int maxMp;
	int level;
	int exp;
public:
	PlayerData()
		:level(1)
		,exp(0)
		,maxHp(0)
		,maxMp(0)
	{
	}

	PlayerData(const Properties& properties)
		:properties(properties)
		,level(1)
		,exp(0)
	{
		maxHp = properties.hp;
		maxMp = properties.mp;
	}
};
#endif