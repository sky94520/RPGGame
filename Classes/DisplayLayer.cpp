#include "DisplayLayer.h"
#include "GameScene.h"
#include "DynamicData.h"
#include "Text.h"

const int DisplayLayer::ITEM_NUMBER_PRE_PAGE = 6;

DisplayLayer::DisplayLayer()
	:m_pBtnGroup(nullptr)
	,m_pTitleLabel(nullptr)
	,m_pDescLabel(nullptr)
	,m_pStepLabel(nullptr)
	,m_pRewardText(nullptr)
	,m_pPageLabel(nullptr)
	,m_bOpened(false)
	,m_nCurPage(0)
	,m_type(Type::None)
{
}

DisplayLayer::~DisplayLayer()
{
}

bool DisplayLayer::init()
{
	auto node = ui::UIWidgetManager::getInstance()->createWidgetsWithXml("scene/display_layer.xml");
	this->addChild(node);
	//关闭按钮
	auto closeBtn = node->getChildByName<Button*>("close_btn");
	closeBtn->addClickEventListener(SDL_CALLBACK_1(DisplayLayer::closeCallback,this));
	//页数label
	m_pPageLabel = node->getChildByName<LabelAtlas*>("page_label");
	//上一页/下一页按钮
	auto prePageBtn = node->getChildByName<Button*>("pre_page_btn");
	auto nextPageBtn = node->getChildByName<Button*>("next_page_btn");
	//绑定回调函数
	prePageBtn->addClickEventListener(SDL_CALLBACK_1(DisplayLayer::turnPage, this, false));
	nextPageBtn->addClickEventListener(SDL_CALLBACK_1(DisplayLayer::turnPage, this, true));

	//各种描述按钮
	m_pTitleLabel = node->getChildByName<LabelBMFont*>("title");
	m_pDescLabel = node->getChildByName<LabelBMFont*>("description");
	m_pStepLabel = node->getChildByName<LabelBMFont*>("step");
	//奖励文本
	m_pRewardText = Text::create("fonts/1.fnt", 300, Color3B(0, 0, 0));
	m_pRewardText->setPosition(Point(380,370));
	m_pRewardText->setAnchorPoint(Point::ZERO);
	node->addChild(m_pRewardText);
	//TODO:任务单选
	m_pBtnGroup = RadioButtonGroup::create();
	this->addChild(m_pBtnGroup);

	auto radioBtnXml = node->getChildByName("task_btn_list");
	auto& children = radioBtnXml->getChildren();
	//添加单选
	for (int i = 0;i < children.size();i++)
	{
		string name = StringUtils::format("task_btn%d",i);
		auto radioBtn = radioBtnXml->getChildByName<RadioButton*>(name);
		//TODO:设置字符串为空，便于之后的任务名链接
		radioBtn->setName("");

		m_pBtnGroup->addRadioButton(radioBtn);
	}
	m_pBtnGroup->addEventListener(SDL_CALLBACK_3(DisplayLayer::selectTaskCallback, this));

	return true;
}

void DisplayLayer::show(Type type)
{
	int tag = 1;
	Size visibleSize = Director::getInstance()->getVisibleSize();
	//动画显示选择界面
	MoveBy*move = MoveBy::create(0.5f, Point(0, visibleSize.height));
	EaseExponentialOut*action = EaseExponentialOut::create(move);
	action->setTag(tag);
	//停止原先动作
	this->stopActionByTag(tag);
	this->runAction(action);
	//上一次和这一次的显示类型不同
	if (m_type != type)
	{
		m_type = type;
		m_nCurPage = 0;
		//更新页数
		this->updatePageIndex();
	}
	//进行更新
	auto& list = this->getContent();
	this->updateRadioButtons(m_pBtnGroup, list);

	m_bOpened = true;
}

void DisplayLayer::hide()
{
	int tag = 1;
	Size visibleSize = Director::getInstance()->getVisibleSize();
	//动画显示选择界面
	MoveBy*move = MoveBy::create(0.5f, Point(0, -visibleSize.height));
	EaseExponentialIn* action = EaseExponentialIn::create(move);
	action->setTag(tag);

	this->stopActionByTag(tag);
	this->runAction(action);

	m_bOpened = false;
}

void DisplayLayer::updatePageIndex()
{
	auto& list = this->getContent();
	//获取总页数
	int nTotalPage = list.size() / ITEM_NUMBER_PRE_PAGE;
	nTotalPage += (list.size() % ITEM_NUMBER_PRE_PAGE == 0 ? 0 : 1);
	//超过页面
	if (m_nCurPage >= nTotalPage)
	{
		m_nCurPage = 0;
	}
	else if (m_nCurPage < 0)
	{
		m_nCurPage = nTotalPage - 1;
	}
	//更新label
	auto text = StringUtils::format("%d/%d", m_nCurPage + 1, nTotalPage);

	m_pPageLabel->setString(text);
}

void DisplayLayer::closeCallback(Object* sender)
{
	GameScene::getInstance()->closeQuest();
}

void DisplayLayer::turnPage(Object* sender, bool bNext)
{
	m_nCurPage = bNext ? m_nCurPage + 1 : m_nCurPage - 1;
	//更新页面索引
	this->updatePageIndex();
	auto& list = this->getContent();

	this->updateRadioButtons(m_pBtnGroup, list);
}

void DisplayLayer::selectTaskCallback(RadioButton* radioBtn,int,RadioButtonGroup::EventType)
{
	//获取选中的
	auto taskName = radioBtn->getName();

	auto title = this->getTitle(taskName);
	auto desc = this->getDescrition(taskName);
	auto step = this->getStep(taskName);
	auto reward = this->getReward(taskName);
	
	m_pTitleLabel->setString(title);
	m_pDescLabel->setString(desc);
	m_pStepLabel->setString(step);

	m_pRewardText->over();
	m_pRewardText->wind(reward, false);
}

void DisplayLayer::updateRadioButton(RadioButton* radioBtn,const string& taskName)
{
	bool ret = !taskName.empty();

	radioBtn->setName(taskName);
	radioBtn->setVisible(ret);
	radioBtn->setTouchEnabled(ret);

	if (!ret)
		return;

	auto title = this->getTitle(taskName);
	auto titleLabel = radioBtn->getChildByName<LabelBMFont*>("title");
	titleLabel->setString(title);
}

void DisplayLayer::updateRadioButtons(RadioButtonGroup* group, vector<Value>& vec)
{
	//是否应该更新
	auto selectedIndex = group->getSelectedIndex();
	RadioButton* selectedBtn = nullptr;
	bool bShouldUpdate = true;
	string selectedItem;

	if (selectedIndex == -1)
		selectedIndex = 0;
	else
		selectedBtn = group->getRadioButtonByIndex(selectedIndex);

	auto& list = group->getRadioButtonList();

	for (int i = list.size() - 1;i >= 0 ;i--)
	{
		auto radioBtn = group->getRadioButtonByIndex(i);

		string item;
		
		if (i + m_nCurPage * ITEM_NUMBER_PRE_PAGE < (int)vec.size())
		{
			item = vec.at(i + m_nCurPage * ITEM_NUMBER_PRE_PAGE).asString();
		}
		//更新对应的单选按钮
		this->updateRadioButton(radioBtn, item);
		//按钮是选中项 或者应该更新还没更新
		if (selectedIndex >= i && bShouldUpdate)
		{
			//当前是选中项，先取消选中
			if (selectedBtn == radioBtn)
				group->unselectedButton();
			//重新设置选中
			if (!item.empty())
			{
				bShouldUpdate = false;
				group->setSelectedButton(radioBtn);
			}
		}
	}
}

vector<Value>& DisplayLayer::getContent()
{
	switch (m_type)
	{
	case DisplayLayer::Type::Quest:
		return DynamicData::getInstance()->getAcceptedTaskList();
		break;
	case DisplayLayer::Type::Forge:
		break;
	default:
		break;
	}
	return DynamicData::getInstance()->getAcceptedTaskList();
}

string DisplayLayer::getTitle(const string& taskName)
{
	auto gameScene = GameScene::getInstance();
	gameScene->getLuaGlobal(taskName.c_str());
	gameScene->getLuaField(-1, "getTitle");
	//放入参数
	gameScene->getLuaGlobal(taskName.c_str());
	//执行函数
	gameScene->executeFunction(1, 1);

	string result = gameScene->checkLuaString(-1);

	gameScene->pop(2);

	return result;
}

string DisplayLayer::getDescrition(const string& taskName)
{
	auto gameScene = GameScene::getInstance();
	gameScene->getLuaGlobal(taskName.c_str());
	gameScene->getLuaField(-1, "getDescription");
	//放入参数
	gameScene->getLuaGlobal(taskName.c_str());
	//执行函数
	gameScene->executeFunction(1, 1);

	string result = gameScene->checkLuaString(-1);

	gameScene->pop(2);

	return result;
}

string DisplayLayer::getStep(const string& taskName)
{
	auto gameScene = GameScene::getInstance();
	gameScene->getLuaGlobal(taskName.c_str());
	gameScene->getLuaField(-1, "getStep");
	//放入参数
	gameScene->getLuaGlobal(taskName.c_str());
	//执行函数
	gameScene->executeFunction(1, 1);

	string result = gameScene->checkLuaString(-1);

	gameScene->pop(2);

	return result;
}

string DisplayLayer::getReward(const string& taskName)
{
	auto gameScene = GameScene::getInstance();
	gameScene->getLuaGlobal(taskName.c_str());
	gameScene->getLuaField(-1, "getReward");
	//放入参数
	gameScene->getLuaGlobal(taskName.c_str());
	//执行函数
	gameScene->executeFunction(1, 1);

	string result = gameScene->checkLuaString(-1);

	gameScene->pop(2);

	return result;
}