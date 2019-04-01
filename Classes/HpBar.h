#ifndef __HpBar_H__
#define __HpBar_H__

#include "SDL_Engine/SDL_Engine.h"
using namespace SDL;

class HpBar : public Node
{
private:
	Sprite*m_pHpBarBG;
	ProgressTimer*m_pHpBar;
	float m_fMaxValue;
	float m_fCurValue;
	HpBar();
	~HpBar();
public:
	static HpBar*create(std::string textureName,std::string textureBGName = "",float maxValue=100);
	bool init(std::string textureName,std::string textureBGName,float maxValue);
	//value from 1 to maxValue
	void setValue(float curValue);
	float getValue() const;
};
#endif