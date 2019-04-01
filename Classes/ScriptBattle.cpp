#include "ScriptBattle.h"
#include "GameScene.h"
#include "BattleScene.h"
#include "BattleLayer.h"
#include "DynamicData.h"
#include "StaticData.h"
#include "PlayerLayer.h"
#include "Character.h"
#include "Actor.h"
#include "EffectLayer.h"
#include "GoodLayer.h"
#include "MapLayer.h"
#include "Fighter.h"

//------------------------------battle------------------------------------
int open_battle(lua_State* pL)
{
	const luaL_Reg lib[] = {
		{"specifyBattleback", specifyBattleback},
		{"startBattle", startBattle},
		{"showEnemyAnimation",showEnemyAnimation},
		{"showBattleAnimation", showBattleAnimation},
		{"showBullet", showBullet},
		{"hurt", hurt},
		{"heal", heal},
		{"searchFighter", searchFighter},
		{"roundOver", roundOver},
		{"addReward", addReward},
		{"getProperty", getProperty},
		{"setProperty", setProperty},
		{"getHitRate", getHitRate},
		{"getEvasionRate", getEvasionRate},
		{"getCriticalRate", getCriticalRate},
		{"getAttackScale", getAttackScale},
		{"setAttackScale", setAttackScale},
		{"getDefenseScale", getDefenseScale},
		{"setDefenseScale", setDefenseScale},
		{"changeFightState", changeFightState},
		{"setFlipX", setFlipX},
		{"isFlipX", isFlipX},
		{"moveToTarget", moveToTarget},
		{"backTo", backTo},
		{"setChartletName", setChartletName},
		{"getChartletName", getChartletName},
		{NULL, NULL}
	};
	luaL_newlib(pL, lib);
	return 1;
}

int specifyBattleback(lua_State* pL)
{
	auto battleScene = GameScene::getInstance()->getBattleScene();
	//设置战斗背景
	int n = lua_gettop(pL);
	for (int i = 0;i < n;i++)
	{
		battleScene->setBattleBack(luaL_checkstring(pL, i + 1), i);
	}
	return 0;
}

int startBattle(lua_State* pL)
{
	auto gameScene = GameScene::getInstance();
	auto playerLayer = gameScene->getPlayerLayer();
	auto battleScene = gameScene->getBattleScene();
	auto battleLayer = battleScene->getBattleLayer();
	auto dynamicData = DynamicData::getInstance();
	//设置状态
	gameScene->setGameState(GameState::Fighting);
	//出现战斗场景
	battleScene->setVisible(true);
	//隐藏地图
	gameScene->getMapLayer()->setVisible(false);
	//设置战斗未结束标识
	battleScene->setBattleOver(false);
	//开始回合
	battleLayer->setRoundOver(true);
	//播放战斗音乐
	SoundManager::getInstance()->playBackgroundMusic(STATIC_DATA_STRING("battle_bgm"), -1);
	SoundManager::getInstance()->playEffect(STATIC_DATA_STRING("battle_me"), 0);
	//主角进入战斗状态，并更新状态 TODO
	auto& characterList = playerLayer->getCharacterList();
	//最多上场4个人物
	for (unsigned i = 0;i < 4;i ++)
	{
		Character* player = nullptr;
		Actor* actor = nullptr;

		if (i < characterList.size())
		{
			//获取主角
			player = characterList.at(i);
			auto chartletName = player->getChartletName();
			int uniqueID = player->getUniqueID();

			actor = Actor::create(chartletName, uniqueID);
			//添加主角
			battleLayer->addOur(actor);
			//更新
			int hp = actor->getProperty(PropertyType::Hp);
			int maxHp = actor->getMaxHitPoint();
			int mp = actor->getProperty(PropertyType::Mp);
			int maxMp = actor->getMaxManaPoint();

			auto faceSpriteFrame = StaticData::getInstance()->getFaceSpriteFrame(chartletName);

			battleScene->setVisibilityOfStatePanel(i,true);
			battleScene->updateStateOfPlayer(i,chartletName,hp,maxHp,mp,maxMp,faceSpriteFrame);
		}
		else//隐藏不需要的状态面板
		{
			battleScene->setVisibilityOfStatePanel(i,false);
		}
	}
	//添加敌人
	size_t n = lua_rawlen(pL, -1);

	for (size_t i = 1;i <= n;i++)
	{
		lua_geti(pL, -1, i);
		//获取name和number
		lua_getfield(pL, -1, "number");
		lua_getfield(pL, -2, "name");

		auto name = luaL_checkstring(pL, -1);
		int number = (int)luaL_checkinteger(pL, -2);
		//添加敌人
		battleLayer->addEnemy(name, number);

		lua_pop(pL, 3);
	}
	return 0;
}

int showEnemyAnimation(lua_State* pL)
{
	int id = (int)luaL_checkinteger(pL, 1);
	auto animationName = luaL_checkstring(pL, 2);

	auto gameScene = GameScene::getInstance();
	auto battleLayer = gameScene->getBattleScene()->getBattleLayer();
	auto fighter = battleLayer->getFighterByID(id);

	if (fighter == nullptr)
		return 0;

	auto animation = AnimationCache::getInstance()->getAnimation(animationName);
	auto animate = Animate::create(animation);
	animate->setTag(Entity::ANIMATION_TAG);

	//智能更新动画
	if (fighter->getSprite() == nullptr)
	{
		fighter->bindSpriteWithAnimate(animate);
	}
	else
	{
		fighter->getSprite()->stopActionByTag(Entity::ANIMATION_TAG);
		fighter->getSprite()->runAction(animate);
	}
	//放入动画持续时间
	lua_pushnumber(pL, animation->getDuration());

	return 1;
}

int showBattleAnimation(lua_State* pL)
{
	auto id = (int)luaL_checkinteger(pL, 1);
	auto animationName = luaL_checkstring(pL, 2);
	bool flipX = false;

	if (lua_gettop(pL) == 3)
	{
		flipX = lua_toboolean(pL, 3) == 1;
	}

	auto gameScene = GameScene::getInstance();
	auto battleLayer = gameScene->getBattleScene()->getBattleLayer();
	auto effectLayer = gameScene->getEffectLayer();
	//获取对应id的Fighter
	auto fighter = battleLayer->getFighterByID(id);

	if (fighter == nullptr)
	{
		printf("LUA_showBattleAnimation:fighter not found\n");
		return 0;
	}
	auto pos = fighter->getPosition();
	//显示动画
	float duration = effectLayer->showAnimationEffect(animationName, pos, flipX, battleLayer);
	//放入持续时间
	lua_pushnumber(pL, duration);

	return 1;
}

int showBullet(lua_State* pL)
{
	auto id1 = (int)luaL_checkinteger(pL, 1);
	auto id2 = (int)luaL_checkinteger(pL, 2);
	auto bulletName = luaL_checkstring(pL, 3);

	auto gameScene = GameScene::getInstance();
	auto battleScene = gameScene->getBattleScene();
	auto fighter1 = battleScene->getBattleLayer()->getFighterByID(id1);
	auto fighter2 = battleScene->getBattleLayer()->getFighterByID(id2);

	if (fighter1 == nullptr || fighter2 == nullptr)
		return 0;
	
	auto pos1 = fighter1->getPosition();
	auto pos2 = fighter2->getPosition();

	auto size1 = fighter1->getContentSize();

	float duration = gameScene->getEffectLayer()->showBullet(pos1, pos2, size1, bulletName);
	lua_pushnumber(pL, duration);
	return 1;
}

int hurt(lua_State* pL)
{
	auto gameScene = GameScene::getInstance();
	auto gameState = gameScene->getGameState();

	if (gameState == GameState::Fighting)
	{
		//获取id
		int uniqueID = (int)luaL_checkinteger(pL, 1);
		int damage = static_cast<int>(luaL_checknumber(pL, 2));
		AttributeType attributeType = AttributeType::None;
		HurtType hurtType = HurtType::Attack;
		//如果有第3个值，获取
		if (lua_gettop(pL) >= 3)
		{
			attributeType = static_cast<AttributeType>(luaL_checkinteger(pL, 3));
		}
		if (lua_gettop(pL) == 4)
		{
			hurtType = static_cast<HurtType>(luaL_checkinteger(pL, 4));
		}
		//获取对应fighter
		auto battleScene = gameScene->getBattleScene();
		auto effectLayer = gameScene->getEffectLayer();

		auto turn = gameScene->getBattleScene()->getBattleLayer()->getTurnByID(uniqueID);
		auto fighter = turn->fighter;

		if (fighter == nullptr)
		{
			printf("ScriptFunc LUA_hurtOfBattle:error fighter not found\n");
			return 0;
		}
		auto size = fighter->getContentSize();
		auto pos = fighter->getPosition() - Point(0.f,size.height / 2);
		//获取伤害值
		auto afterDamage = fighter->hurt(damage,attributeType,hurtType);
		//显示飘字
		effectLayer->showWord(-afterDamage, pos, hurtType);
		//更新对应控件
		battleScene->updateStateOfTurn(turn);
	}
	return 0;
}

int heal(lua_State* pL)
{
	int uniqueID = (int)luaL_checkinteger(pL, 1);
	int value = (int)luaL_checkinteger(pL, 2);

	auto dynamicData = DynamicData::getInstance();
	auto gameScene = GameScene::getInstance();
	auto gameState = gameScene->getGameState();
	//获取人物
	if (gameState == GameState::Script || gameState == GameState::Normal)
	{
		auto playerLayer = gameScene->getPlayerLayer();
		auto goodLayer = gameScene->getGoodLayer();
		auto player = playerLayer->getPlayerOfID(uniqueID);

		if (player == nullptr)
		{
			printf("eror:LUA_heal player not found\n");
			return 0;
		}
		auto name = player->getChartletName();
		auto curHp = dynamicData->getProperty(name, PropertyType::Hp);

		dynamicData->setProperty(name,PropertyType::Hp,value + curHp);
		//更新显示
		goodLayer->updateShownOfGoods();
		goodLayer->updateShownOfProps();
	}
	else if (gameState == GameState::Fighting)
	{
		auto battleScene = gameScene->getBattleScene();
		auto effectLayer = gameScene->getEffectLayer();
		auto turn = battleScene->getBattleLayer()->getTurnByID(uniqueID);

		if (turn == nullptr)
		{
			printf("error:LUA_heal turn == nullptr\n");
			return 0;
		}
		auto fighter = turn->fighter;
		auto curHp = fighter->getProperty(PropertyType::Hp);

		auto size = fighter->getContentSize();
		auto pos = fighter->getPosition() - Point(0.f,size.height / 2);
		//显示飘字
		effectLayer->showWord(value,pos, HurtType::None);
		//修改hp
		fighter->setProperty(PropertyType::Hp, value + curHp);
		//更新面板
		battleScene->updateStateOfTurn(turn);
		//更新角色状态
		fighter->updateSelf();
	}
	return 0;
}

int searchFighter(lua_State* pL)
{
	auto gameScene = GameScene::getInstance();

	TurnType turnType = static_cast<TurnType>(luaL_checkinteger(pL, 1));
	SearchType searchType = static_cast<SearchType>(luaL_checkinteger(pL, 2));

	auto fighter = gameScene->getBattleScene()->getBattleLayer()->searchFighter(turnType, searchType);
	
	if (fighter != nullptr)
	{
		int uniqueID = fighter->getFighterID();
		lua_pushinteger(pL, (lua_Integer)uniqueID);
		return 1;
	}
	else
		return 0;
}

int roundOver(lua_State* pL)
{
	GameScene::getInstance()->getBattleScene()->getBattleLayer()->roundOver();

	return 0;
}

int addReward(lua_State* pL)
{
	int exp = (int) luaL_checkinteger(pL, 2);
	int gold = (int) luaL_checkinteger(pL, 3);

	auto battleScene = GameScene::getInstance()->getBattleScene();
	//获取掉落物品
	int n = lua_gettop(pL);
	lua_pushnil(pL);  /* first key */
	while (lua_next(pL, -n - 1) != 0) 
	{
		/* uses 'key' (at index -2) and 'value' (at index -1) */
		auto name = lua_tostring(pL, -2);
		int number = (int)lua_tonumber(pL, -1);
		//添加物品
		battleScene->addReward(name, number);
		/* removes 'value'; keeps 'key' for next iteration */
		lua_pop(pL, 1);
	}
	//添加经验和金币
	battleScene->addReward(exp, gold);
	return 0;
}

int getProperty(lua_State* pL)
{
	int id = (int)luaL_checkinteger(pL, 1);
	PropertyType type = static_cast<PropertyType>(luaL_checkinteger(pL, 2));
	
	auto gameScene = GameScene::getInstance();
	auto gameState = gameScene->getGameState();
	//正常 或脚本状态下
	if (gameState == GameState::Normal || gameState == GameState::Script)
	{
		auto player = gameScene->getPlayerLayer()->getPlayerOfID(id);
		
		if (player == nullptr)
		{
			printf("error:LUA_getProperty player not found\n");
			return 0;
		}
		//获取属性
		auto chartletName = player->getChartletName();
		int value = DynamicData::getInstance()->getProperty(chartletName, type);
		//返回属性
		lua_pushinteger(pL, value);
		return 1;
	}
	else if (gameState == GameState::Fighting)
	{
		auto turn = gameScene->getBattleScene()->getBattleLayer()->getTurnByID(id);
		auto fighter = turn->fighter;
		auto value = fighter->getProperty(type);
		//返回属性
		lua_pushinteger(pL, value);
		return 1;
	}
	return 0;
}

int setProperty(lua_State* pL)
{
	int id = (int)luaL_checkinteger(pL, 1);
	PropertyType type = static_cast<PropertyType>(luaL_checkinteger(pL, 2));
	int value = (int)luaL_checkinteger(pL, 3);

	auto gameScene = GameScene::getInstance();
	auto gameState = gameScene->getGameState();

	if (gameState == GameState::Normal || gameState == GameState::Script)
	{
		auto goodLayer = gameScene->getGoodLayer();
		auto player = gameScene->getPlayerLayer()->getPlayerOfID(id);
		
		if (player == nullptr)
		{
			printf("error:LUA_getProperty player not found\n");
			return 0;
		}
		//设置属性
		auto chartletName = player->getChartletName();
		DynamicData::getInstance()->setProperty(chartletName,type,value);
		//更新显示
		//goodLayer->updateProperties();
	}
	else if (gameState == GameState::Fighting)
	{
		auto battleScene = gameScene->getBattleScene();
		auto turn = battleScene->getBattleLayer()->getTurnByID(id);

		auto fighter = turn->fighter;
		fighter->setProperty(type, value);
		//更新
		battleScene->updateStateOfTurn(turn);
	}
	return 0;
}

int getHitRate(lua_State* pL)
{
	auto id = (int)luaL_checkinteger(pL, 1);

	auto gameScene = GameScene::getInstance();
	auto battleScene = gameScene->getBattleScene();

	auto fighter = battleScene->getBattleLayer()->getFighterByID(id);
	auto value = fighter->getHitRate();

	lua_pushnumber(pL, value);
	return 1;
}

int getEvasionRate(lua_State* pL)
{
	auto id = (int)luaL_checkinteger(pL, 1);

	auto gameScene = GameScene::getInstance();
	auto battleScene = gameScene->getBattleScene();

	auto fighter = battleScene->getBattleLayer()->getFighterByID(id);
	auto value = fighter->getEvasionRate();

	lua_pushnumber(pL, value);
	return 1;
}

int getCriticalRate(lua_State* pL)
{
	auto id = (int)luaL_checkinteger(pL, 1);

	auto gameScene = GameScene::getInstance();
	auto battleScene = gameScene->getBattleScene();

	auto fighter = battleScene->getBattleLayer()->getFighterByID(id);
	auto value = fighter->getCriticalRate();

	lua_pushnumber(pL, value);
	return 1;
}

int getAttackScale(lua_State* pL)
{
	auto id = (int)luaL_checkinteger(pL, 1);

	auto gameScene = GameScene::getInstance();
	auto battleScene = gameScene->getBattleScene();

	auto fighter = battleScene->getBattleLayer()->getFighterByID(id);
	auto value = fighter->getAttackScale();

	lua_pushnumber(pL, value);
	return 1;
}

int setAttackScale(lua_State* pL)
{
	auto id = (int)luaL_checkinteger(pL, 1);
	auto scale = (float)luaL_checknumber(pL, 2);

	auto gameScene = GameScene::getInstance();
	auto battleScene = gameScene->getBattleScene();

	auto fighter = battleScene->getBattleLayer()->getFighterByID(id);
	fighter->setAttackScale(scale);

	return 0;
}

int getDefenseScale(lua_State* pL)
{
	auto id = (int)luaL_checkinteger(pL, 1);

	auto gameScene = GameScene::getInstance();
	auto battleScene = gameScene->getBattleScene();

	auto fighter = battleScene->getBattleLayer()->getFighterByID(id);
	auto value = fighter->getDefenseRate();

	lua_pushnumber(pL, value);
	return 1;
}

int setDefenseScale(lua_State* pL)
{
	auto id = (int)luaL_checkinteger(pL, 1);
	auto scale = (float)luaL_checknumber(pL, 2);

	auto gameScene = GameScene::getInstance();
	auto battleScene = gameScene->getBattleScene();

	auto fighter = battleScene->getBattleLayer()->getFighterByID(id);
	fighter->setDefenseRate(scale);

	return 0;
}

int changeFightState(lua_State* pL)
{
	auto id = (int)luaL_checkinteger(pL, 1);
	int nState = (int)luaL_checkinteger(pL, 2);
	auto state = static_cast<FightState>(nState);

	auto gameScene = GameScene::getInstance();
	auto battleScene = gameScene->getBattleScene();

	auto fighter = battleScene->getBattleLayer()->getFighterByID(id);

	auto action = fighter->changeFightState(state);
	auto duration = action->getDuration();

	lua_pushnumber(pL, duration);
	return 1;
}

int setFlipX(lua_State* pL)
{
	auto id = (int)luaL_checkinteger(pL, 1);
	bool flipX = lua_toboolean(pL, 2) == 1;

	auto gameScene = GameScene::getInstance();
	auto battleScene = gameScene->getBattleScene();

	auto fighter = battleScene->getBattleLayer()->getFighterByID(id);
	fighter->getSprite()->setFlipX(flipX);

	return 0;
}

int isFlipX(lua_State* pL)
{
	auto id = (int)luaL_checkinteger(pL, 1);
	bool flipX = lua_toboolean(pL, 2) == 1;

	auto gameScene = GameScene::getInstance();
	auto battleScene = gameScene->getBattleScene();
	auto fighter = battleScene->getBattleLayer()->getFighterByID(id);

	lua_pushboolean(pL, fighter->getSprite()->isFlipX());
	return 1;
}

int moveToTarget(lua_State* pL)
{
	auto id1 = (int)luaL_checkinteger(pL, 1);
	auto id2 = (int)luaL_checkinteger(pL, 2);

	auto gameScene = GameScene::getInstance();
	auto battleScene = gameScene->getBattleScene();
	auto fighter1 = battleScene->getBattleLayer()->getFighterByID(id1);
	auto fighter2 = battleScene->getBattleLayer()->getFighterByID(id2);

	if (fighter1 == nullptr || fighter2 == nullptr)
		return 0;

	auto duration = fighter1->moveToTarget(fighter2);

	lua_pushnumber(pL, duration);
	return 1;
}

int backTo(lua_State* pL)
{
	auto id = (int)luaL_checkinteger(pL, 1);

	auto gameScene = GameScene::getInstance();
	auto battleScene = gameScene->getBattleScene();
	auto fighter = battleScene->getBattleLayer()->getFighterByID(id);

	auto duration = fighter->backTo();
	lua_pushnumber(pL, duration);

	return 1;
}

int setChartletName(lua_State* pL)
{
	auto id = (int)luaL_checkinteger(pL, 1);
	auto chartletName = luaL_checkstring(pL, 2);

	auto gameScene = GameScene::getInstance();
	auto battleScene = gameScene->getBattleScene();
	auto fighter = battleScene->getBattleLayer()->getFighterByID(id);

	fighter->setChartletName(chartletName);
	return 0;
}

int getChartletName(lua_State* pL)
{
	auto id = (int)luaL_checkinteger(pL, 1);

	auto gameScene = GameScene::getInstance();
	auto battleScene = gameScene->getBattleScene();
	auto fighter = battleScene->getBattleLayer()->getFighterByID(id);

	lua_pushstring(pL, fighter->getChartletName().c_str());
	return 1;
}