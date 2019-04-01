#ifndef __Fighter_H__
#define __Fighter_H__
#include "Entity.h"

enum class PropertyType;
struct PropertyStruct;
class Good;

/*16种战斗状态*/
enum class FightState
{
	None = -1,
	Walk,
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

class Fighter : public Entity
{
	SDL_SYNTHESIZE(float, m_attackScale, AttackScale);//攻击系数
	SDL_SYNTHESIZE(float, m_defenseRate, DefenseRate);//防御倍率
	SDL_SYNTHESIZE(string, m_chartletName, ChartletName);
protected:
	//是否防御
	bool m_bGuarding;
	bool m_bDead;
	int m_nUniqueID;
	//战斗状态
	FightState m_fightState;
	FightState m_lastFightState;
private:
	Point m_lastPos;//上一个位置，用于返回
	static const float SPEED;
public:
	Fighter();
	virtual ~Fighter();
	//受伤 返回实际伤害值
	int hurt(int, AttributeType, HurtType);
	//移动至目标
	float moveToTarget(Fighter* fighter);
	//返回
	float backTo();
	//死亡，一般不主动调用
	virtual void dead();
	//获取速度 = 敏捷*常量
	int getSpeed() const;
	//是否将要死亡
	bool isDying() const;
	//是否死亡
	bool isDead() const;
	//切换状态
	virtual FiniteTimeAction* changeFightState(FightState state);
	//更新状态
	virtual void updateSelf();
	//逃跑
	virtual void escape();
	//防御
	virtual void guard();
	//获取战斗唯一id 如果是主角，则与character对应
	virtual int getFighterID() const = 0;
	//名称生成
	virtual string getFighterName() const = 0;
	//出手顺序图
	virtual string getTurnFilename() const = 0;
	//脚本执行时调用
	virtual void execute(Fighter* fighter) = 0;
	//使用道具
	virtual void good(Good* good) = 0;
	//胜利
	virtual void victory() = 0;
	//清除对应的table
	virtual void clean() = 0;
	//获取总属性
	virtual PropertyStruct getTotalProperties() const = 0;
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
	virtual void onHurt(int afterDamage) = 0;
};
#endif