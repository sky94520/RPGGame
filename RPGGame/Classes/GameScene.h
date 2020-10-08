#ifndef __GameScene_H__
#define __GameScene_H__
#include <vector>
#include <string>

#include "SDL_Engine/SDL_Engine.h"
#include "entity/AStar.h"

using namespace std;
USING_NS_SDL;

class MapLayer;
class EffectLayer;
class PlayerManager;
class ScriptManager;
enum class GameState;

class GameScene : public Scene, public AStartDelegate
{
	SDL_SYNTHESIZE_READONLY(MapLayer*, m_pMapLayer, MapLayer);//地图层
	SDL_SYNTHESIZE_READONLY(EffectLayer*, m_pEffectLayer, EffectLayer);//地图层
	SDL_SYNTHESIZE_READONLY(PlayerManager*, m_pPlayerManager, PlayerManager);//玩家层
	SDL_SYNTHESIZE_READONLY(ScriptManager*, m_pScriptManager, ScriptManager);//脚本层
	SDL_SYNTHESIZE(GameState, m_gameState, GameState);//游戏状态
public:
	static GameScene* getInstance();
	static void purge();
private:
	GameScene();
	~GameScene();
	bool init();
	bool initializeMap();
	bool isPassing(const SDL_Point& tilePos) const;
	bool isPassing4(const SDL_Point& tilePos, Direction direction) const;
	void moveToTile(EventCustom* eventCustom);
public:
	virtual bool onTouchBegan(Touch* touch,SDL_Event* event);
	void update(float dt);
	Node* getCollisionLayer() const;
public:
	//改变场景
	void changeMap(const string& mapFilename, const Point& tileCoodinate);
private:
	static GameScene* s_pInstance;
};
#endif