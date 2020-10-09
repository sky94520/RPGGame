#ifndef __Enemy_H__
#define __Enemy_H__
#include "Fighter.h"

class Good;

class Enemy : public Fighter
{
	SDL_SYNTHESIZE_READONLY(string, m_tableName, TableName);//敌人所对应的table名
	SDL_SYNTHESIZE_READONLY(string, m_prototype, Prototype);//本物品所对应的父类table名
public:
	Enemy();
	~Enemy();

	static Enemy*create(const string& enemyName);
	bool init(const string& enemyName);
	//调用lua对应函数
	bool initialize();
	//获取turn char TODO:待修改
	char getTurnChar() const;
	virtual void dead();
	//切换状态
	virtual FiniteTimeAction* changeFightState(FightState state);
	//更新状态
	virtual void updateSelf();
	//获取战斗唯一id
	virtual int getFighterID() const;
	//名称生成
	virtual string getFighterName() const;
	//出手顺序图
	virtual string getThumbnail() const;
	//脚本执行时调用
	virtual void execute(Fighter* fighter);
	//使用道具
	virtual void useItem(Good* good);
	//胜利
	virtual void victory();
	virtual void clean();
	//获取总属性
	virtual Properties* getTotalProperties() const;
	//血量
	virtual int getMaxHitPoint() const;
	virtual void setMaxHitPoint(int var);
	//魔法值
	virtual int getMaxManaPoint() const;
	virtual void setMaxManaPoint(int var);
	//获取属性
	virtual int getProperty(PropertyType type) const;
	virtual void setProperty(PropertyType type, int value);
	//获取命中率
	virtual float getHitRate() const;
	//获取躲避率
	virtual float getEvasionRate() const;
	//获取暴击几率
	virtual float getCriticalRate() const;
protected:
	virtual void onHurt(int afterDamage);
private:
	int getUniqueID(bool bAdding) const;
	void setUniqueID(int uniqueID);

	Value getValueForKey(const string& key, Value::Type type) const;
};
#endif