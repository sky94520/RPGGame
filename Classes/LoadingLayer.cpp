#include "LoadingLayer.h"
#include "GameScene.h"
#include "StaticData.h"
#include "DynamicData.h"

LoadingLayer::LoadingLayer()
	:m_pProgress1(nullptr)
	,m_pProgress2(nullptr)
	,m_nCurIndex(0)
	,m_nSize(0)
	,m_pLoadingLabel(nullptr)
	,m_nSaveDataIndex(0)
{
}

LoadingLayer::~LoadingLayer()
{
}

Scene* LoadingLayer::createScene(int idx)
{
	auto scene = Scene::create();
	auto layer = LoadingLayer::create(idx);

	scene->addChild(layer);

	return scene;
}

LoadingLayer* LoadingLayer::create(int idx)
{
	auto layer = new LoadingLayer();

	if (layer && layer->init(idx))
		layer->autorelease();
	else
		SDL_SAFE_DELETE(layer);

	return layer;
}

bool LoadingLayer::init(int idx)
{
	m_nSaveDataIndex = idx;

	Size visibleSize = Director::getInstance()->getVisibleSize();
	//添加背景
	Sprite* background = Sprite::createWithSpriteFrameName("loading_bg.jpg");
	background->setAnchorPoint(Point::ZERO);
	this->addChild(background);
	//加载loading背景
	Sprite* loadingSprite = Sprite::createWithSpriteFrameName("loading_3.png");
	loadingSprite->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(loadingSprite);
	//创建进度条
	m_pProgress1 = ProgressTimer::create(Sprite::createWithSpriteFrameName("loading_2.png"));
	m_pProgress1->setType(ProgressTimer::Type::BAR);
	m_pProgress1->setBarChangeRate(Point(1.f, 0.f));
	m_pProgress1->setMidpoint(Point(0.f, 0.5f));
	m_pProgress1->setPosition(visibleSize.width / 2, visibleSize.height / 2);

	this->addChild(m_pProgress1);
	//创建度
	Size size = loadingSprite->getContentSize();

	Sprite* barSprite = Sprite::createWithSpriteFrameName("loading_4.png");
	barSprite->setPosition(visibleSize.width / 2, visibleSize.height / 2 + size.height);
	this->addChild(barSprite);
	//创建进度条
	auto pos2 = Point(visibleSize.width / 2, visibleSize.height / 2 + size.height);

	m_pProgress2 = ProgressTimer::create(Sprite::createWithSpriteFrameName("loading_5.png"));
	m_pProgress2->setType(ProgressTimer::Type::BAR);
	m_pProgress2->setBarChangeRate(Point(1.f, 0.f));
	m_pProgress2->setMidpoint(Point(0.f, 0.5f));
	m_pProgress2->setPosition(pos2);
	this->addChild(m_pProgress2);
	//创建LabelBMFont
	m_pLoadingLabel = LabelBMFont::create("1", "fonts/1.fnt",500);
	m_pLoadingLabel->setPosition(pos2);
	this->addChild(m_pLoadingLabel);
	//解析
	auto& valueMap = StaticData::getInstance()->getValueForKey("resources")->asValueMap();

	for(auto& dict : valueMap)
	{
		auto key = dict.first;
		auto& vec = dict.second.asValueVector();

		m_nSize += vec.size();

		for (auto value:vec)
		{
			auto filename = value.asString();
			
			if (key == "png")
			{
				m_images.push_back(filename);
			}
			else if (key == Music::TYPE)
			{
				m_musics.push_back(filename);
			}
			else if (key == Chunk::TYPE)
			{
				m_chunks.push_back(filename);
			}
		}
	}
	//先加载第一个图片
	//加载的过程为 图片=> 音乐 => 音效
	//this->loadImageCallback(nullptr);

	auto callback = [this](float)
	{
		this->loadImageCallback(nullptr);
	};

	this->scheduleOnce(callback,"loading");

	return true;
}
void LoadingLayer::loadImageCallback(Texture* texture)
{
	//当前指向不为空，不进行更新
	if(!m_images.empty())
		this->loadResAsyncCallback(m_images.front());
	//图片加载完成，进行音乐的加载
	if (m_images.empty())
	{
		this->loadMusicCallback(nullptr,"");
	}
	else
	{
		auto filename = m_images.front();
		//进行异步加载
		Director::getInstance()->getTextureCache()->addImageAsync(filename, SDL_CALLBACK_1(LoadingLayer::loadImageCallback, this));
		m_images.pop_front();
	}
}

void LoadingLayer::loadMusicCallback(Object*sound,const string&type)
{
	if (!m_musics.empty())
		this->loadResAsyncCallback(m_musics.front());

	if (m_musics.empty())
	{
		this->loadChunkCallback(nullptr,"");
	}
	else
	{
		auto filename = m_musics.front();

		SoundManager::getInstance()->addSoundAsync(filename,Music::TYPE
			,SDL_CALLBACK_2(LoadingLayer::loadMusicCallback,this));
		m_musics.pop_front();
	}
}

void LoadingLayer::loadChunkCallback(Object* sound,const string& type)
{
	if (!m_chunks.empty())
		this->loadResAsyncCallback(m_chunks.front());
	//资源全部加载完成,进行页面的跳转
	if (m_chunks.empty())
	{
		DynamicData::getInstance()->initializeSaveData(m_nSaveDataIndex);
		Director::getInstance()->replaceScene(GameScene::getInstance());
	}
	else
	{
		auto filename = m_chunks.front();

		SoundManager::getInstance()->addSoundAsync(filename,Chunk::TYPE
			,SDL_CALLBACK_2(LoadingLayer::loadChunkCallback,this));
		m_chunks.pop_front();
	}
}

void LoadingLayer::loadResAsyncCallback(const string& filename)
{
	float percentage = 100.f * (m_nCurIndex + 1) / m_nSize;

	m_pProgress1->setPercentage(percentage);
	m_pProgress2->setPercentage(percentage);

	m_nCurIndex++;

	auto text = StringUtils::format("load:%s", filename.c_str());
	m_pLoadingLabel->setString(text);
}