#include "OperationLayer.h"
#include "../GameScene.h"

OperationLayer::OperationLayer()
	:m_pBagBtn(nullptr)
	, m_pSaveBtn(nullptr)
	, m_pTaskBtn(nullptr)
	, m_pArrowBtn(nullptr)
	, m_bBtnHide(false)
{
}

OperationLayer::~OperationLayer()
{
}

bool OperationLayer::init()
{
	//加载ui
	auto node = ui::UIWidgetManager::getInstance()->createWidgetsWithXml("scene/operation_layer.xml");
	this->addChild(node);

	//背包按钮
	m_pBagBtn = node->getChildByName<ui::Button*>("bag_btn");
	m_pBagBtn->addClickEventListener(SDL_CALLBACK_1(OperationLayer::openBag, this));
	//保存按钮
	m_pSaveBtn = node->getChildByName<ui::Button*>("save_btn");
	m_pSaveBtn->addClickEventListener(SDL_CALLBACK_1(OperationLayer::save, this));
	//任务按钮
	m_pTaskBtn = node->getChildByName<ui::Button*>("task_btn");
	m_pTaskBtn->addClickEventListener(SDL_CALLBACK_1(OperationLayer::openTask, this));
	//箭头按钮
	m_pArrowBtn = node->getChildByName<ui::Button*>("arrow_btn");
	m_pArrowBtn->addClickEventListener(SDL_CALLBACK_1(OperationLayer::clickArrow, this));

	return true;
}

float OperationLayer::setTouchEnabled(bool enable)
{
	//当前可用且未隐藏  当前不可用且隐藏
	if (enable && !m_bBtnHide
		|| !enable && m_bBtnHide)
		return 0.f;
	m_bBtnHide = !enable;

	Size bagSize = m_pBagBtn->getContentSize();
	Size taskSize = m_pTaskBtn->getContentSize();
	Size saveSize = m_pSaveBtn->getContentSize();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	//需要隐藏 背包 任务 向左
	float duration = 0.3f;

	float bagDeltaX = m_bBtnHide ? -bagSize.width / 2 : bagSize.width / 2;
	float taskDeltaX = m_bBtnHide ? -taskSize.width / 2 : taskSize.width / 2;

	float saveDeltaX = m_bBtnHide ? visibleSize.width + saveSize.width / 2 : visibleSize.width - saveSize.width / 2;
	//创建
	MoveTo* move1 = MoveTo::create(duration, Point(bagDeltaX, m_pBagBtn->getPositionY()));
	MoveTo* move2 = MoveTo::create(duration, Point(taskDeltaX, m_pTaskBtn->getPositionY()));
	MoveTo* move3 = MoveTo::create(duration, Point(saveDeltaX, m_pSaveBtn->getPositionY()));

	move1->setTag(1);
	move2->setTag(1);
	move3->setTag(1);
	//停止原先的运动
	m_pBagBtn->stopActionByTag(1);
	m_pTaskBtn->stopActionByTag(1);
	m_pSaveBtn->stopActionByTag(1);
	//移动
	m_pBagBtn->runAction(move1);
	m_pTaskBtn->runAction(move2);

	m_pSaveBtn->runAction(move3);

	m_pBagBtn->setTouchEnabled(!m_bBtnHide);
	m_pTaskBtn->setTouchEnabled(!m_bBtnHide);
	m_pSaveBtn->setTouchEnabled(!m_bBtnHide);
	m_pArrowBtn->setTouchEnabled(!m_bBtnHide);

	return duration;
}

void OperationLayer::openBag(Object* sender)
{
}

void OperationLayer::save(Object* sender)
{
}

void OperationLayer::openTask(Object* sender)
{
}

void OperationLayer::clickArrow(Object* sender)
{
	float duration = this->setTouchEnabled(m_bBtnHide);
	//避免重复按，造成ui偏移
	m_pArrowBtn->setTouchEnabled(false);

	DelayTime* delay = DelayTime::create(duration);
	CallFunc* end = CallFunc::create([this]()
	{
		this->m_pArrowBtn->setTouchEnabled(true);
	});
	auto seq = Sequence::createWithTwoActions(delay, end);

	m_pArrowBtn->runAction(seq);
}