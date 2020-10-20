#include "Actor.h"
#include "HpBar.h"
#include "Fighter.h"
#include "BattleScene.h"
#include "BattleLayer.h"
#include "BattleResult.h"
#include "PriorityQueue.h"

#include "../GameScene.h"
#include "../manager/PlayerManager.h"

#include "../ui/BagLayer.h"
#include "../ui/GoodLayer.h"

#include "../entity/Good.h"
#include "../entity/Character.h"

#include "../data/DynamicData.h"
#include "../data/StaticData.h"

BattleScene::BattleScene()
	:m_pBattleLayer(nullptr)
	,m_pPanelLayer(nullptr)
	,m_pBattleResult(nullptr)
{
}

BattleScene::~BattleScene()
{
}

bool BattleScene::init()
{
	//战斗层
	m_pBattleLayer = BattleLayer::create();
	this->addChild(m_pBattleLayer);
	//ui层
	m_pPanelLayer = BattlePanelLayer::create("scene/battle_scene.xml");
	m_pPanelLayer->setDelegate(this);
	this->addChild(m_pPanelLayer);
	//战斗结果层
	m_pBattleResult = BattleResult::create("scene/battle/battle_result_layer.xml");
	this->addChild(m_pBattleResult);
	//添加fighter死亡监听
	_eventDispatcher->addEventCustomListener(BATTLE_FIGHTER_DEAD_EVENT, SDL_CALLBACK_1(BattleScene::fighterDeadCallback, this), this);
	//显示玩家操作按钮监听
	_eventDispatcher->addEventCustomListener(BATTLE_PLAYER_OPERATION, SDL_CALLBACK_1(BattleScene::showOperationBtnCallback, this), this);

	return true;
}

void BattleScene::startBattle(const unordered_map<string, int>& enemyData)
{
	//初始化
	m_pBattleResult->setBattleOver(false);
	m_pBattleResult->setVisible(false);
	m_pBattleLayer->roundOver();
	//上场我方人员 最多4人
	vector<Character*> characterList = GameScene::getInstance()->getCharacterList();
	for (unsigned i = 0; i < 4; i++)
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
			m_pBattleLayer->addOur(actor);
			//更新
			int hp = actor->getProperty(PropertyType::Hp);
			int mp = actor->getProperty(PropertyType::Mp);
			int maxHp = actor->getMaxHitPoint();
			int maxMp = actor->getMaxManaPoint();
			auto faceSpriteFrame = StaticData::getInstance()->getFaceSpriteFrame(chartletName);

			m_pPanelLayer->setVisibileOfStatePanel(i, true);
			m_pPanelLayer->updateStateOfPlayer(i, chartletName, hp, maxHp, mp, maxMp, faceSpriteFrame);
		}
		else//隐藏不需要的状态面板
		{
			m_pPanelLayer->setVisibileOfStatePanel(i, false);
		}
	}
	//添加敌人
	for (auto it = enemyData.begin(); it != enemyData.end(); it++)
	{
		m_pBattleLayer->addEnemy(it->first, it->second);
	}
}

void BattleScene::update(float dt)
{
	//只有战斗未结束，才进行出手更新
	if (!m_pBattleResult->isBattleOver())
	{
		m_pBattleLayer->update(dt);
	}
}

void BattleScene::setBattleBack(const string& back, int index)
{
	m_pBattleLayer->setBattleBack(back, index);
}

void BattleScene::updateStateOfTurn(Turn* turn)
{
	auto fighter = turn->fighter;
	int hp = fighter->getProperty(PropertyType::Hp);

	//是主角或是友军
	if (turn->type == TurnType::Friend || turn->type == TurnType::Player)
	{
		int maxHp = fighter->getMaxHitPoint();
		int mp = fighter->getProperty(PropertyType::Mp);
		int maxMp = fighter->getMaxManaPoint();
		auto sName = fighter->getFighterName();

		auto playerManager = GameScene::getInstance()->getPlayerManager();
		int index = playerManager->getIndexOfCharacter(fighter->getChartletName());

		m_pPanelLayer->updateStateOfPlayer(index, sName, hp, maxHp, mp, maxMp, nullptr);
	}//是敌人，仅进行血量更新
	else if (turn->type == TurnType::Enemy)
	{
		if (turn->hpBar != nullptr)
			turn->hpBar->setValue((float)hp);
	}
}

bool BattleScene::onTouchBegan(Touch* touch, SDL_Event* event)
{
	//已经显示结算面版，表示战斗结束
	if (m_pBattleResult->isVisible())
	{
		m_pBattleResult->setVisible(false);
		//TODO:战斗结束 清除BattleScene
		//GameScene::getInstance()->endBattle();
	}
	auto type = m_pPanelLayer->getClickedType();
	if (type == BattlePanelLayer::ClickedType::None)
		return false;

	auto pos = touch->getLocation();
	Turn* turn = m_pBattleLayer->getClickedTurn(pos);
	bool bSuccess = false;

	if (turn == nullptr)
		return false;
	//当前“活动”的玩家
	auto player = m_pBattleLayer->getTopTurn()->fighter;
	//点击到活着的敌人才会进行攻击
	if (type == BattlePanelLayer::ClickedType::Attack
		&& turn != nullptr && turn->type == TurnType::Enemy
		&& turn->fighter->getProperty(PropertyType::Hp) > 0)
	{
		//主角攻击
		player->execute(turn->fighter);

		bSuccess = true;
		m_pPanelLayer->setClickedType(BattlePanelLayer::ClickedType::None);
	}
	//使用道具
	auto good = m_pPanelLayer->getGood();
	if (type == BattlePanelLayer::ClickedType::Good && good != nullptr)
	{
		//获取使用范围
		auto usageRange = good->getUsageRange();
		//只能是自己
		if ((usageRange == UsageRange::SingleAlly && turn->type != TurnType::Enemy)
			|| (usageRange == UsageRange::SingleEnemy && turn->type == TurnType::Enemy))
		{
			auto userId = player->getFighterID();
			auto targetId = turn->fighter->getFighterID();

			good->execute(userId, targetId);
			//使用道具
			player->useItem(good);
			m_pPanelLayer->setGood(nullptr);

			bSuccess = true;

			SoundManager::getInstance()->playEffect(STATIC_DATA_STRING("use_good_se"), 0);
		}
		m_pPanelLayer->setClickedType(BattlePanelLayer::ClickedType::None);
	}

	if (bSuccess)
	{
		m_pPanelLayer->setVisibileOfUndoBtn(false);
	}
	return true;
}

void BattleScene::clear()
{
	m_pBattleLayer->clear();
	m_pBattleResult->clear();
}

void BattleScene::attackBtnCallback()
{
	//什么也不做
}

void BattleScene::magicBtnCallback()
{
	//打开背包，并锁定角色
	auto curTurn = m_pBattleLayer->getTopTurn();
	auto layer = GameScene::getInstance()->getBagLayer();
	layer->setType(BagLayer::Type::Skill);
	layer->lockPlayer(curTurn->fighter->getFighterID());
}

void BattleScene::goodBtnCallback()
{
	//打开背包，并锁定角色
	auto curTurn = m_pBattleLayer->getTopTurn();
	auto layer = GameScene::getInstance()->getBagLayer();
	layer->setType(BagLayer::Type::Bag);
	layer->lockPlayer(curTurn->fighter->getFighterID());
}

void BattleScene::guardBtnCallback()
{
	//防御，然后回合结束
	auto turn = m_pBattleLayer->getTopTurn();
	turn->fighter->guard();
	m_pBattleLayer->roundOver();
}

void BattleScene::escapeBtnCallback()
{
	//主角逃跑
	m_pBattleLayer->getTopTurn()->fighter->escape();
	//TODO:结束战斗
	m_pBattleLayer->endBattle();
}

void BattleScene::undoBtnCallback()
{
}

void BattleScene::showOperationBtnCallback(EventCustom* eventCustom)
{
	m_pPanelLayer->setVisibileOfActionBtns(true);
	m_pPanelLayer->setVisibileOfUndoBtn(false);
}

void BattleScene::fighterDeadCallback(EventCustom* eventCustom)
{
	auto fighter = static_cast<Fighter*>(eventCustom->getUserData());
	m_pBattleLayer->fighterDead(fighter);
	//任何一方人数为0，战斗结束
	int nEnemyNumber = m_pBattleLayer->getEnemyNumber();
	int nOurNumber = m_pBattleLayer->getOurNumber();
	//战斗还未结束
	if (nOurNumber != 0 && nEnemyNumber != 0)
		return;
	//胜利或失败
	int victory = (nEnemyNumber == 0)? 1 : 0;
	vector<string> deadOurNames = m_pBattleLayer->endBattle();
	//TODO:死亡的友军，设置为1滴血
	for (auto& name : deadOurNames)
	{
		/*
		DynamicData::getInstance()->setProperty(turn->fighter->getFighterName(), PropertyType::Hp, 1);
		GameScene::getInstance()->getGoodLayer()->updateShownOfProps();
		*/
	}
	m_pBattleResult->showSummary(victory);
	//增加经验、金币 掉落物品
	if (victory == 1)
	{
	/*
		auto gameScene = GameScene::getInstance();
		gameScene->addGold(m_nGold);
		gameScene->addExp(m_nExp);
	*/
	}
}
