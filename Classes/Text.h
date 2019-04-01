#ifndef __Text_H__
#define __Text_H__
#include <map>
#include <vector>
#include <string>
#include <cstdlib>
#include "SDL_Engine/SDL_Engine.h"

using namespace std;
USING_NS_SDL;

struct TextStruct
{
public:
	Sprite* sprite;
	float delay;
public:
	TextStruct(Sprite* sprite, float delay)
		:sprite(sprite)
		,delay(delay)
	{
	}
};

class Text : public Node
{
private:
	BMFontConfiguration* m_pConfiguration;
	int m_nWidth;
	vector<TextStruct> m_textStructs;
	//精灵池
	vector<Sprite*> m_spritePool;
	//当前的字体尺寸
	int m_fontSize;
	//当前颜色
	Color3B m_color;
	//字体路径
	string m_fontpath;
	//当前显示到的精灵索引
	int m_nWindIndex;
	//流逝的时间，和、主要用于定时显示字符
	float m_elapsed;
public:
	Text();
	~Text();
	static Text* create(const string& fontFile, int width, const Color3B& color);

	bool init(const string& fontFile, int width, const Color3B& color);
	
	float wind(const string& text, bool run);
	//打字效果
	void windRun(float dt);
	//打字是否结束
	bool isWindOver() const;
	void windOver();
	//结束
	void over();
private:
	Font* getFont(int fontSize);
	Sprite* popSpriteFromPool();
	Sprite* parseInfo(const u16string& str, int from, float& delay, float& scale, int& count);
};
#endif