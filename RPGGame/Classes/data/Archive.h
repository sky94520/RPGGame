#ifndef __Archive_H__
#define __Archive_H__

#include <string>
#include <SDL_Engine/SDL_Engine.h>

using namespace std;
using namespace SDL;

class UserRecord;
class Character;
struct PlayerData;

class Archive
{
public:
	//从文件中读取数据
	virtual bool read(const string& filename, bool bFirstGame) = 0;
	//保存数据到文件中
	virtual bool write(const string& filename, const string&map,const Point& tilePos) = 0;
	void setUserRecord(UserRecord* p) { m_pUserRecord = p; }
protected:
	UserRecord* m_pUserRecord;
};
#endif