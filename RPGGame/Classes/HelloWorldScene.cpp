#include "HelloWorldScene.h"
#include "layer/Text.h"

HelloWorld::HelloWorld()
	:m_pText(nullptr)
{
}

HelloWorld::~HelloWorld()
{
}

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    Director::getInstance()->getRenderer()->setDrawColor(Color4B(0,255,255));

	m_pText = Text::create("fonts/1.fnt", 100, Color3B());
	m_pText->setPosition(300.f, 300.f);
	this->addChild(m_pText);

	m_pText->wind("12345677684327940327043", 0.1f);

	this->scheduleUpdate();

    return true;
}

void HelloWorld::update(float dt)
{
	m_pText->windRun(dt);
}

void HelloWorld::menuCloseCallback(Object* pSender)
{
}

bool HelloWorld::onTouchBegan(Touch* touch, SDL_Event* event)
{
	return true;
}

