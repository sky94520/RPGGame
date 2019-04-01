#ifndef __OperationLayer_H__
#define __OperationLayer_H__
#include "SDL_Engine/SDL_Engine.h"

USING_NS_SDL;
class OperationLayer : public Layer
{
private:
	ui::Button* m_pBagBtn;
	ui::Button* m_pSaveBtn;
	ui::Button* m_pTaskBtn;
	ui::Button* m_pArrowBtn;
	//按钮是否隐藏
	bool m_bBtnHide;
public:
	OperationLayer();
	~OperationLayer();
	CREATE_FUNC(OperationLayer);
	bool init();

	float setTouchEnabled(bool enable);
private:
	void openBag(Object* sender);
	void save(Object* sender);
	void openTask(Object* sender);
	void clickArrow(Object* sender);
};
#endif