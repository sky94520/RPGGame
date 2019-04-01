#ifndef __MapLayer_H__
#define __MapLayer_H__
#include <string>
#include "SDL_Engine/SDL_Engine.h"

using namespace std;
USING_NS_SDL;
enum class Direction;

class MapLayer : public Layer
{
private:
	TMXTiledMap* m_pTiledMap;
	//地图文件路径
	string m_filepath;
public:
	MapLayer();
	~MapLayer();
	CREATE_FUNC(MapLayer);
	static MapLayer* create(const string& filepath);

	bool init();
	bool init(const string& filepath);
	//清除
	void clear();
	//根据某图块的属性判断某方向是否可通过
	bool isPassing(int gid);
	bool isPassing(int gid,Direction direction);
	//把本地坐标转化为图块坐标
	Point convertToTileCoordinate(const Point& pos);
	//获取该地图的文件路径
	string& getFilepath();
	//获取地图名字
	string getMapName() const;
	//获取背景音乐
	string getBGMFilename() const;
	//获取meta层
	TMXLayer* getMetaLayer() const;
	//获取碰撞层
	TMXLayer* getCollisionLayer() const;
	//获取脚本对象
	TMXObjectGroup* getScriptObjectGroup() const;
	TMXTiledMap* getTiledMap() const;
private:
	void resetLocalZOrderOfTile();
};
#endif