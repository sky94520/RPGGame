#ifndef __GameScene_H__
#define __GameScene_H__
#include <vector>
#include <string>

#include "lua.hpp"
#include "SDL_Engine/SDL_Engine.h"

using namespace std;
USING_NS_SDL;

class EventLayer;
class MapLayer;
class PlayerLayer;
class ScriptLayer;
class EffectLayer;
class GoodLayer;
class DisplayLayer;
class MessageLayer;
class OperationLayer;
class BattleScene;
enum class Direction;
class Character;
enum class WaitType;
class Good;

//游戏状态
enum class GameState
{
	Normal,/*正常状态，即行走状态*/
	Fighting,/*战斗状态*/
	Script,/*脚本状态*/
};

class GameScene : public Scene
{
private:
	static GameScene* s_pInstance;
public:
	static GameScene* getInstance();
	static void purge();
	SDL_SYNTHESIZE_READONLY(EventLayer*, m_pEventLayer, EventLayer);
	SDL_SYNTHESIZE_READONLY(MapLayer*, m_pMapLayer, MapLayer);
	SDL_SYNTHESIZE_READONLY(PlayerLayer*, m_pPlayerLayer, PlayerLayer);
	SDL_SYNTHESIZE_READONLY(ScriptLayer*, m_pScriptLayer, ScriptLayer);
	SDL_SYNTHESIZE_READONLY(OperationLayer*, m_pOperationLayer, OperationLayer);
	SDL_SYNTHESIZE_READONLY(BattleScene*, m_pBattleScene, BattleScene);
	SDL_SYNTHESIZE_READONLY(GoodLayer*, m_pGoodLayer, GoodLayer);
	SDL_SYNTHESIZE_READONLY(DisplayLayer*, m_pDisplayLayer, DisplayLayer);
	SDL_SYNTHESIZE_READONLY(MessageLayer*, m_pMessageLayer, MessageLayer);
	SDL_SYNTHESIZE_READONLY(EffectLayer*, m_pEffectLayer, EffectLayer);
private:
	Character* m_pViewpointCharacter;
	lua_State* m_pLuaState;
	GameState m_gameState;
	//保留协程运行结果
	int m_nCoroutineRet;
public:
	static const int CHARACTER_LOCAL_Z_ORDER = 9999;//需要比tmx地图总图块大
	static const string CHARACTER_MOVE_TO_TILE;
private:
	GameScene();
	~GameScene();
	bool init();
	void preloadResources();
	bool initializeScript();
	bool initializeMapAndPlayers();
	//角色移动到图块
	void moveToTile(EventCustom* eventCustom);
	//获取图块的索引
	int getAreaIndexAt(const Point& tilePos);
	//死路避免
	bool avoidDeadPath(Point& toTile, const Point& fromTile, int fromTileIndex);
public:
	void update(float dt);
	//重写MapLayer方法
	bool isPassing(const Point& tilePos);
	bool isPassing4(const Point& tilePos, Direction dir);
	//改变场景
	void changeMap(const string& mapName, const Point& tileCoodinate);
	//设置视图中心点
	void setViewpointCenter(const Point& position, float duration = 0.f);
	//设置视角跟随
	void setViewpointFollow(Character* character);
	//游戏状态
	GameState getGameState() const;
	void setGameState(GameState state);
	//在背包中添加物品
	Good* addGood(const string& goodName, int number);
	//在背包中移除物品
	bool subGood(const string& goodName, int number);
	bool subEquipment(const string& playerName, const string& goodName, int number);
	//金钱
	void addGold(int num);
	bool subGold(int num);
	//经验
	void addExp(int exp);
public:
	//点击了地面 操作主角
	void clickPath(const Point& location);
	//点击事件
	void clickEvent(const Point& location);
	//根据唯一的id获取角色类
	Character* getCharacterByID(int uniqueID);
	//存档
	void saveData();
	//打开背包
	void openBag();
	//关闭背包
	void closeBag();
	//使用物品
	void useGood(Good* good);
	//购买物品
	bool buyGood(Good* good);
	//出售
	bool sellGood(Good* good);
	//战斗结束
	void endBattle();
	//打开请求层
	void openQuest();
	void closeQuest();
	//是否可以点击地面
	bool isPathClickable() const;
public://脚本相关
	//为lua添加加载器
	void addLuaLoader(lua_CFunction func);
	//添加lua文件搜索路径
	void addLuaSearchPath(const char* path);
	//执行脚本文件 force是否强制执行该文件
	int executeScriptFile(const string& filename, bool force = false);
	//是否已经加载了该文件
	bool isLoadedScriptFile(const char* filename) const;
	//执行字符串
	int executeString(const char* codes);
	//执行函数 参数和函数需要先入栈
	int executeFunction(int nargs, int nresults);
	//开始或者恢复协程 仅开始时需要函数入栈，其他恢复协程则只需要参数即可
	int resumeFunction(int nargs);
	//暂停
	int yield(int nresult);
	
	int resumeLuaScript(WaitType waitType, int nargs);
	//把对应类型的值放入栈顶，并返回其类型
	int getLuaGlobal(const char* name);
	//把t[k]放入栈顶，并返回其类型
	int getLuaField(int index, const char* key);

	lua_State* getLuaState() const { return m_pLuaState;}
	//参数入栈
	void pushInt(int intValue);
	void pushFloat(float floatValue);
	void pushBoolean(bool boolValue);
	void pushString(const char* stringValue);
	void pushNil();

	void pop(int n);

	bool toLuaBoolean(int index);
	int checkLuaInt(int index);
	float checkLuaFloat(int index);
	const char* checkLuaString(int index);
	//获取对应索引的长度 lua_getlen()
	int getLuaLen(int index);
	//lua_geti()
	int getLuaIndex(int index, int n);
	//移除栈中的元素
	void removeLuaIndex(int index);
};
#endif