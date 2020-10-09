#ifndef __Fighter_H__
#define __Fighter_H__

#include "Entity.h"
#include "../GameMacros.h"

struct Properties;
class Good;

/*16种战斗状态*/
enum class FightState
{
	None = -1,
	Idle,
	Thrust,
	Escape,
	Wait,
	Swing,
	Victory,
	Chant,
	Missile,
	Dying,
	Guard,
	Skill,
	Abnormal,
	Damage,
	Spell,
	Sleep,
	Evade,
	Item,
	Dead,
};
/*属性类型*/
enum class AttributeType
{
	None,//无属性
	Fire,//火属性
};
/*受伤类型*/
enum class HurtType
{
	None,
	Attack,
	MagicAttack,
	CriticalAttack,/*暴击物理伤害*/
};

/*基类，主要用于派生出Actor（主角团）和Enemy（敌人）*/
class Fighter : public Entity
{
	SDL_SYNTHESIZE(float, m_attackScale, AttackScale);//攻击系数
	SDL_SYNTHESIZE(float, m_defenseRate, DefenseRate);//防御倍率
	SDL_SYNTHESIZE(string, m_chartletName, ChartletName);
protected:
	bool m_bGuarding;//是否防御
	bool m_bDead;//是否死亡
	int m_nUniqueID;//唯一ID
	//战斗动画状态
	FightState m_fightState;
	FightState m_lastFightState;
private:
	Point m_lastPos;//上一个位置，用于近战攻击后的返回
	static const float SPEED; //移动速度(近战靠近)
public:
	Fighter();
	virtual ~Fighter();
	/**
	* 受伤 返回实际伤害值
	* @param attack 攻击力
	* @param attrType 攻击属性类型
	* @param hurtType 受伤类型（无属性、物理攻击、魔法攻击、暴击物理攻击）
	*/
	int hurt(int attack, AttributeType attrType, HurtType hurtType);
	//移动至目标 and 返回
	float moveToTarget(Fighter* fighter);
	float backTo();
	//死亡，一般不主动调用
	virtual void dead();
	//获取速度 = 敏捷*常量
	int getSpeed() const { return getProperty(PropertyType::Agility) * 5; }
	//是否将要死亡
	bool isDying() const { return getProperty(PropertyType::Hp) <= 0; }
	//是否死亡
	bool isDead() const { return m_bDead; }
	//切换状态
	virtual FiniteTimeAction* changeFightState(FightState state);
	//更新状态
	virtual void updateSelf() = 0;
	//逃跑
	virtual void escape();
	//防御
	virtual void guard();
	//获取战斗唯一id 如果是主角，则与character对应
	virtual int getFighterID() const = 0;
	//名称(战斗时所显示的名称)
	virtual string getFighterName() const = 0;
	//缩略图(用于出手顺序的展示)
	virtual string getThumbnail() const = 0;
	//脚本执行时调用
	virtual void execute(Fighter* fighter) = 0;
	//使用道具
	virtual void useItem(Good* good) = 0;
	//胜利
	virtual void victory() = 0;
	//清除对应的table
	virtual void clean() = 0;
	//获取总属性
	virtual Properties* getTotalProperties() const = 0;
	//血量
	virtual int getMaxHitPoint() const = 0;
	virtual void setMaxHitPoint(int var) = 0;
	//魔法值
	virtual int getMaxManaPoint() const = 0;
	virtual void setMaxManaPoint(int var) = 0;
	//获取属性
	virtual int getProperty(PropertyType type) const = 0;
	virtual void setProperty(PropertyType type, int value) = 0;
	//获取命中率
	virtual float getHitRate() const = 0;
	//获取躲避率
	virtual float getEvasionRate() const = 0;
	//获取暴击几率
	virtual float getCriticalRate() const = 0;
protected:
	//受伤后回调函数
	virtual void onHurt(int afterDamage) = 0;
};
#endif