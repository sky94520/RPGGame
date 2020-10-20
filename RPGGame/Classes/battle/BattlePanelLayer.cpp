#include "BattlePanelLayer.h"
#include "../entity/Good.h"

BattlePanelLayer::BattlePanelLayer()
	:m_pXmlNode(nullptr)
	,m_pStateNode(nullptr)
	,m_bVisibileOfActionBtn(true)
	,m_bVisibleOfUndoBtn(false)
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

	attackBtn->addClickEventListener(SDL_CALLBACK_1(BattlePanelLayer::attackBtnCallback, this));
	magicBtn->addClickEventListener(SDL_CALLBACK_1(BattlePanelLayer::magicBtnCallback, this));
	goodBtn->addClickEventListener(SDL_CALLBACK_1(BattlePanelLayer::goodBtnCallback, this));
	guardBtn->addClickEventListener(SDL_CALLBACK_1(BattlePanelLayer::guardBtnCallback, this));
	escapeBtn->addClickEventListener(SDL_CALLBACK_1(BattlePanelLayer::escapeBtnCallback, this));

	auto undoBtn = m_pXmlNode->getChildByName<ui::Button*>("undo_btn");
	undoBtn->addClickEventListener(SDL_CALLBACK_1(BattlePanelLayer::clickUndoBtnCallback, this));
	return true;
}

void BattlePanelLayer::setVisibileOfActionBtns(bool visibility)
{
	if (m_bVisibileOfActionBtn != visibility)
	{
		Point pos;
		auto node = m_pXmlNode->getChildByName("battle_action_layer");
		m_bVisibileOfActionBtn = visibility;

		pos.x += m_bVisibileOfActionBtn ? 0.f : 100.f;
		//行动按钮
		MoveTo* move = MoveTo::create(0.8f, pos);
		EaseExponentialOut*action = EaseExponentialOut::create(move);
		action->setTag(1);

		node->stopActionByTag(1);
		node->runAction(action);
	}
}

void BattlePanelLayer::setVisibileOfUndoBtn(bool visibility)
{
	m_bVisibleOfUndoBtn = visibility;
	auto undoBtn = m_pXmlNode->getChildByName<ui::Button*>("undo_btn");

	undoBtn->setTouchEnabled(visibility);
	undoBtn->setVisible(visibility);
}

void BattlePanelLayer::setVisibileOfStatePanel(int index, bool visibility)
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

void BattlePanelLayer::attackBtnCallback(Object* sender)
{
	//隐藏行动按钮，出现撤销按钮
	this->setVisibileOfActionBtns(false);
	this->setVisibileOfUndoBtn(true);
	m_pDelegate->attackBtnCallback();

	m_clickedType = ClickedType::Attack;
}

void BattlePanelLayer::magicBtnCallback(Object* sender)
{
	//隐藏行动按钮，出现撤销按钮
	this->setVisibileOfActionBtns(false);
	this->setVisibileOfUndoBtn(true);
	m_pDelegate->magicBtnCallback();
	m_clickedType = ClickedType::Good;
}

void BattlePanelLayer::goodBtnCallback(Object* sender)
{
	//隐藏行动按钮，出现撤销按钮
	this->setVisibileOfActionBtns(false);
	m_pDelegate->goodBtnCallback();
	m_clickedType = ClickedType::Good;
}

void BattlePanelLayer::guardBtnCallback(Object* sender)
{
	//隐藏行动按钮
	this->setVisibileOfActionBtns(false);
	m_pDelegate->guardBtnCallback();
}

void BattlePanelLayer::escapeBtnCallback(Object* sender)
{
	//隐藏行动按钮
	this->setVisibileOfActionBtns(false);
	m_pDelegate->escapeBtnCallback();
}

void BattlePanelLayer::clickUndoBtnCallback(Object* sender)
{
	//显示行动按钮
	this->setVisibileOfActionBtns(true);
	//存在物品，则释放引用
	if (m_pGood != nullptr)
	{
		SDL_SAFE_RELEASE_NULL(m_pGood);
	}
}
