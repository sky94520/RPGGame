#ifndef __DisplayLayer_H__
#define __DisplayLayer_H__
#include <string>
#include "SDL_Engine/SDL_Engine.h"

using namespace std;
using namespace SDL;
using namespace SDL::ui;

class Text;

class DisplayLayer : public Layer
{
public:
	enum class Type
	{
		None,
		Quest,/*任务*/
		Forge,/*打造*/
	};
private:
	static const int ITEM_NUMBER_PRE_PAGE;
private:
	//单选按钮组
	RadioButtonGroup* m_pBtnGroup;
	//各种有用的控件
	LabelBMFont* m_pTitleLabel;
	LabelBMFont* m_pDescLabel;
	LabelBMFont* m_pStepLabel;
	Text* m_pRewardText;
	LabelAtlas* m_pPageLabel;

	bool m_bOpened;
	//当前所在的页
	int m_nCurPage;
	Type m_type;
public:
	DisplayLayer();
	~DisplayLayer();
	CREATE_FUNC(DisplayLayer);
	bool init();

	void show(Type type);
	void hide();

	bool isOpened() const { return m_bOpened;}
private:
	//更新页面索引
	void updatePageIndex();
	void closeCallback(Object* sender);
	//切换页面
	void turnPage(Object* sender, bool bNext);
	//选中单选按钮回调函数
	void selectTaskCallback(RadioButton*,int,RadioButtonGroup::EventType);
	//更新按钮及对应的名称
	void updateRadioButton(RadioButton* radioBtn,const string& taskName);
	void updateRadioButtons(RadioButtonGroup* group, vector<Value>& vec);

	vector<Value>& getContent();
	//-----------------------任务相关-------------------------------
	string getTitle(const string& taskName);
	string getDescrition(const string& taskName);
	string getStep(const string& taskName);
	string getReward(const string& taskName);
};
#endif