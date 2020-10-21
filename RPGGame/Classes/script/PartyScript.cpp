#include "PartyScript.h"
#include "../GameScene.h"
#include "../entity/Good.h"
#include "../entity/Character.h"
#include "../data/DynamicData.h"
#include "../manager/PlayerManager.h"
#include "../battle/BattleScene.h"
#include "../ui/BagLayer.h"
//------------------------------party----------------------------------
int open_party(lua_State* pL)
{
	const luaL_Reg lib[] = {
		{"addGold", addGold},
		{"subGold", subGold},
		{"addGood", addGood},
		{"subGood", subGood},
		{"subEquipment", subEquipment},
		{"getEquipment", getEquipment},
		{"getGoodNumber", getGoodNumber},
		{"getName", getName},
		{"recover", recover},
		{NULL, NULL}
	};
	luaL_newlib(pL, lib);
	return 1;
}

int addGold(lua_State* pL)
{
	int gold = (int)luaL_checkinteger(pL, 1);
	GameScene::getInstance()->getBagLayer()->addGold(gold);

	return 0;
}

int subGold(lua_State* pL)
{
	int gold = (int)luaL_checkinteger(pL, 1);
	bool ret = GameScene::getInstance()->getBagLayer()->removeGold(gold);

	lua_pushboolean(pL, ret);
	return 1;
}


int addGood(lua_State* pL)
{
	auto name = luaL_checkstring(pL, 1);
	int number = (int)luaL_checkinteger(pL, 2);

	GameScene::getInstance()->addGood(name, number);

	return 0;
}

int subGood(lua_State* pL)
{
	auto name = luaL_checkstring(pL, 1);
	int number = (int)luaL_checkinteger(pL, 2);

	bool ret = GameScene::getInstance()->removeGood(name, number);

	lua_pushboolean(pL, ret);
	return 1;
}

int subEquipment(lua_State* pL)
{
	auto id = (int)luaL_checkinteger(pL, 1);
	auto goodName = luaL_checkstring(pL, 2);
	int number = (int)luaL_checkinteger(pL, 3);

	auto character = GameScene::getInstance()->getCharacterByID(id);
	auto playerName = character->getChartletName();

	bool ret = DynamicData::getInstance()->removeEquipment(playerName, goodName, number);
	if (ret)
	{
		GameScene::getInstance()->getBagLayer()->updateGoods();
	}

	lua_pushboolean(pL, ret);
	return 1;
}

int getEquipment(lua_State* pL)
{
	auto id = (int)luaL_checkinteger(pL, 1);
	auto equipmentType = static_cast<EquipmentType>(luaL_checkinteger(pL, 2));

	auto character = GameScene::getInstance()->getCharacterByID(id);
	auto playerName = character->getChartletName();
	
	auto good = DynamicData::getInstance()->getEquipment(playerName, equipmentType);

	if (good != nullptr)
	{
		auto goodName = good->getTableName();

		lua_pushstring(pL, goodName.c_str());
		return 1;
	}
	return 0;
}

int getGoodNumber(lua_State* pL)
{
	auto goodName = luaL_checkstring(pL, 1);
	auto good = DynamicData::getInstance()->getGood(goodName);
	int number = 0;

	if (good != nullptr)
	{
		number = good->getNumber();
	}
	lua_pushinteger(pL, number);
	return 1;
}

int getName(lua_State* pL)
{
	int id = (int)luaL_checkinteger(pL, -1);
	auto character = GameScene::getInstance()->getCharacterByID(id);

	if (character == nullptr)
		return 0;
	//TODO: ???
	auto sID = character->getChartletName();
	//auto& name = DynamicData::getInstance()->getName(sID);

	lua_pushstring(pL, sID.c_str());
	return 1;
}

int recover(lua_State* pL)
{
	int id = (int)luaL_checkinteger(pL, 1);
	int hp = (int)luaL_checkinteger(pL, 2);
	int mp = 0;

	if (lua_gettop(pL) == 3)
	{
		mp = (int)luaL_checkinteger(pL, 3);
	}

	auto gameScene = GameScene::getInstance();
	auto gameState = gameScene->getGameState();
	auto playerManager = gameScene->getPlayerManager();
	auto dynamicData = DynamicData::getInstance();
	auto battleScene = gameScene->getBattleScene();
	//恢复血量
	auto& list = playerManager->getCharacterList();
	/*当id为-1时，表示全队进行恢复血量
		当id=0、1、2、3时，为特定的人物恢复血量
		当id=为对应的某个人物的id时，恢复血量*/
	for (unsigned int i = 0; i < list.size(); i++)
	{
		auto character = list.at(i);
		auto uniqueID = character->getUniqueID();
		if (id == -1 || id == i || id == uniqueID)
		{
			auto chartletName = character->getChartletName();
			int curHp = dynamicData->getProperty(chartletName, PropertyType::Hp);

			dynamicData->setProperty(chartletName, PropertyType::Hp, curHp + hp);
			//增加的mp大于0时，才会增加
			if (mp > 0)
			{
				int curMp = dynamicData->getProperty(chartletName, PropertyType::Mp);
				dynamicData->setProperty(chartletName, PropertyType::Mp, curMp + mp);
			}
			//当处于战斗状态下，进行刷新显示
			if (gameState == GameState::Fighting)
			{
				auto turn = battleScene->getTurnByID(uniqueID);
				battleScene->updateStateOfTurn(turn);
			}
		}
	}
	if (gameState != GameState::Fighting)
	{
		auto bagLayer = gameScene->getBagLayer();
		bagLayer->updateShownOfProp();
	}
	return 0;
}