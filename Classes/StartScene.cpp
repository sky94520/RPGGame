#include "StartScene.h"
#include "StaticData.h"
#include "GameScene.h"
#include "LoadingLayer.h"

StartScene::StartScene()
	:m_nBGIndex(0)
	,m_nBGOpacity(0)
	,m_nAdding(1)
	,m_pLoadBtn(nullptr)
	,m_pAboutBtn(nullptr)
	,m_pExitBtn(nullptr)
	,m_pSaveDataXml(nullptr)
	,m_pMusicBtn(nullptr)
	,m_pChunkBtn(nullptr)
{
	memset(m_pBGArr, 0, sizeof(m_pBGArr));
}

StartScene::~StartScene()
{
}

bool StartScene::init()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	//加载背景
	for (int i = 0; i < 3; i++)
	{
		auto filename = StringUtils::format("img/bg/T_bg%d.png", i + 1);

		m_pBGArr[i] = Sprite::create(filename);
		m_pBGArr[i]->setPosition(visibleSize.width / 2, visibleSize.height / 2);
		m_pBGArr[i]->getTexture()->setBlendMode(SDL_BLENDMODE_BLEND);

		this->addChild(m_pBGArr[i]);
	}
	//随机设置当前背景
	m_nBGIndex = rand() % 3 - 1;
	//加载图片
	Director::getInstance()->getSpriteFrameCache()->addSpriteFramesWithFile("sprite/start_ui.xml");
	//加载界面文件
	auto node = ui::UIWidgetManager::getInstance()->createWidgetsWithXml("scene/start_scene.xml");
	this->addChild(node, 4);
	//按钮
	m_pLoadBtn = node->getChildByName<Button*>("load_btn");
	m_pAboutBtn = node->getChildByName<Button*>("about_btn");
	m_pExitBtn = node->getChildByName<Button*>("exit_btn");
	//音乐 音效
	m_pMusicBtn = node->getChildByName<CheckBox*>("music_btn");
	m_pChunkBtn = node->getChildByName<CheckBox*>("chunk_btn");
	//绑定回调函数
	m_pLoadBtn->addClickEventListener(SDL_CALLBACK_1(StartScene::loadCallback, this));
	m_pAboutBtn->addClickEventListener(SDL_CALLBACK_1(StartScene::aboutCallback, this));
	m_pExitBtn->addClickEventListener(SDL_CALLBACK_1(StartScene::exitCallback, this));

	m_pMusicBtn->addEventListener(SDL_CALLBACK_2(StartScene::changeMusic, this));
	m_pChunkBtn->addEventListener(SDL_CALLBACK_2(StartScene::changeSound, this));
	//存档层
	m_pSaveDataXml = node->getChildByName("save_data_layer");
	//初始化存档
	this->initializeSaveDataLayer();
	this->updateUI();
	//播放音乐
	SoundManager::getInstance()->playBackgroundMusic(STATIC_DATA_STRING("theme_bgm"), 0);
	this->scheduleUpdate();

	return true;
}

void StartScene::update(float dt)
{
	//设置透明度
	m_nBGOpacity -= 1;
	//切换图片
	if (m_nBGOpacity <= 0)
	{
		m_nBGOpacity = 255;
		//循环
		m_nBGIndex += m_nAdding;

		if (m_nBGIndex >= 2)
		{
			m_nAdding = -1;
		}
		else if (m_nBGIndex <= 0)
		{
			m_nAdding = 1;
		}
		//设置背景优先级
		int index = m_nBGIndex, adding = m_nAdding;

		for (int i = 0; i < 3; i++)
		{
			m_pBGArr[index]->setLocalZOrder(3 - i);
			//设置图片透明度
			m_pBGArr[index]->setOpacity(255);
			
			index = (m_nBGIndex + adding);
			if (index >= 2)
				adding = -1;
			else if (index <= 0)
				adding = 1;
		}
	}
	m_pBGArr[m_nBGIndex]->setOpacity(m_nBGOpacity);
}

void StartScene::initializeSaveDataLayer()
{
	//绑定关闭按钮
	auto closeBtn = m_pSaveDataXml->getChildByName<Button*>("close_btn");
	closeBtn->addClickEventListener(SDL_CALLBACK_1(StartScene::closeSaveDataLayer, this));
	
	auto& valueMap = DynamicData::getInstance()->getDescValueMap();
	//存档按钮
	for (int i = 0; i < 3;i++)
	{
		//绑定存档按钮
		auto name = StringUtils::format("save%d", i + 1);
		auto btn = m_pSaveDataXml->getChildByName<Button*>(name);

		btn->addClickEventListener(SDL_CALLBACK_1(StartScene::startGame, this, i));
		//设置描述
		auto descLabel = m_pSaveDataXml->getChildByName<LabelAtlas*>(StringUtils::format("game_time%d", i + 1));

		auto it = valueMap.find(name);

		if (it == valueMap.end())
		{
			descLabel->setVisible(false);
			continue;
		}
		auto gameTimeBg = m_pSaveDataXml->getChildByName<Sprite*>(StringUtils::format("game_time_bg%d", i + 1));
		gameTimeBg->setSpriteFrame("game_time_text.png");
		//设置label 以后可以添加 角色：名称 等级
		auto saveMap = it->second.asValueMap();
		auto game_time = saveMap["game_time"].asInt();
		//优化显示
		int minute = game_time / 60;
		int hour = minute / 60;
		int second = game_time - minute * 60;
		minute = minute - hour * 60;

		descLabel->setString(StringUtils::format(":%02d:%02d:%02d", hour, minute, second));
	}
}

void StartScene::updateUI()
{
	bool bPlayingMusic = SoundManager::getInstance()->isPlayingMusic();
	bool bPlayingChunk = SoundManager::getInstance()->isPlayingChunk();
	//这个是因为使用的Checkbox，所以得取反
	m_pMusicBtn->setSelected(!bPlayingMusic);
	m_pChunkBtn->setSelected(!bPlayingChunk);
}

void StartScene::loadCallback(Object* sender)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	//显示选择界面
	MoveBy*move = MoveBy::create(0.5f,Point(0, visibleSize.height));
	EaseExponentialOut*action = EaseExponentialOut::create(move);
	
	m_pSaveDataXml->runAction(action);
	//按钮不可用
	this->setTouchEnable(false);
}

void StartScene::aboutCallback(Object* sender)
{
}

void StartScene::exitCallback(Object* sender)
{
	Director::getInstance()->setRunning(false);
}

void StartScene::changeMusic(Object*,ui::CheckBox::EventType type)
{
	auto soundManager = SoundManager::getInstance();
	bool var = soundManager->isPlayingMusic();

	soundManager->setPlayingMusic(!var);
}

void StartScene::changeSound(Object*,ui::CheckBox::EventType type)
{
	auto soundManager = SoundManager::getInstance();
	bool var = soundManager->isPlayingChunk();

	soundManager->setPlayingChunk(!var);
}

void StartScene::setTouchEnable(bool enable)
{
	m_pLoadBtn->setTouchEnabled(enable);
	m_pAboutBtn->setTouchEnabled(enable);
	m_pExitBtn->setTouchEnabled(enable);
}

void StartScene::closeSaveDataLayer(Object* sender)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	//显示选择界面
	MoveBy*move = MoveBy::create(0.5f,Point(0, -visibleSize.height));
	EaseExponentialIn*action = EaseExponentialIn::create(move);
	
	m_pSaveDataXml->runAction(action);
	//设置按钮可用
	this->setTouchEnable(true);
}

void StartScene::startGame(Object* sender, int index)
{
	//播放音效
	SoundManager::getInstance()->playEffect(STATIC_DATA_STRING("load_data_se"), 0);

	Director::getInstance()->replaceScene(LoadingLayer::createScene(index + 1));
}
