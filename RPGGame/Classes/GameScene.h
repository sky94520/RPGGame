#ifndef __GameScene_H__
#define __GameScene_H__
#include <vector>
#include <string>
#include <SDL_Engine/SDL_Engine.h>

#include "entity/AStar.h"
#include "ui/GoodLayer.h"
#include "ui/OperationLayer.h"

using namespace std;
USING_NS_SDL;

class MapLayer;
class EffectLayer;
class PlayerManager;
class ScriptManager;
class OperationLayer;
class BagLayer;
enum class GameState;

class GameScene : public Scene, public AStartDelegate, OperationDelegate, GoodLayerDelegate
{
	SDL_SYNTHESIZE_READONLY(MapLayer*, m_pMapLayer, MapLayer);//地图层
	SDL_SYNTHESIZE_READONLY(EffectLayer*, m_pEffectLayer, EffectLayer);//特效层
	SDL_SYNTHESIZE_READONLY(OperationLayer*, m_pOperationLayer, OperationLayer);//操作层
	SDL_SYNTHESIZE_READONLY(BagLayer*, m_pBagLayer, BagLayer);//物品层

	SDL_SYNTHESIZE_READONLY(PlayerManager*, m_pPlayerManager, PlayerManager);//玩家层
	SDL_SYNTHESIZE_READONLY(ScriptManager*, m_pScriptManager, ScriptManager);//脚本层
public:
	static GameScene* getInstance();
	static void purge();
private:
	GameScene();
	~GameScene();
	bool init();
	bool initializeMap();
	//加载资源
	void preloadResources();
	bool isPassing(const SDL_Point& tilePos) const;
	bool isPassing4(const SDL_Point& tilePos, Direction direction) const;
	void moveToTile(EventCustom* eventCustom);
public:
	virtual bool onTouchBegan(Touch* touch,SDL_Event* event);
	void update(float dt);
	Node* getCollisionLayer() const;
	GameState getGameState() const { return m_gameState; }
	void setGameState(GameState state);
public: //OperationDelegate
	virtual void openBag();
	virtual void saveProgress();
public: //GoodLayerDelegate
	virtual void pageBtnCallback(GoodLayer* goodLayer, int value);
	virtual void useBtnCallback(GoodLayer* goodLayer);
	virtual void equipBtnCallback(GoodLayer* goodLayer);
	virtual void closeBtnCallback(GoodLayer* goodLayer);
	virtual void selectGoodCallback(GoodLayer* goodLayer, GoodInterface* good);
public:
	//改变场景
	void changeMap(const string& mapFilename, const Point& tileCoodinate);
private:
	static GameScene* s_pInstance;
	GameState m_gameState; //游戏状态
};
#endif