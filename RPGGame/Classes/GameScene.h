#ifndef __GameScene_H__
#define __GameScene_H__
#include <vector>
#include <string>

#include "SDL_Engine/SDL_Engine.h"

using namespace std;
USING_NS_SDL;

class MapLayer;

class GameScene : public Scene
{
	SDL_SYNTHESIZE_READONLY(MapLayer*, m_pMapLayer, MapLayer);//地图层
private:
	static GameScene* s_pInstance;
public:
	static GameScene* getInstance();
	static void purge();
public:
	static const int CHARACTER_LOCAL_Z_ORDER = 9999;//需要比tmx地图总图块大
	static const string CHARACTER_MOVE_TO_TILE;
private:
	GameScene();
	~GameScene();
	bool init();
	bool initializeMapAndPlayers();
public:
	//改变场景
	void changeMap(const string& mapName, const Point& tileCoodinate);
	//设置视图中心点
	void setViewpointCenter(const Point& position, float duration = 0.f);
};
#endif