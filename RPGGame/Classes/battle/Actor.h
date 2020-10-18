#ifndef __Actor_H__
#define __Actor_H__
#include "Fighter.h"

class Good;

/*主角团 玩家所操作的战斗角色类*/
class Actor : public Fighter
{
public:
	Actor();
	~Actor();

	static Actor* create(const string& chartletName, int uniqueID);
	bool init(const string& chartletName, int uniqueID);

	virtual void updateSelf();
	//准备出手
	void ready();
	//逃跑
	virtual void escape();
	//防御
	virtual void guard();
	//获取战斗唯一id 如果是主角，则与character对应
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
};
#endif