#ifndef __ScriptManager_H__
#define __ScriptManager_H__

#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>

#include "SDL_Engine/SDL_Engine.h"

USING_NS_SDL;
using namespace std;

class LuaObject;
class LuaStack;
class AStarController;
enum class WaitType;
enum class TriggerType;
struct lua_State;
enum class GameState;

class ScriptManager : public Layer
{
public:
	ScriptManager();
	~ScriptManager();

	CREATE_FUNC(ScriptManager);
	bool init();

	void update(float dt, GameState gameState);

	LuaObject* addLuaObject(const string& name, const string& chartletName, Node* layer, GameState gameState);
	bool removeLuaObject(const string& name);

	//A星算法移动到图块回调函数
	void triggerTouchScript(AStarController* controller, GameState gameState);

	//获取点击了npc
	LuaObject* getClickedNPC(const Rect& r, int priority) const;
	LuaObject* getNPCByID(int id) const;
	LuaObject* getLuaObject(const string& name);

	//恢复协程
	int resumeCoroutine(WaitType waitType, int nargs);

	//清除全部脚本事件
	void clear();

	void setWaitType(WaitType waitType) { m_waitType = waitType; }
	void setDuration(float duration) { m_duration = duration; }
	LuaStack* getLuaStack() const { return m_pLuaStack; }
private:
	//注册函数给lua
	void registerFuncs(lua_State* pL);
private:
	//脚本等待类型
	WaitType m_waitType;
	//等待时间
	float m_duration;

	unordered_map<string, LuaObject*> m_objects;
	vector<LuaObject*> m_toAddedObjects;
	LuaStack* m_pLuaStack;
};
#endif