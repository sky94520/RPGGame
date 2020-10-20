#ifndef __MessageLayer_H__
#define __MessageLayer_H__

#include <string>
#include "SDL_Engine/SDL_Engine.h"
using namespace std;
USING_NS_SDL;
/*文本位置*/
enum class TextPosition
{
	Bottom,
	Middle,
	Top,
};

enum class FacePosition
{
	Left,
	Right,
};

class Text;

class MessageLayer : public Layer
{
	SDL_BOOL_SYNTHESIZE(m_bShowingText, ShowingText);//是否正在展示文本
	SDL_BOOL_SYNTHESIZE(m_bClickable, Clickable);//是否可点击
private:
	Text* m_pText;
	//记录当前的文本位置
	TextPosition m_curTextPosition;
	//提示xml节点
	Node* m_pTipNode;
	//按钮集
	Menu* m_pMenu;
public:
	MessageLayer();
	~MessageLayer();

	CREATE_FUNC(MessageLayer);
	bool init();
	//显示文本 name 名字 text 文本 textPos 文本位置 run 是否打字机效果 face 立绘
	float showText(const string& name, const string& text, TextPosition textPos, bool run, const string& faceFilename, int index, FacePosition facePos);
	float showText(const string& name, const string& text, TextPosition textPos, bool run);
	//隐藏文本
	void hideText();
	//显示提示文本
	//float showTip(const string& text, TextPosition, bool run);
	//显示提示框 和另一个showTip不同
	void showTip(const string& text, TextPosition posType, float duration);
	//获取当前文本的动画所在的位置
	Point getWaitClickPosition();
	//添加按钮
	void addButton(const string& text);
	//排列按钮
	void alignItemsVertically();
	//组合
	void windRun(float dt);
	void windOver();
	bool isWindOver() const;
private:
	Point getPosition(TextPosition textPos, const Size& bgSize);
	void clickBtn(Object* sender);
};
#endif