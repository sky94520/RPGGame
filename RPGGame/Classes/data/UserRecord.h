#ifndef __UserRecord_H__
#define __UserRecord_H__

#include <string>
#include <cstring>
#include <unordered_map>

#include "SDL_Engine/SDL_Engine.h"

USING_NS_SDL;
using namespace std;

struct PlayerData;

class UserRecord : public Object
{
public:
	UserRecord();
	~UserRecord();
	CREATE_FUNC(UserRecord);

	//从文件中读取数据
	bool readFromXML(const string& filename);
	//保存数据到文件中
	bool writeToXML(const string& filename);
private:
	void parsePlayer(rapidxml::xml_node<>* root);
	void parseBag(rapidxml::xml_node<>* root);
public:
	//金币数量
	int goldNumber;
	//出售比例
	float sellRatio;
	//地图
	string mapFilename;
	//玩家位置
	Point tileCoordinate;
	//玩家数据
	unordered_map<string, PlayerData*> players;
};
#endif