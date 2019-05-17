#ifndef __GameScene_H__
#define __GameScene_H__
#include <vector>
#include <string>

#include "SDL_Engine/SDL_Engine.h"

using namespace std;
USING_NS_SDL;

class MapLayer;
class EffectLayer;
enum class GameState;

class GameScene : public Scene
{
	SDL_SYNTHESIZE_READONLY(MapLayer*, m_pMapLayer, MapLayer);//地图层
	SDL_SYNTHESIZE_READONLY(EffectLayer*, m_pEffectLayer, EffectLayer);//地图层
	SDL_SYNTHESIZE_READONLY(GameState, m_gameState, GameState);//游戏状态
private:
	static GameScene* s_pInstance;
public:
	static GameScene* getInstance();
	static void purge();
private:
	GameScene();
	~GameScene();
	bool init();
	bool initializeMap();
public:
	virtual bool onTouchBegan(Touch* touch,SDL_Event* event);
public:
	//改变场景
	void changeMap(const string& mapFilename, const Point& tileCoodinate);
};
#endif