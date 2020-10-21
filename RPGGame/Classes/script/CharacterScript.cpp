#include "CharacterScript.h"
#include "../GameScene.h"
#include "../data/StaticData.h"
#include "../data/CharacterData.h"
#include "../entity/Entity.h"
#include "../entity/Character.h"
#include "../entity/Controller.h"
#include "../layer/MapLayer.h"
#include "../layer/EffectLayer.h"
//------------------------------character------------------------------------
int open_character(lua_State* pL)
{
	const luaL_Reg lib[] = {
		{"showAnimation", showAnimation},
		{"getDirection", getDirection},
		{"setDirection", setDirection},
		{"showBalloonIcon", showBalloonIcon},
		{"moveToward", moveToward},
		{"setVisible", setVisible},
		{"setDurationPerGrid", setDurationPerGrid},
		{"getTilePosition", getTilePosition},
		{"getPosition", getPosition},
		{NULL, NULL}
	};
	luaL_newlib(pL, lib);
	return 1;
}

int showAnimation(lua_State* pL)
{
	int id = (int)luaL_checkinteger(pL, 1);

	auto character = GameScene::getInstance()->getCharacterByID(id);

	if (character == nullptr)
		return 0;

	Animation* animation = nullptr;
	//animationName
	if (lua_gettop(pL) == 2)
	{
		auto animationName = luaL_checkstring(pL, 2);
		animation = AnimationCache::getInstance()->getAnimation(animationName);
	}//chartletName dir
	else if (lua_gettop(pL) == 3)
	{
		auto chartletName = luaL_checkstring(pL, 2);
		int nDir = (int)luaL_checkinteger(pL, 3);
		Direction dir = static_cast<Direction>(nDir);

		animation = StaticData::getInstance()->getCharacterData()->getWalkingAnimation(chartletName, dir);
	}
	//运行动画
	lua_Number duration = (lua_Number)animation->getDuration();
	auto animate = Animate::create(animation);
	animate->setTag(Entity::ANIMATION_TAG);

	character->getSprite()->stopActionByTag(Entity::ANIMATION_TAG);
	character->getSprite()->runAction(animate);	

	lua_pushnumber(pL,duration);

	return 1;
}

int getDirection(lua_State* pL)
{
	int id = (int)luaL_checkinteger(pL, 1);

	auto character = GameScene::getInstance()->getCharacterByID(id);

	if (character == nullptr)
		return 0;

	auto dir = character->getDirection();
	lua_Integer nDir = static_cast<lua_Integer>(dir);

	lua_pushinteger(pL, nDir);
	return 1;
}

int setDirection(lua_State* pL)
{
	int id = (int)luaL_checkinteger(pL, 1);

	auto character = GameScene::getInstance()->getCharacterByID(id);

	if (character == nullptr)
		return 0;
	Direction oldDir = character->getDirection();
	lua_Integer nOldDir = static_cast<lua_Integer>(oldDir);
	//获取方向并设置
	int nDir = (int)luaL_checkinteger(pL, 2);
	Direction dir = static_cast<Direction>(nDir);

	character->setDirection(dir);
	//返回原来的方向
	lua_pushinteger(pL, nOldDir);
	return 1;
}

int showBalloonIcon(lua_State* pL)
{
	int id = (int)luaL_checkinteger(pL, 1);
	auto balloonName = luaL_checkstring(pL, 2);
	//获取character
	auto gameScene = GameScene::getInstance();
	
	auto character = gameScene->getCharacterByID(id);
	if (character == nullptr)
	{
		printf("error:LUA_showBalloonIconOfCharacter character == nullptr\n");
		return 0;
	}
	auto collisionLayer = gameScene->getMapLayer()->getCollisionLayer();
	auto effectLayer = gameScene->getEffectLayer();
	//设置位置
	auto pos = character->getPosition();
	auto size = character->getContentSize();

	pos.y -= size.height / 2;
	//显示气泡
	float duration = effectLayer->showBalloonIcon(balloonName, pos, collisionLayer);
	//放入参数
	lua_pushnumber(pL, (lua_Number)duration);
	return 1;
}

int moveToward(lua_State* pL)
{
	//获取id
	int id = (int)luaL_checkinteger(pL, 1);
	//获取目的x y
	int x = (int)luaL_checkinteger(pL, 2);
	int y = (int)luaL_checkinteger(pL, 3);
	SDL_Point toTile = { x, y };

	auto gameScene = GameScene::getInstance();
	auto mapLayer = gameScene->getMapLayer();

	Controller* controller = gameScene->getControllerByCharacterID(id);
	bool bRet = false;
	float duration = 0.f;
	//角色存在并且不为空。进行移动
	if (controller != nullptr && gameScene->isPassing(toTile))
	{
		//移动
		duration = controller->moveToward(toTile);
		bRet = true;
	}
	lua_pushboolean(pL, bRet);
	//可以到达 放入持续时间
	if (bRet)
	{
		lua_pushnumber(pL, duration);

		return 2;
	}
	else
	{
		return 1;
	}
}

int setVisible(lua_State* pL)
{
	int id = (int)luaL_checkinteger(pL, 1);
	auto visiblity = (lua_toboolean(pL, 2) == 1);
	//获取character
	auto gameScene = GameScene::getInstance();
	
	auto character = gameScene->getCharacterByID(id);
	if (character == nullptr)
	{
		printf("error:LUA_showBalloonIconOfCharacter character == nullptr\n");
		return 0;
	}
	character->setVisible(visiblity);
	return 0;
}

int setDurationPerGrid(lua_State* pL)
{
	int id = (int)luaL_checkinteger(pL, 1);
	float duration = (float)luaL_checknumber(pL, 2);
	//获取character
	auto gameScene = GameScene::getInstance();
	Controller* controller = gameScene->getControllerByCharacterID(id);
	
	if (controller == nullptr)
	{
		printf("error:LUA_showBalloonIconOfCharacter character == nullptr\n");
		return 0;
	}
	controller->setDurationPerGrid(duration);

	return 0;
}

int getTilePosition(lua_State* pL)
{
	int id = (int)luaL_checkinteger(pL, 1);
	//获取character
	auto gameScene = GameScene::getInstance();
	
	auto character = gameScene->getCharacterByID(id);
	if (character == nullptr)
	{
		printf("error:LUA_showBalloonIconOfCharacter character == nullptr\n");
		return 0;
	}
	auto mapLayer = gameScene->getMapLayer();
	auto tileSize = mapLayer->getTiledMap()->getTileSize();
	auto pos = character->getPosition();

	int tileX = int (pos.x / tileSize.width);
	int tileY = int (pos.y / tileSize.height);

	lua_pushinteger(pL, tileX);
	lua_pushinteger(pL, tileY);

	return 2;
}

int getPosition(lua_State* pL)
{
	int id = (int)luaL_checkinteger(pL, 1);
	//获取character
	auto gameScene = GameScene::getInstance();
	
	auto character = gameScene->getCharacterByID(id);
	if (character == nullptr)
	{
		printf("error:LUA_showBalloonIconOfCharacter character == nullptr\n");
		return 0;
	}
	auto pos = character->getPosition();

	lua_pushnumber(pL, pos.x);
	lua_pushnumber(pL, pos.y);

	return 2;
}