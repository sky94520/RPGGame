#include "MessageLayer.h"
#include "Text.h"
#include "StaticData.h"
#include "GameScene.h"
#include "ScriptLayer.h"

 MessageLayer:: MessageLayer()
	 :m_bShowingText(false)
	 ,m_bClickable(true)
	 ,m_pText(nullptr)
	 ,m_curTextPosition(TextPosition::Bottom)
	 ,m_pTipNode(nullptr)
	 ,m_pMenu(nullptr)
 {
 }

MessageLayer::~MessageLayer()
{
}

bool MessageLayer::init()
{
	//读取xml
	auto msgNode = ui::UIWidgetManager::getInstance()->createWidgetsWithXml("scene/message.xml");
	//隐藏 设置名称
	msgNode->setVisible(false);
	msgNode->setName("message");

	this->addChild(msgNode);
	//读取xml
	m_pTipNode = ui::UIWidgetManager::getInstance()->createWidgetsWithXml("scene/tip.xml");
	m_pTipNode->setOpacity(0);
	m_pTipNode->setCascadeOpacityEnabled(true);
	
	this->addChild(m_pTipNode);
	//创建文本
	m_pText = Text::create("fonts/1.fnt", 430, Color3B(0, 0, 0));
	this->addChild(m_pText);
	
	m_pMenu = Menu::create();
	this->addChild(m_pMenu);

	return true;
}

float MessageLayer::showText(const string& name, const string& text, TextPosition textPos, bool run, const string& faceFilename, int index, FacePosition facePos)
{
	auto msgNode = this->getChildByName("message");
	auto bgSize = msgNode->getContentSize();
	//获取位置
	m_curTextPosition = textPos;

	Point msgPos = this->getPosition(textPos, bgSize);
	msgNode->setPosition(msgPos);
	//显示
	msgNode->setVisible(true);
	//获取对应的节点
	auto pFaceSprite = msgNode->getChildByName<Sprite*>("face");
	auto pNameLabel = msgNode->getChildByName<LabelBMFont*>("name");
	//更改控件内容
	pNameLabel->setString(name);
	m_pText->over();
	//更改文本,并返回持续时间
	auto duration = m_pText->wind(text, run);
	//设置文本位置
	m_pText->setPosition(msgPos);
	m_pText->setOpacity(255);
	//表示正在显示文本
	this->setShowingText(true);
	//修改立绘
	pFaceSprite->setVisible(!faceFilename.empty());
	//立绘路径不存在，隐藏立绘并直接退出
	if (faceFilename.empty())
	{
		return duration;
	}
	
	auto spriteFrame = StaticData::getInstance()->getFaceSpriteFrame(faceFilename,index);

	pFaceSprite->setSpriteFrame(spriteFrame);
	//设置立绘位置
	auto faceSize = pFaceSprite->getContentSize();
	Point facePosition = Point(faceSize.width/2, bgSize.height - faceSize.height/2);

	if (facePos == FacePosition::Right)
	{
		facePosition.x = bgSize.width - facePosition.x;
	}
	pFaceSprite->setPosition(facePosition);

	return duration;
}

float MessageLayer::showText(const string& name, const string& text, TextPosition textPos, bool run)
{
	return this->showText(name, text,textPos, run,"",0, FacePosition::Left);
}

void MessageLayer::hideText()
{
	auto msgNode = this->getChildByName("message");
	//显示
	msgNode->setVisible(false);
	//隐藏文本
	m_pText->over();
	this->setShowingText(false);
}

void MessageLayer::showTip(const string&text,TextPosition posType, float duration)
{
	m_pTipNode->setOpacity(255);
	auto label = m_pTipNode->getChildByName<LabelBMFont*>("text_label");
	//设置位置
	auto bgSize = m_pTipNode->getContentSize();
	auto pos = this->getPosition(posType,bgSize);
	m_pTipNode->setPosition(pos);
	//设置文本
	label->setString(text);
	//设置动作
	DelayTime* delay = DelayTime::create(duration / 3 * 2);
	FadeOut* fade = FadeOut::create(duration / 3);

	auto seq = Sequence::create(delay, fade, nullptr);
	seq->setTag(1);
	//运行动作
	m_pTipNode->stopActionByTag(1);
	m_pTipNode->runAction(seq);
}

Point MessageLayer::getWaitClickPosition()
{
	auto msgNode = this->getChildByName("message");
	auto bgSize = msgNode->getContentSize();
	auto pos = this->getPosition(m_curTextPosition, bgSize);

	pos.y += bgSize.height / 2;

	return pos;
}

void MessageLayer::addButton(const string& text)
{
	//创建新按钮
	LabelBMFont* label = LabelBMFont::create(text, "fonts/1.fnt");
	auto size = label->getContentSize();
	label->setPosition(size.width / 2, size.height / 2);
	label->setAnchorPoint(Point(0.5f,0.5f));

	Scale9Sprite* normalSprite = Scale9Sprite::create("bt6_1.png");
	normalSprite->setPreferredSize(size);

	Scale9Sprite* selectedSprite = Scale9Sprite::create("bt6_2.png");
	selectedSprite->setPreferredSize(size);

	MenuItemSprite* item = MenuItemSprite::create(normalSprite, selectedSprite);

	item->addChild(label, 6);
	item->setName(text);
	item->setCallback(SDL_CALLBACK_1(MessageLayer::clickBtn, this));

	m_pMenu->addChild(item);
}

void MessageLayer::alignItemsVertically()
{
	m_pMenu->alignItemsVertically();

	Size visibleSize = Director::getInstance()->getVisibleSize();
	
	auto& children = m_pMenu->getChildren();
	auto n = children.size();
	auto child = children.front();
	//获取第一个item的位置
	auto size = child->getContentSize();
	//设置位置
	m_pMenu->setPosition(visibleSize.width / 2, visibleSize.height / 2 - n * size.height / 2);
}

void MessageLayer::windRun(float dt)
{
	m_pText->windRun(dt);
}

void MessageLayer::windOver()
{
	m_pText->windOver();
}

bool MessageLayer::isWindOver() const
{
	return m_pText->isWindOver();
}

Point MessageLayer::getPosition(TextPosition textPos, const Size& bgSize)
{
	Point msgPos;
	Size visibleSize = Director::getInstance()->getVisibleSize();

	msgPos.x = visibleSize.width / 2;

	switch(textPos)
	{
	case TextPosition::Bottom:
		msgPos.y = visibleSize.height - bgSize.height/2;
		break;
	case TextPosition::Middle:
		msgPos.y = visibleSize.height/2;
		break;
	case TextPosition::Top:
		msgPos.y = bgSize.height/2;
		break;
	}
	return msgPos;
}

void MessageLayer::clickBtn(Object* sender)
{
	//获取点击的按钮
	auto node = static_cast<Node*>(sender);

	auto text = node->getName();
	//移除所有按钮
	m_pMenu->removeAllChildren();
	//放入栈中
	auto gameScene = GameScene::getInstance();

	gameScene->pushString(text.c_str());
	gameScene->resumeLuaScript(WaitType::Button, 1);
}