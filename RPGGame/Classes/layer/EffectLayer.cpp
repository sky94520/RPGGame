#include "EffectLayer.h"
#include "SpritePool.h"

const int EffectLayer::ANIMATION_TAG = 1;

EffectLayer::EffectLayer()
	:m_pLayerColor(nullptr)
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
