#ifndef __StartScene_H__
#define __StartScene_H__
#include <map>
#include<string>
#include <cstring>
#include "SDL_Engine/SDL_Engine.h"
USING_NS_SDL;
using namespace std;
using namespace SDL::ui;

class StartScene : public Scene
{
private:
	int m_nBGIndex;
	int m_nBGOpacity;
	int m_nAdding;
	Sprite* m_pBGArr[3];//背景精灵数组
	Button* m_pLoadBtn;
	Button* m_pAboutBtn;
	Button* m_pExitBtn;
	CheckBox* m_pMusicBtn;
	CheckBox* m_pChunkBtn;

	Node* m_pSaveDataXml;
public:
	StartScene();
	~StartScene();

	CREATE_FUNC(StartScene);
	bool init();
	void update(float dt);
private:
	//初始化存档层
	void initializeSaveDataLayer();
	//更新ui
	void updateUI();

	void loadCallback(Object* sender);
	void aboutCallback(Object* sender);
	void exitCallback(Object* sender);

	void changeMusic(Object*,ui::CheckBox::EventType type);
	void changeSound(Object*,ui::CheckBox::EventType type);

	void setTouchEnable(bool enable);
	void closeSaveDataLayer(Object* sender);
	//以对应存档开始游戏
	void startGame(Object* sender, int index);
};
#endif