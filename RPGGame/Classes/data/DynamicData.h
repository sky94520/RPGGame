#ifndef __DynamicData_H__
#define __DynamicData_H__

#include <map>
#include <string>
#include <vector>
#include <algorithm>

#include "SDL_Engine/SDL_Engine.h"

USING_NS_SDL;
using namespace std;

class UserRecord;

//--------------------------------------------DynamicData---------------------------------------
class DynamicData : public Object
{
public:
	static DynamicData* getInstance();
	static void purge();
public:
	//读取存档
	bool initializeSaveData(int idx);
	//保存数据
	bool save(const string& map,const Point& tilePos, int nDir);

	//获取存档的所在地图名
	const string& getMapFilename() const;
	//获取角色的位置
	const Point& getTileCoordinate() const;
private:
	DynamicData();
	~DynamicData();
	bool init();
private:
	static DynamicData* s_pInstance;
private:
	//存档名称
	string m_filename;
	//存档索引
	int m_nSaveDataIndex;
	//是否第一次进入游戏
	bool m_bFirstGame;
	UserRecord* m_pUserRecord;
};
#endif