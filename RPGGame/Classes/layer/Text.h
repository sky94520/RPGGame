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

/*有着打字机效果的文本类，其实现类似于BMFontLabel*/
class Text : public Node
{
public:
	Text();
	~Text();

	/**
	 * 创建一个Text对象
	 *
	 * @param fontFile 字体文件
	 * @param width 文本区域的宽度
	 * @param color 字体颜色
	 * @return 返回一个Text对象或者nullptr
	*/
	static Text* create(const string& fontFile, int width, const Color3B& color);

	bool init(const string& fontFile, int width, const Color3B& color);
	
	/**
	 * 根据给定的字符串来显示
	 *
	 * @param text 要显示的文本
	 * @param delay 每个文字的延迟时间 >= 0.f
	 * @return 返回显示出所有文字的总时间
	*/
	float wind(const string& text, float delay);

	/**
	 * 每一帧都调用此函数来实现打字效果
	 * 
	 * @param dt 每一帧的持续时间
	 */
	void windRun(float dt);

	/**
	 * 打字是否结束
	 *
	 * @return 文字是否全部打印
	 */
	bool isWindOver() const;

	/**
	 * 全部显示文字 
	 */
	void windOver();

	/**
	 * 删除文本
	 */
	void clear();
private:
	Font* getFont(int fontSize);
private:
	BMFontConfiguration* m_pConfiguration;
	int m_nWidth;
	vector<TextStruct> m_textStructs;
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
};
#endif