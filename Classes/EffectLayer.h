#ifndef __EffectLayer_H__
#define __EffectLayer_H__
#include <vector>
#include <string>
#include "SDL_Engine/SDL_Engine.h"

using namespace std;
USING_NS_SDL;

class FlowWord;
enum class HurtType;
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
private:
	static const int ANIMATION_TAG;
private:
	vector<Sprite*> m_spritePool;
	Sprite* m_pClickPoint;
	Sprite* m_pWaitClick;
	LayerColor* m_pLayerColor;
	FlowWord* m_pFlowWord;
public:
	EffectLayer();
	~EffectLayer();
	CREATE_FUNC(EffectLayer);
	bool init();
	//展示点击地面动画
	void showClickAnimation(const Point& position, Layer* layer = nullptr);
	//展示等待点击动画
	void showWaitClick(const Point& position);
	void hideWaitClick();
	//显示气泡动画
	float showBalloonIcon(const string& balloonName, const Point& pos, Layer* layer = nullptr);
	//显示子弹动画+动作(可能会有碰撞动画)
	float showBullet(const Point& fromPos, const Point& toPos, const Size& size, const string& bulletName);
	//显示动画特效 并返回持续时间
	float showAnimationEffect(const string& animationName, const Point& position, bool flipX = false, Layer* layer = nullptr);
	//淡入场景
	void fadeInScreen(float duration);
	//淡出场景
	void fadeOutScreen(float duration);
	//显示伤害数字 和miss TODO
	void showWord(int number, Point pos, HurtType hurtType);
private:
	Sprite* popSpriteFromPool();
	void pushSpriteToPool(Sprite* sprite);

	float showAnimationEffect(Sprite* sprite, const string& animationName, const Point& position, Layer* layer = nullptr);
	float showAnimationEffect(Sprite* sprite, Animation* animation, const Point& position, Layer* layer = nullptr);
	//响应动画事件
	void frameEvent(EventCustom*eventCustom);
};
#endif