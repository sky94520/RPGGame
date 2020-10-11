#ifndef __CharacterData_H__
#define __CharacterData_H__

#include <map>
#include <string>
#include <vector>
#include <SDL_Engine/SDL_Engine.h>

#include "../GameMacros.h"
#include "../dist/json/json.h"

USING_NS_SDL;
using namespace std;

enum class Direction;
enum class FightState;

class CharacterData : public Object
{
public:
	CharacterData();
	CREATE_FUNC(CharacterData);
	bool init();
	//加载角色数据以及加载所需要的图片并解析
	bool loadCharacterFile(const string& filename);
	//获取人物行走动画
	Animation* getWalkingAnimation(const string& chartletName, Direction direction);
	Animation* getWalkingAnimation(const string& filename, int index, Direction dir, float delay, int loops, bool restoreOriginalFrame);
	//获取立绘
	SpriteFrame* getFaceSpriteFrame(const string& filename, int index);
	SpriteFrame* getFaceSpriteFrame(const string& chartletName);
	//获取sv 战斗图
	Animation* getSVAnimation(const string& chartletName,FightState fightState);
	//获取turn name
	string getTurnFilename(const string& chartletName) const;
	//获取升级数据
	LevelUp& getDataByLevel(const string& chartletName,int level);
private:
	//添加角色战斗图并生成16状态动画
	bool addSVAnimation(const string& chartletName, const string& filename);
	//添加角色升级文件
	bool addLevelUpData(const string& chartletName, const string& filename);
	/*在纹理指定区域rect按照宽度切割,并返回*/
	void splitTexture(Texture* texture, const Rect& rect ,float width, vector<SpriteFrame*>& frames);
private:
	//角色键值对
	Json::Value m_jsonData;
	map<string,vector<LevelUp>> m_levelUpData;
};
#endif