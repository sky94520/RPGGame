#ifndef __LuaObject_H__
#define __LuaObject_H__

#include <string>

#include "lua.hpp"
#include "Character.h"

using namespace std;

enum class TriggerType;
class LuaStack;

/*
 * LuaObject 脚本类
 * trigger type 触发方式
 * TriggerType::Touch
 * TriggerType::Click 点击触发
 * priority 优先级 影响的是碰撞
*/
class LuaObject : public Character
{
public:
	LuaObject();
	~LuaObject();

	static LuaObject* create(const string& chartletName);
	bool init(const string& chartletName);

	//设置lua栈
	void setLuaStack(LuaStack* pLuaStack);
	//执行脚本函数
	bool execScriptFunc(const string& playerName);
	//是否产生了碰撞
	bool intersectRect(const Rect& rect);

	//脚本对象变量名
	const string& getLuaName() const { return m_luaName; }
	void setLuaName(const string& name) { m_luaName = name; }

	//触发方式
	TriggerType getTriggerType() const { return m_triggerType; }
	void setTriggerType(TriggerType trigger) { m_triggerType = trigger; }

	//触发函数
	const string& getFuncName()const { return m_funcName; }
	void setFuncName(const string& funcName) { m_funcName = funcName; }

	//优先级
	int getPriority() const { return m_nPriority; }
	void setPriority(int priority) { m_nPriority = priority; }

	//该对象是否废弃
	bool isObsolete() const { return m_bObsolete; }
	void setObsolete(bool obsolete) { m_bObsolete = obsolete; }
private:
	//脚本对象的变量名
	string m_luaName;
	LuaStack* m_pLuaStack;
	//包围盒
	Rect m_boundingBox;
	bool m_bUsingBox;
	//触发方式
	TriggerType m_triggerType;
	//触发函数
	string m_funcName;
	//优先级
	int m_nPriority;
	//是否废弃了
	bool m_bObsolete;
};
#endif