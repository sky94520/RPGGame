#ifndef __OperationLayer_H__
#define __OperationLayer_H__
#include "SDL_Engine/SDL_Engine.h"

class OperationDelegate 
{
public:
	virtual void openBag()=0;
	virtual void saveProgress()=0;
};

USING_NS_SDL;
class OperationLayer : public Layer
{
public:
	OperationLayer();
	~OperationLayer();
	CREATE_FUNC(OperationLayer);
	bool init();

	void setDelegate(OperationDelegate* pDelegate) { m_pDelegate = pDelegate; }
	float setTouchEnabled(bool enable);
private:
	void openBag(Object* sender);
	void saveProgress(Object* sender);
	void openTask(Object* sender);
	void clickArrow(Object* sender);
private:
	ui::Button* m_pBagBtn;
	ui::Button* m_pSaveBtn;
	ui::Button* m_pTaskBtn;
	ui::Button* m_pArrowBtn;
	//按钮是否隐藏
	bool m_bBtnHide;
	//委托器
	OperationDelegate* m_pDelegate;
};
#endif