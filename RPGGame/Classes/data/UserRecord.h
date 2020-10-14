#ifndef __UserRecord_H__
#define __UserRecord_H__

#include <vector>
#include <string>
#include <cstring>
#include <unordered_map>

#include "SDL_Engine/SDL_Engine.h"

USING_NS_SDL;
using namespace std;

class Good;
class Character;
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
	//装备
	void equip(const string&playerName, int uniqueId, Good* good);
private:
	void parsePlayer(rapidxml::xml_node<>* root);
	void parseSkill(rapidxml::xml_node<>* root, PlayerData* playerData);
	void parseBag(rapidxml::xml_node<>* root);
	void parseEquipment(rapidxml::xml_node<>* root, Character* player);
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
	//玩家背包物品
	vector<Good*> m_bagGoodList;
};
#endif