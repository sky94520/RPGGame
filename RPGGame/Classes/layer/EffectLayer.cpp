#include "EffectLayer.h"
#include "SpritePool.h"
#include "../GameScene.h"
#include "../script/LuaStack.h"
#include "../manager/ScriptManager.h"
#include "../entity/FlowWord.h"
#include "../entity/RotateSpeedSprite.h"

const int EffectLayer::ANIMATION_TAG = 1;

EffectLayer::EffectLayer()
	:m_pLayerColor(nullptr)
	,m_pFlowWord(nullptr)
{
}

EffectLayer::~EffectLayer()
{
}

bool EffectLayer::init()
{
	//加载图片资源
	auto frameCache = Director::getInstance()->getSpriteFrameCache();
	bool ret = frameCache->addSpriteFramesWithFile("sprite/effect1.xml");
	ret |= frameCache->addSpriteFramesWithFile("sprite/effect2.xml");
	if (!ret)
	{
		printf("effect1.xml | effect2.xml load error\n");
		return false;
	}
	//加载资源
	AnimationCache::getInstance()->addAnimationsWithFile("data/effect1.plist");
	AnimationCache::getInstance()->addAnimationsWithFile("data/effect2.plist");

	//创建遮罩层
	m_pLayerColor = LayerColor::create(Color4B());
	m_pLayerColor->setVisible(false);
	this->addChild(m_pLayerColor);
	//创建FlowWord
	m_pFlowWord = FlowWord::create();
	this->addChild(m_pFlowWord);
	//添加动画监听器
	//auto listener = EventListenerCustom::create(AnimationFrameDisplayedNotification, SDL_CALLBACK_1(EffectLayer::frameEvent, this));
	//_eventDispatcher->addEventListener(listener, this);

	return true;
}

void EffectLayer::showClickAnimation(const Point& position, Layer* layer)
{
	string animationName = "ClickPath";
	Sprite* sprite = SpritePool::getInstance()->createObject();
	
	this->showAnimationEffect(sprite, animationName, position, layer);
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

float EffectLayer::showBullet(const Point& fromPos, const Point& toPos, const Size& size, const string& bulletName)
{
	Sprite* sprite = SpritePool::getInstance()->createObject();
	sprite->setName(bulletName);
	sprite->setAnchorPoint(Point(0.5f, 0.5f));
	//sprite->setLocalZOrder(GameScene::CHARACTER_LOCAL_Z_ORDER + 10);

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
	//回收
	CallFunc* end = CallFunc::create([this, sprite, bulletName]()
	{
		SpritePool::getInstance()->freeObject(sprite);
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

float EffectLayer::showAnimationEffect(const string& animationName, const Point& position, bool flipX, Layer* layer)
{
	Sprite* sprite = SpritePool::getInstance()->createObject();
	sprite->setName(animationName);
	sprite->setFlipX(flipX);

	Animation* animation = nullptr;
	//检测是否存在对应的脚本table
	auto gameScene = GameScene::getInstance();
	auto luaStack = gameScene->getScriptManager()->getLuaStack();
	int type = luaStack->getLuaGlobal(animationName.c_str());
	Size visibleSize = Director::getInstance()->getVisibleSize();
	//可能会重新设置位置
	Point target = position;
	//不存在该动画对应的脚本table
	if (type != LUA_TTABLE)
	{
		luaStack->pop(1);
		animation = AnimationCache::getInstance()->getAnimation(animationName);
	}
	else
	{
		animation = StaticData::getInstance()->getAnimation(animationName);

		luaStack->getLuaField(-1, "posType");
		auto posType = static_cast<AnimPosType>(luaStack->checkLuaInt(-1));

		luaStack->getLuaField(-2, "localZOrder");
		int localZOrder = luaStack->checkLuaInt(-1);
		luaStack->pop(3);
		//对位置和顺序进行设置

		switch (posType)
		{
		case AnimPosType::Head:
			sprite->setAnchorPoint(Point(0.5f, 0.f));
			break;
		case AnimPosType::Body:
			sprite->setAnchorPoint(Point(0.5f, 0.5f));
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

float EffectLayer::showBalloonIcon(const string& balloonName, const Point& pos, Layer* layer)
{
	Sprite* sprite = SpritePool::getInstance()->createObject();
	sprite->setName(balloonName);
	sprite->setAnchorPoint(Point(0.5f, 1.f));
	sprite->setGlobalZOrder(10);

	return this->showAnimationEffect(sprite, balloonName, pos, layer);
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
		SDL_SAFE_RETAIN(sprite);
		sprite->removeFromParent();
		SpritePool::getInstance()->freeObject(sprite);
	});
	auto seq = Sequence::createWithTwoActions(animate, end);
	seq->setTag(ANIMATION_TAG);
	//运行动画
	sprite->runAction(seq);

	return animation->getDuration();
}
