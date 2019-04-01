#ifndef __LoadingLayer_H__
#define __LoadingLayer_H__
#include<vector>
#include<map>
#include<string>
#include<stack>
#include "SDL_Engine/SDL_Engine.h"

using namespace std;
USING_NS_SDL;

class LoadingLayer : public Layer
{
private:
	ProgressTimer* m_pProgress1;
	ProgressTimer* m_pProgress2;

	int m_nCurIndex;
	int m_nSize;

	LabelBMFont* m_pLoadingLabel;

	deque<string> m_images;
	deque<string> m_musics;
	deque<string> m_chunks;
	int m_nSaveDataIndex;
public:
	LoadingLayer();
	~LoadingLayer();
	static Scene* createScene(int idx);
	static LoadingLayer* create(int idx);

	bool init(int idx);
private:
	void loadImageCallback(Texture* texture);
	void loadMusicCallback(Object* sound,const string& type);
	void loadChunkCallback(Object* sound,const string& type);

	void loadResAsyncCallback(const string& filename);
};
#endif