#ifndef __EffectLayer_H__
#define __EffectLayer_H__
#include <vector>
#include <string>
#include <cstdio>
#include "SDL_Engine/SDL_Engine.h"

using namespace std;
USING_NS_SDL;

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
private:
	float showAnimationEffect(Sprite* sprite, const string& animationName, const Point& position, Layer* layer = nullptr);
	float showAnimationEffect(Sprite* sprite, Animation* animation, const Point& position, Layer* layer = nullptr);
private:
	static const int ANIMATION_TAG;
private:
	LayerColor* m_pLayerColor;
};
#endif