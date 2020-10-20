#ifndef __BattleResult_H__
#define __BattleResult_H__

#include <vector>
#include <string>
#include <unordered_map>
#include <SDL_Engine/SDL_Engine.h>

using namespace std;
using namespace SDL;
using namespace SDL::ui;

class BattleResult : public Layer
{
public:
	BattleResult();
	~BattleResult();
	static BattleResult* create(const string& xmlPath);
	bool init(const string& xmlPath);
	//战斗结算
	void showSummary(int victory);
	void clear();
	//设置战斗是否结束
	bool isBattleOver()const { return m_bBattleOver; }
	void setBattleOver(bool var);
	//是否显示战斗结算面板
	bool isVisible() const { return m_bVisibility; }
	void setVisible(bool var);
	//添加奖励
	void addReward(int exp, int gold);
	void addReward(const string& name, int number);
	//战斗胜利
	void battleWin(LabelAtlas* goldLabel, LabelAtlas* expLabel, LabelBMFont* dropLabel);
	//战斗失败
	void battleFailure(LabelAtlas* goldLabel, LabelAtlas* expLabel, LabelBMFont* dropLabel);
private:
	//战斗结束标识
	bool m_bBattleOver;
	//战斗胜利的奖励
	int m_nExp;
	int m_nGold;
	//战斗结算面板显示
	bool m_bVisibility;
	unordered_map<string, int> m_dropGoods;
	Node* m_pXmlNode;
};

#endif