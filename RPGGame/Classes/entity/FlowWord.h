/*
    文件名：    FlowWord.h
    描　述：    文字飘动效果
    创建人：    笨木头_钟迪龙 (博客：http://www.benmutou.com)
	修改人：	任继位
*/

#ifndef _FlowWord_H_
#define _FlowWord_H_
#include <string>
#include "SDL_Engine/SDL_Engine.h"

using namespace std;
using namespace SDL;

class FlowWord : public Node 
{
public:
	enum class ColorType
	{
		White,
		Blue,
		Green,
		Yellow,
	};
public:
    CREATE_FUNC(FlowWord);
    virtual bool init();
	//仅能飘动数字0-9 和miss
	void showWord(const string& text, Point pos,ColorType colorType = ColorType::White);
};

#endif