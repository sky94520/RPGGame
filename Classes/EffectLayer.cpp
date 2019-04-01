#include "EffectLayer.h"
#include "GameScene.h"
#include "StaticData.h"
#include "FlowWord.h"
#include "Fighter.h"
#include "RotateSpeedSprite.h"

const int EffectLayer::ANIMATION_TAG = 1;

EffectLayer::EffectLayer()
	:m_pClickPoint(nullptr)
	,m_pWaitClick(nullptr)
	,m_pLayerColor(nullptr)
	,m_pFlowWord(nullptr)
{
}

EffectLayer::~EffectLayer()
{
	for (auto it = m_spritePool.begin(); it != m_spritePool.end();)
	{
		auto sprite = *it;
		it = m_spritePool.erase(it);

		SDL_SAFE_RELEASE(sprite);
	}
}

bool EffectLayer::init()
{
	m_pLayerColor = LayerColor::create(Color4B(0,0,0,255));
	m_pLayerColor->setVisible(false);
	this->addChild(m_pLayerColor);
	//创建FlowWord
	m_pFlowWord = FlowWord::create();
	this->addChild(m_pFlowWord);
	//添加动画监听器
	auto listener = EventListenerCustom::create(AnimationFrameDisplayedNotification, SDL_CALLBACK_1(EffectLayer::frameEvent,this));

	_eventDispatcher->addEventListener(listener,this);

	return true;
}

void EffectLayer::showClickAnimation(const Point& position, Layer* layer)
{
	string animationName = "ClickPath";
	//当前为空 或者正在播放且名称不为ClickPath 表示被别人占用，另选
	if (m_pClickPoint == nullptr 
		|| (m_pClickPoint->getParent() != nullptr 
			&& m_pClickPoint->getName() != animationName))
	{
		m_pClickPoint = this->popSpriteFromPool();
	}
	else
	{
		m_pClickPoint->stopActionByTag(ANIMATION_TAG);
		m_pClickPoint->setRotation(0.f);

		if (m_pClickPoint->getParent() == nullptr)
			m_pClickPoint = this->popSpriteFromPool();
	}
	m_pClickPoint->setLocalZOrder(GameScene::CHARACTER_LOCAL_Z_ORDER + 10);
	
	this->showAnimationEffect(m_pClickPoint, animationName, position, layer);
}

void EffectLayer::showWaitClick(const Point& position)
{
	string animationName = "WaitClick";

	m_pWaitClick = this->popSpriteFromPool();
	m_pWaitClick->setAnchorPoint(Point(0.5f, 1.f));

	this->showAnimationEffect(m_pWaitClick, animationName, position);
}

void EffectLayer::hideWaitClick()
{
	if (m_pWaitClick != nullptr)
	{
		m_pWaitClick->stopActionByTag(ANIMATION_TAG);
		this->pushSpriteToPool(m_pWaitClick);
	}
	m_pWaitClick = nullptr;
}

float EffectLayer::showBalloonIcon(const string& balloonName, const Point& pos, Layer* layer)
{
	auto sprite = this->popSpriteFromPool();
	sprite->setName(balloonName);
	sprite->setAnchorPoint(Point(0.5f, 1.f));
	sprite->setGlobalZOrder(10);

	return this->showAnimationEffect(sprite, balloonName, pos, layer);
}

float EffectLayer::showBullet(const Point& fromPos, const Point& toPos, const Size& size, const string& bulletName)
{
	auto sprite = this->popSpriteFromPool();
	sprite->setName(bulletName);
	sprite->setAnchorPoint(Point(0.5f, 0.5f));
	sprite->setLocalZOrder(GameScene::CHARACTER_LOCAL_Z_ORDER + 10);

	auto delta = fromPos - toPos;
	Point pos = fromPos;
	//从右向左 调整箭的起始位置
	if (delta.x > 0.f)
	{
		sprite->setFlipX(true);

		pos.x -= size.width / 2;
	}
	else
	{
		pos.x += size.width / 2;
	}
	//添加精灵
	this->addChild(sprite);
	sprite->setName(bulletName);
	sprite->setPosition(pos);
	sprite->setVisible(true);
	//重新设置delta
	delta = pos - toPos;
	//获取动作
	auto length = delta.length();
	auto duration = length / 650.f;

	ActionInterval* action = nullptr;
	MoveTo* move = MoveTo::create(duration, toPos);
	CallFunc* moveEnd = CallFunc::create([this]()
	{
		//播放打击音效
		SoundManager::getInstance()->playEffect(STATIC_DATA_STRING("arrow_hit_se"), 0);
	});
	action = Sequence::createWithTwoActions(move, moveEnd);
	//是否存在子弹碰撞动画
	auto hitName = StringUtils::format("%s_hit", bulletName.c_str());
	Animation* hitAnimation = StaticData::getInstance()->getAnimation(hitName);
	//播放hit特效
	if (hitAnimation != nullptr)
	{
		auto animate = Animate::create(hitAnimation);
		action = Sequence::createWithTwoActions(action, animate);
	}
	//检测是否存在打击动画
	CallFunc* end = CallFunc::create([this, sprite, bulletName]()
	{
		this->pushSpriteToPool(sprite);
	});
	action = Sequence::createWithTwoActions(action, end);
	//运行动作
	sprite->runAction(RotateSpeedSprite::create(action));
	//动画
	auto animation = AnimationCache::getInstance()->getAnimation(bulletName);
	sprite->setSpriteFrame(animation->getFrames().front()->getSpriteFrame());
	//创建动画并运行动画
	sprite->runAction(Animate::create(animation));

	return duration;
}

float EffectLayer::showAnimationEffect(const string& animationName, const Point& position, bool flipX, Layer* layer)
{
	Sprite* sprite = this->popSpriteFromPool();
	sprite->setName(animationName);
	sprite->setFlipX(flipX);

	Animation* animation = nullptr;
	//检测是否存在对应的脚本table
	auto gameScene = GameScene::getInstance();
	int type = gameScene->getLuaGlobal(animationName.c_str());
	Size visibleSize = Director::getInstance()->getVisibleSize();
	//可能会重新设置位置
	Point target = position;
	//不存在该动画对应的脚本table
	if (type != LUA_TTABLE)
	{
		gameScene->pop(1);

		animation = AnimationCache::getInstance()->getAnimation(animationName);
	}
	else
	{
		animation = StaticData::getInstance()->getAnimation(animationName);

		gameScene->getLuaField(-1, "posType");
		auto posType = static_cast<AnimPosType>(gameScene->checkLuaInt(-1));

		gameScene->getLuaField(-2, "localZOrder");
		int localZOrder = gameScene->checkLuaInt(-1);

		gameScene->pop(3);
		//对位置和顺序进行设置

		switch (posType)
		{
		case AnimPosType::Head:
			sprite->setAnchorPoint(Point(0.5f, 0.f));
			break;
		case AnimPosType::Body:
			sprite->setAnchorPoint(Point(0.5f , 0.5f));
			break;
		case AnimPosType::Foot:
			sprite->setAnchorPoint(Point(0.5f, 0.7f));
			break;
		case AnimPosType::Scene:
			target = Point(visibleSize.width / 2, visibleSize.height / 2);
			sprite->setAnchorPoint(Point(0.5f, 0.5f));
			break;
		case AnimPosType::Weapon:
			sprite->setAnchorPoint(Point(0.5f, 0.5f));

			if (flipX)
			{
				target.x += 22.f;
				target.y -= 9.f;
			}
			else
			{
				target.x -= 22.f;
				target.y -= 9.f;
			}
			break;
		default:
			break;
		}
		sprite->setLocalZOrder(localZOrder);
	}

	return this->showAnimationEffect(sprite, animationName, target, layer);
}

void EffectLayer::fadeInScreen(float duration)
{
	auto sprite = m_pLayerColor->getInnerSprite();

	m_pLayerColor->setVisible(true);
	sprite->setOpacity(0);

	FadeIn* fade = FadeIn::create(duration);
	CallFunc* end = CallFunc::create([this]()
	{
		this->m_pLayerColor->setVisible(false);
	});
	auto seq = Sequence::createWithTwoActions(fade,end);

	sprite->runAction(fade);
}

void EffectLayer::fadeOutScreen(float duration)
{
	auto sprite = m_pLayerColor->getInnerSprite();

	m_pLayerColor->setVisible(true);
	sprite->setOpacity(255);

	FadeOut* fade = FadeOut::create(duration);
	CallFunc* end = CallFunc::create([this]()
	{
		this->m_pLayerColor->setVisible(false);
	});
	auto seq = Sequence::createWithTwoActions(fade,end);

	sprite->runAction(fade);
}

void EffectLayer::showWord(int number, Point pos, HurtType hurtType)
{
	FlowWord::ColorType colorType = FlowWord::ColorType::White;
	string text;

	if (number == 0)
	{
		text = "miss";
	}
	else if (number > 0)
	{
		colorType = FlowWord::ColorType::Green;

		text = StringUtils::format("%d", number);
	}
	//TODO:物理攻击和暴击物理攻击数字
	else
	{
		if (hurtType == HurtType::Attack)
			colorType = FlowWord::ColorType::White;
		else if (hurtType == HurtType::CriticalAttack)
			colorType = FlowWord::ColorType::Blue;

		text = StringUtils::format("%d", abs(number));
	}
	m_pFlowWord->showWord(text, pos, colorType);
}

Sprite* EffectLayer::popSpriteFromPool()
{
	Sprite* sprite = nullptr;

	if (m_spritePool.empty())
	{
		sprite = Sprite::create();
	}
	else
	{
		sprite = m_spritePool.back();
		sprite->autorelease();

		m_spritePool.pop_back();
	}
	return sprite;
}

void EffectLayer::pushSpriteToPool(Sprite* sprite)
{
	SDL_SAFE_RETAIN(sprite);
	sprite->setLocalZOrder(0);
	sprite->setGlobalZOrder(0);
	sprite->setFlipX(false);
	sprite->setRotation(0.f);
	sprite->removeFromParent();
	sprite->setAnchorPoint(Point(0.5f, 0.5f));

	m_spritePool.push_back(sprite);
}

float EffectLayer::showAnimationEffect(Sprite* sprite, const string& animationName, const Point& position, Layer* layer)
{
	auto animation = AnimationCache::getInstance()->getAnimation(animationName);
	sprite->setName(animationName);

	return this->showAnimationEffect(sprite, animation, position, layer);
}

float EffectLayer::showAnimationEffect(Sprite* sprite, Animation* animation, const Point& position, Layer* layer)
{
	if (animation == nullptr)
		return 0.f;
	//添加精灵
	if (layer != nullptr)
		layer->addChild(sprite);
	else
		this->addChild(sprite);

	sprite->setSpriteFrame(animation->getFrames().front()->getSpriteFrame());
	sprite->setPosition(position);
	sprite->setVisible(true);
	//创建动画
	auto animate = Animate::create(animation);
	CallFunc* end = CallFunc::create([this, sprite]()
	{
		this->pushSpriteToPool(sprite);
	});
	auto seq = Sequence::createWithTwoActions(animate, end);
	seq->setTag(ANIMATION_TAG);
	//运行动画
	sprite->runAction(seq);

	return animation->getDuration();
}

void EffectLayer::frameEvent(EventCustom*eventCustom)
{
	auto userData = static_cast<AnimationFrame::DisplayedEventInfo*>(eventCustom->getUserData());
	const auto valueMap = userData->userInfo;
	//动画名，对应一个脚本名
	auto animationName = valueMap->at("animationName").asString();
	auto index = valueMap->at("index").asInt();
	//调用对应的脚本事件
	auto gameScene = GameScene::getInstance();
	//获取函数
	gameScene->getLuaGlobal(animationName.c_str());
	gameScene->getLuaField(-1, "frameEvent");
	//放入参数
	gameScene->getLuaGlobal(animationName.c_str());
	gameScene->pushInt(index);
	//执行函数
	gameScene->executeFunction(2, 0);

	gameScene->pop(1);
}