#include "FlowWord.h"
#include "../data/StaticData.h"

bool FlowWord::init() 
{
	return true;
}

void FlowWord::showWord( const string& text, Point pos , ColorType colorType ) 
{
	Node* node = nullptr;

	if (text == "miss")
	{
		node = Sprite::createWithSpriteFrameName("miss.png");
	}
	else
	{
		auto& list = StaticData::getInstance()->getValueForKey("damage_filename");
		auto index = static_cast<int>(colorType);
		auto filename = list[index].asString();

		node = LabelAtlas::create(text,filename,24,32,'0');
	}
	node->setPosition(pos);
	this->addChild(node);

	/* 组合*/
    auto scaleSmall = ScaleTo::create(0.5f, 0.5f, 0.5f);
	auto jump = JumpBy::create(0.5f,Point(20,0),-10,1);

	auto spawn = Spawn::createWithTwoActions(scaleSmall,jump);
	/* 动作结束，从父节点中删除自身 */
	RemoveSelf*remove = RemoveSelf::create();
	auto actions = Sequence::create(spawn, remove, NULL);

	node->runAction(actions);
}
