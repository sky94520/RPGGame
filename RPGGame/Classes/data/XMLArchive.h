#ifndef __XMLArchive_H__
#define __XMLArchive_H__

#include <map>
#include <string>
#include <SDL_Engine/SDL_Engine.h>
#include "Archive.h"

using namespace std;
using namespace SDL;

class UserRecord;
class Character;
struct PlayerData;

class XMLArchive: public Archive
{
public:
	virtual bool read(const string& filename, bool bFirstGame);
	virtual bool write(const string& filename, const string&map, const Point& tilePos);
private:
	//玩家状态
	void parsePlayer(rapidxml::xml_node<>* root, bool bFirstGame);
	void writePlayer(rapidxml::xml_document<>& doc, rapidxml::xml_node<>*root);

	void parseSkill(rapidxml::xml_node<>* root, PlayerData* playerData);
	void writeSkill(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* root, PlayerData* playerData);

	void parseBag(rapidxml::xml_node<>* root);
	void writeBag(rapidxml::xml_document<>& doc, rapidxml::xml_node<>*root);

	void parseEquipment(rapidxml::xml_node<>* root, PlayerData* data);
	void writeEquipment(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* root, PlayerData* playerData);
};
#endif