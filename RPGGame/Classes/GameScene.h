#ifndef __GameScene_H__
#define __GameScene_H__
#include <vector>
#include <string>
#include <unordered_map>
#include <SDL_Engine/SDL_Engine.h>

#include "entity/AStar.h"
#include "layer/SpritePool.h"
#include "data/StaticData.h"
#include "data/DynamicData.h"
#include "ui/GoodLayer.h"
#include "ui/OperationLayer.h"

using namespace std;
USING_NS_SDL;

class Good;
class MapLayer;
class EffectLayer;
class PlayerManager;
class ScriptManager;
class OperationLayer;
class BattleScene;
class BagLayer;
class Character;
class LuaStack;
class MessageLayer;
enum class GameState;

class GameScene : public Scene, public AStartDelegate, OperationDelegate, GoodLayerDelegate
{
	SDL_SYNTHESIZE_READONLY(MapLayer*, m_pMapLayer, MapLayer);//地图层
	SDL_SYNTHESIZE_READONLY(EffectLayer*, m_pEffectLayer, EffectLayer);//特效层
	SDL_SYNTHESIZE_READONLY(OperationLayer*, m_pOperationLayer, OperationLayer);//操作层
	SDL_SYNTHESIZE_READONLY(BattleScene*, m_pBattleScene, BattleScene); //战斗层
	SDL_SYNTHESIZE_READONLY(BagLayer*, m_pBagLayer, BagLayer);//物品层
	SDL_SYNTHESIZE_READONLY(MessageLayer*, m_pMsgLayer, MessageLayer);//文本层

	SDL_SYNTHESIZE_READONLY(PlayerManager*, m_pPlayerManager, PlayerManager);//玩家层
	SDL_SYNTHESIZE_READONLY(ScriptManager*, m_pScriptManager, ScriptManager);//脚本层
public:
	static GameScene* getInstance()
	{
		//仅在场景在运行时才会分配新的对象
		if (s_pInstance == nullptr && Director::getInstance()->isRunning())
		{
			s_pInstance = new GameScene();
			s_pInstance->init();
		}
		return s_pInstance;
	}
	static void purge()
	{
		SDL_SAFE_RELEASE_NULL(s_pInstance);
		StaticData::purge();
		DynamicData::purge();
		SpritePool::purge();
	}
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
	vector<Character*> getCharacterList();
	LuaStack* getLuaStack() const;
public: //OperationDelegate
	virtual void openBag();
	virtual void saveProgress();
public: //GoodLayerDelegate
	virtual void pageBtnCallback(GoodLayer* goodLayer, int value);
	virtual void updateGoodHook(LabelAtlas* pCostLabel, LabelAtlas* pNumberLabel, int cost, int number);
	virtual void useBtnCallback(GoodLayer* goodLayer);
	virtual void equipBtnCallback(GoodLayer* goodLayer);
	virtual void closeBtnCallback(GoodLayer* goodLayer);
	virtual void selectGoodCallback(GoodLayer* goodLayer, GoodInterface* good);
	virtual bool touchOutsideCallback(GoodLayer* goodLayer);
public:
	//改变场景
	void changeMap(const string& mapFilename, const Point& tileCoodinate);
	void closeBag();
	//战斗
	void startBattle(const unordered_map<string, int>& enemy);
	void endBattle();

	Good* addGood(const string& goodName, int number);
	bool removeGood(const string& goodName, int number);
	//使用物品
	void useGood(Good* good);
	bool buyGood(Good* good);
	bool sellGood(Good* good);
private:
	static GameScene* s_pInstance;
	GameState m_gameState; //游戏状态
};
#endif