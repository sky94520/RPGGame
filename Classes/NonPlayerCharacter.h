#ifndef __NonPlayerCharacter_H__
#define __NonPlayerCharacter_H__
#include <string>
#include "lua.hpp"
#include "Character.h"

using namespace std;
//脚本触发方式
enum class TriggerType
{
	None,
	Click,/*点击NPC*/
	Touch,/*触摸触发*/
	All,
};

class NonPlayerCharacter : public Character
{
	SDL_SYNTHESIZE(TriggerType, m_triggerType, TriggerType);
	SDL_BOOL_SYNTHESIZE(m_bUpdate, Update);//是否调用update函数
	SDL_BOOL_SYNTHESIZE(m_bObsolete, Obsolete);//是否废弃
	SDL_SYNTHESIZE_READONLY(int, m_nPriority, Priority);//优先级
private:
	string m_name;//脚本名称
	string m_filename;//脚本文件名
	Rect m_boundingBox;//包围盒
public:
	NonPlayerCharacter();
	~NonPlayerCharacter();

	static NonPlayerCharacter* create(const ValueMap& objects);

	bool init(const ValueMap& objects);
	//清除函数，便于重用
	void clear();

	bool initialize(const string& mapName);
	//主角进行交互并执行对应函数 当返回值为true则吞并事件
	bool execute(int playerID);
	void update(float dt);
	//调用对应的脚本的clean函数
	void clean();
	//是否和对应矩形发生碰撞
	bool intersectRect(const Rect& rect);
	//是否能触发
	bool isTrigger(TriggerType triggerType);
	//是否能通过
	bool isPassing() const;
private:
	static TriggerType getTriggerTypeByString(const string& sType);
};
#endif