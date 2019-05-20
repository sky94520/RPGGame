#include "Character.h"
#include "../data/StaticData.h"
#include "../data/CharacterData.h"
#include "../GameMacros.h"

int Character::TILE_WIDTH = 0;
int Character::TILE_HEIGHT = 0;

Character::Character()
	:m_dir(Direction::Down)
	,m_state(State::None)
	,m_bDirty(true)
	,m_bMoving(false)
{
}

Character::~Character()
{
}

Character* Character::create(const string& chartletName)
{
	auto character = new Character();

	if (character && character->init(chartletName))
		character->autorelease();
	else
		SDL_SAFE_DELETE(character);

	return character;
}

Character* Character::create(const string& chartletName, Direction direction)
{
	auto character = new Character();

	if (character && character->init(chartletName, direction))
		character->autorelease();
	else
		SDL_SAFE_DELETE(character);

	return character;
}

bool Character::init(const string& chartletName)
{
	return Character::init(chartletName, Direction::Down);
}

bool Character::init(const string& chartletName, Direction direction)
{
	m_chartletName = chartletName;
	m_dir = direction;
	//设置当前为站立状态
	this->changeState(State::Idle);

	return true;
}

void Character::clear()
{
	m_chartletName = "";
	m_bDirty = true;

	m_dir = Direction::Down;
	m_state = State::None;

	m_bMoving = false;
}

void Character::sit()
{
	this->changeState(State::Idle);
	m_bMoving = false;
	//尝试停止动画
	this->getSprite()->stopActionByTag(Entity::ANIMATION_TAG);
}

void Character::setTilePosition(const Point& tileCoordinate)
{
	Point point;
	//根据锚点来获取真正位置
	auto anchor = this->getSprite()->getAnchorPoint();

	point.x = (tileCoordinate.x + anchor.x) * TILE_WIDTH;
	point.y = (tileCoordinate.y + anchor.y) * TILE_HEIGHT;

	this->setPosition(point);
}

SDL_Point Character::getTilePosition() const
{
	SDL_Point tilePos = { 0, 0 };
	Point pos = this->getPosition();

	tilePos.x = (int)pos.x / TILE_WIDTH;
	tilePos.y = (int)pos.y / TILE_HEIGHT;

	return tilePos;
}

void Character::changeState(State state)
{
	if (m_state == state && !m_bDirty)
		return;

	m_state = state;
	m_bDirty = false;

	//获取动画
	auto animation = StaticData::getInstance()->getCharacterData()->getWalkingAnimation(m_chartletName, m_dir);

	switch (state)
	{
	case State::None:
		break;
	case State::Walking:
		{
		}break;
	case State::Idle://人物站立图跟素材相关
		{
			auto frame = animation->getFrames().at(1)->getSpriteFrame();

			if (m_pSprite == nullptr)
				this->bindSpriteWithSpriteFrame(frame);
			else
			{
				m_pSprite->stopActionByTag(ANIMATION_TAG);
				m_pSprite->setSpriteFrame(frame);
			}
			animation = nullptr;
		}
		break;
	default:
		break;
	}

	if (animation == nullptr)
		return;
	//运行动画
	auto animate = Animate::create(animation);
	animate->setTag(ANIMATION_TAG);

	if (m_pSprite == nullptr)
	{
		this->bindSpriteWithAnimate(animate);
	}
	else
	{
		m_pSprite->stopActionByTag(ANIMATION_TAG);
		m_pSprite->runAction(animate);
	}
}

void Character::moveTo(const SDL_Point& toTile, float duration, CallFunc* endCallback)
{
	//开始新的运动
	Point pos = Point((toTile.x + 0.5f) * TILE_WIDTH,(toTile.y + 0.5f) * TILE_HEIGHT);
	//开始运动
	MoveTo* move = MoveTo::create(duration, pos);
	//运行动作
	auto seq = Sequence::createWithTwoActions(move,endCallback);
	seq->setTag(Entity::ACTION_TAG);

	this->runAction(seq);
	//引擎原因，需要先调用一次
	seq->step(1.f/60);
}

Direction Character::getDirection() const
{
	return m_dir;
}

void Character::setDirection(Direction direction)
{
	if (m_dir != direction)
	{
		m_dir = direction;
		m_bDirty = true;

		this->changeState(m_state);
	}
}

void Character::setTileSize(int tileWidth, int tileHeight)
{
	TILE_WIDTH = tileWidth;
	TILE_HEIGHT = tileHeight;
}
