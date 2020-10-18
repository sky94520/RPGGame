#include "BattlePanelLayer.h"
#include "../entity/Good.h"

BattlePanelLayer::BattlePanelLayer()
	:m_pXmlNode(nullptr)
	,m_pStateNode(nullptr)
	,m_bVisibility(true)
	,m_clickedType(ClickedType::None)
	,m_pGood(nullptr)
{
}

BattlePanelLayer::~BattlePanelLayer()
{
}

BattlePanelLayer* BattlePanelLayer::create(const string& xmlPath)
{
	auto layer = new BattlePanelLayer();
	if (layer && layer->init(xmlPath))
		layer->autorelease();
	else
		SDL_SAFE_DELETE(layer);
	return layer;
}

bool BattlePanelLayer::init(const string& xmlPath)
{
	//加载xml
	m_pXmlNode = UIWidgetManager::getInstance()->createWidgetsWithXml(xmlPath);
	this->addChild(m_pXmlNode);
	//状态节点
	m_pStateNode = m_pXmlNode->getChildByName("battle_state_layer");
	//行动按钮绑定
	auto actionNode = m_pXmlNode->getChildByName("battle_action_layer");

	auto attackBtn = actionNode->getChildByName<ui::Button*>("attack_btn");
	auto magicBtn = actionNode->getChildByName<ui::Button*>("magic_btn");
	auto goodBtn = actionNode->getChildByName<ui::Button*>("good_btn");
	auto guardBtn = actionNode->getChildByName<ui::Button*>("guard_btn");
	auto escapeBtn = actionNode->getChildByName<ui::Button*>("escape_btn");

	attackBtn->addClickEventListener(SDL_CALLBACK_1(BattlePanelLayer::attackCallback, this));
	magicBtn->addClickEventListener(SDL_CALLBACK_1(BattlePanelLayer::magicCallback, this));
	goodBtn->addClickEventListener(SDL_CALLBACK_1(BattlePanelLayer::goodCallback, this));
	guardBtn->addClickEventListener(SDL_CALLBACK_1(BattlePanelLayer::guardCallback, this));
	escapeBtn->addClickEventListener(SDL_CALLBACK_1(BattlePanelLayer::escapeCallback, this));

	m_pXmlNode->getChildByName<ui::Button*>("undo_btn")->addClickEventListener(SDL_CALLBACK_1(BattlePanelLayer::undoCallback, this));
	return true;
}

void BattlePanelLayer::setVisibilityOfActionBtns(bool visibility)
{
	if (m_bVisibility != visibility)
	{
		Point pos;
		auto node = m_pXmlNode->getChildByName("battle_action_layer");
		m_bVisibility = visibility;

		pos.x += m_bVisibility ? 0.f : 100.f;
		//行动按钮
		MoveTo* move = MoveTo::create(0.8f, pos);
		EaseExponentialOut*action = EaseExponentialOut::create(move);
		action->setTag(1);

		node->stopActionByTag(1);
		node->runAction(action);
	}
}

void BattlePanelLayer::setVisibilityOfUndoBtn(bool visibility)
{
	auto undoBtn = m_pXmlNode->getChildByName<ui::Button*>("undo_btn");

	undoBtn->setTouchEnabled(visibility);
	undoBtn->setVisible(visibility);
}

void BattlePanelLayer::setVisibilityOfStatePanel(int index, bool visibility)
{
	auto node = m_pStateNode->getChildByTag(index);
	node->setVisible(visibility);
}

void BattlePanelLayer::updateStateOfPlayer(int i, const string& sName, int hp, int maxHp, int mp, int maxMp, SpriteFrame* frame)
{
	auto node = m_pStateNode->getChildByTag(i);

	LabelBMFont* nameLabel = node->getChildByName<LabelBMFont*>("name");
	LoadingBar* hpLoadingBar = node->getChildByName<LoadingBar*>("hp");
	LoadingBar* mpLoadingBar = node->getChildByName<LoadingBar*>("mp");
	LabelAtlas* hpLabel = node->getChildByName<LabelAtlas*>("hp_label");
	LabelAtlas* mpLabel = node->getChildByName<LabelAtlas*>("mp_label");

	Sprite* sprite = node->getChildByName<Sprite*>("face");
	//设置名字
	nameLabel->setString(sName);
	//设置数值
	hpLoadingBar->setPercent((float)hp / maxHp);
	mpLoadingBar->setPercent((float)mp / maxMp);

	auto hpStr = StringUtils::format("%d/%d", hp, maxHp);
	auto mpStr = StringUtils::format("%d/%d", mp, maxMp);

	hpLabel->setString(hpStr);
	mpLabel->setString(mpStr);
	//设置贴图
	if (frame != nullptr)
	{
		sprite->setSpriteFrame(frame);
	}
}

void BattlePanelLayer::setGood(Good* good)
{
	SDL_SAFE_RETAIN(good);
	SDL_SAFE_RELEASE_NULL(m_pGood);
	m_pGood = good;
}

Good* BattlePanelLayer::getGood()
{
	return m_pGood;
}

void BattlePanelLayer::attackCallback(Object* sender)
{
	//TODO:清空
	/*
	m_nExp = 0;
	m_nGold = 0;
	m_dropGoods.clear();
	*/
	//隐藏行动按钮，出现撤销按钮
	this->setVisibilityOfActionBtns(false);
	this->setVisibilityOfUndoBtn(true);

	m_clickedType = ClickedType::Attack;
}

void BattlePanelLayer::magicCallback(Object* sender)
{
	//隐藏行动按钮，出现撤销按钮
	this->setVisibilityOfActionBtns(false);
	//打开背包，并锁定角色
	/*
	auto goodLayer = GameScene::getInstance()->getGoodLayer();
	auto curTurn = m_pBattleLayer->getTopTurn();

	goodLayer->show(GoodLayer::Type::Skill);
	goodLayer->lockPlayer(curTurn->fighter->getFighterID());
	*/
	m_clickedType = ClickedType::Good;
}

void BattlePanelLayer::goodCallback(Object* sender)
{
	//隐藏行动按钮，出现撤销按钮
	this->setVisibilityOfActionBtns(false);
	//打开背包，并锁定角色
	/*
	auto goodLayer = GameScene::getInstance()->getGoodLayer();
	auto curTurn = m_pBattleLayer->getTopTurn();

	goodLayer->show(GoodLayer::Type::Bag);
	goodLayer->lockPlayer(curTurn->fighter->getFighterID());
	*/

	m_clickedType = ClickedType::Good;
}

void BattlePanelLayer::guardCallback(Object* sender)
{
	//隐藏行动按钮
	this->setVisibilityOfActionBtns(false);
	//获取当前的turn
	/*
	auto turn = m_pBattleLayer->getTopTurn();
	//防御
	turn->fighter->guard();
	//回合结束
	m_pBattleLayer->roundOver();
	*/
}

void BattlePanelLayer::escapeCallback(Object* sender)
{
	//隐藏行动按钮
	this->setVisibilityOfActionBtns(false);
	/*
	//主角逃跑
	m_pBattleLayer->getTopTurn()->fighter->escape();
	//结束战斗
	m_pBattleLayer->endBattle();
	*/
}

void BattlePanelLayer::undoCallback(Object* sender)
{
	//隐藏行动按钮
	this->setVisibilityOfActionBtns(true);
	//存在物品，则释放引用
	if (m_pGood != nullptr)
	{
		SDL_SAFE_RELEASE_NULL(m_pGood);
	}
}
