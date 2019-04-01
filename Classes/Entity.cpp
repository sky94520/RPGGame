#include "Entity.h"

const int Entity::ANIMATION_TAG = 100;
const int Entity::ACTION_TAG = 101;

Entity::Entity()
	:m_pSprite(nullptr)
	,m_bOpenShade(false)
{
}
Entity::~Entity()
{
}

void Entity::setSprite(Sprite*sprite)
{
	if(m_pSprite)
		m_pSprite->removeFromParent();

	m_pSprite = sprite;
	Size size = this->getContentSize();

	m_pSprite->setPosition(size.width/2,size.height/2);
	this->addChild(m_pSprite);
}

void Entity::bindSprite(Sprite*sprite)
{
	if(m_pSprite)
		m_pSprite->removeFromParent();

	m_pSprite = sprite;
	auto size = m_pSprite->getContentSize();

	this->setContentSize(size);
	m_pSprite->setPosition(size.width/2,size.height/2);
	this->addChild(m_pSprite);
}

Sprite* Entity::bindSpriteWithSpriteFrame(SpriteFrame*spriteFrame)
{
	if(spriteFrame != nullptr)
	{
		Sprite*sprite = Sprite::createWithSpriteFrame(spriteFrame);
		Entity::bindSprite(sprite);

		return sprite;
	}
	return nullptr;
}

Sprite* Entity::bindSpriteWithSpriteFrameName(const string&spriteName)
{
	//获取精灵帧
	auto spriteFrame = Director::getInstance()->getSpriteFrameCache()->getSpriteFrameByName(spriteName);
	
	return this->bindSpriteWithSpriteFrame(spriteFrame);
}

Sprite*Entity::bindSpriteWithAnimate(Animate*animate)
{
	auto animation = animate->getAnimation();
	auto firstFrame = animation->getFrames().front()->getSpriteFrame();	
	auto sprite = this->bindSpriteWithSpriteFrame(firstFrame);
	//运行动画
	sprite->runAction(animate);

	return sprite;
}

Sprite*Entity::getSprite()const
{
	return m_pSprite;
}

void Entity::unbingSprite()
{
	if (m_pSprite != nullptr)
	{
		m_pSprite->removeFromParent();
		m_pSprite = nullptr;
	}
}

Animate*Entity::createAnimate(std::string format,int begin,int end,float delayPerUnit,unsigned int loops)
{
	std::vector<SpriteFrame*> frames;
	auto spriteFrameCache = Director::getInstance()->getSpriteFrameCache();
	//添加资源
	//spriteFrameCache->addSpriteFramesWithFile(xml,png);
	for(int i=begin;i<=end;i++)
	{
		auto frame = spriteFrameCache->getSpriteFrameByName(StringUtils::format(format.c_str(),i));
		frames.push_back(frame);
	}
	Animation*animation = Animation::createWithSpriteFrames(frames,delayPerUnit,loops);
	return Animate::create(animation);
}

void Entity::visit()
{
	auto renderer = Director::getInstance()->getRenderer();
	//开启了遮罩
	if (m_bOpenShade)
	{
		Size size = this->getContentSize();
		auto pos = this->convertToWorldSpace(Point::ZERO);
		SDL_Rect rect = {(int)pos.x,(int)pos.y,(int)size.width,(int)size.height};
		
		renderer->setClipRect(&rect);
		Node::visit();
		renderer->setClipRect(nullptr);
	}
	else
		Node::visit();
}
