#ifndef __EffectLayer_H__
#define __EffectLayer_H__
#include <vector>
#include <string>
#include <cstdio>
#include "SDL_Engine/SDL_Engine.h"

#include "../battle/Fighter.h"

using namespace std;
USING_NS_SDL;

class FlowWord;
class SpritePool;

/*出现位置*/
enum class AnimPosType
{
	Head,
	Body,
	Foot,
	Scene,
	Weapon,/*武器专用*/
};

class EffectLayer : public Layer
{
public:
	EffectLayer();
	~EffectLayer();
	CREATE_FUNC(EffectLayer);
	bool init();
	//展示点击地面动画
	void showClickAnimation(const Point& position, Layer* layer = nullptr);
	//淡入场景
	void fadeInScreen(float duration);
	//淡出场景
	void fadeOutScreen(float duration);
	//展示子弹 飞行特效
	float showBullet(const Point& fromPos, const Point& toPos, const Size& size, const string& bulletName);
	//展示飘字
	void showWord(int number, Point pos, HurtType hurtType);
	//展示动画
	float showAnimationEffect(const string& animationName, const Point& position, bool flipX, Layer* layer);
	float showBalloonIcon(const string& balloonName, const Point& pos, Layer* layer);
private:
	float showAnimationEffect(Sprite* sprite, const string& animationName, const Point& position, Layer* layer = nullptr);
	float showAnimationEffect(Sprite* sprite, Animation* animation, const Point& position, Layer* layer = nullptr);
private:
	static const int ANIMATION_TAG;
private:
	LayerColor* m_pLayerColor;
	FlowWord* m_pFlowWord;
};
#endif