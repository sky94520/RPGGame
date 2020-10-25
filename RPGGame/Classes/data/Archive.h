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
	virtual bool write(const string& filename) = 0;
	void setUserRecord(UserRecord* p) { m_pUserRecord = p; }
protected:
	UserRecord* m_pUserRecord;
};

class XMLArchive: public Archive
{
public:
	virtual bool read(const string& filename, bool bFirstGame);
	virtual bool write(const string& filename);
private:
	void parsePlayer(rapidxml::xml_node<>* root, bool bFirstGame);
	void parseSkill(rapidxml::xml_node<>* root, PlayerData* playerData);
	void parseBag(rapidxml::xml_node<>* root);
	void parseEquipment(rapidxml::xml_node<>* root, Character* player);
};

#endif