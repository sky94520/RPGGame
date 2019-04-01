#ifndef __ScriptLayer_H__
#define __ScriptLayer_H__
#include <vector>
#include <algorithm>
#include "SDL_Engine/SDL_Engine.h"
class NonPlayerCharacter;
enum class TriggerType;

using namespace std;
USING_NS_SDL;
/*脚本等待类型*/
enum class WaitType
{
	None,
	Time,
	Click,
	Button,
};

class ScriptLayer : public Layer
{
	SDL_SYNTHESIZE(WaitType, m_waitType, WaitType);//脚本等待类型
	SDL_SYNTHESIZE(float, m_waitTime, WaitTime);//等待时间
private:
	vector<NonPlayerCharacter*> m_npcList;
	vector<NonPlayerCharacter*> m_npcPool;
public:
	ScriptLayer();
	~ScriptLayer();

	CREATE_FUNC(ScriptLayer);
	bool init();

	virtual void update(float dt);
	void updateWaitTime(float dt);
	/*触发事件，如果触发事件则返回true*/
	bool checkAndTriggerScriptEvent(const Rect& boundingBox, int playerID, TriggerType type, int priority);
	//获取点击了npc
	NonPlayerCharacter* getClickedNPC(const Rect& r, int priority) const;
	//根据id获取NPC
	NonPlayerCharacter* getNPCByID(int id) const;
	//添加NPC
	NonPlayerCharacter* addNPC(const ValueMap& valueMap);
	//清除全部脚本事件
	void clear();
};
#endif