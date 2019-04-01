#ifndef __GoodLayer_H__
#define __GoodLayer_H__
#include <vector>
#include "SDL_Engine/SDL_Engine.h"

using namespace std;
USING_NS_SDL;
using namespace SDL::ui;
class Character;
class Good;
struct PropertyStruct;

class GoodLayer : public Layer
{
public:
	enum class Type
	{
		Bag,
		Skill,
		ShopBuy,/*从商店购买TODO*/
		ShopSale,/*出售给商店*/
		Seed,/*种子*/
	};
private:
	static const int GOOD_NUMBER_PER_PAGE = 4;
private:
	bool m_bOpened;
	//玩家组
	RadioButtonGroup* m_pPlayerGroup;
	//物品组
	RadioButtonGroup* m_pGoodGroup;
	//文件的xml节点
	Node* m_pXmlNode;
	//当前的页面
	int m_nCurPage;
	//暂存物品 最大值为8
	vector<Good*> m_goods;
	//xml内常用的按钮
	Button* m_pUseBtn;
	Button* m_pEquipBtn;
	Button* m_pToggleBtn;
	//当前按住的按钮 目前仅仅用在装备按钮上
	Button* m_pSelectedBtn;
	//流逝时间
	float m_elapsed;
	//属性标签
	vector<LabelBMFont*> m_propLabels;
	vector<LabelBMFont*> m_deltaPropLabels;
	//显示类型
	Type m_type;
	//是否只显示背包，即不显示角色状态栏和角色
	bool m_bOnlyShowingBag;
public:
	GoodLayer();
	~GoodLayer();

	CREATE_FUNC(GoodLayer);
	bool init();

	void update(float dt);
	//显示背包
	void show(Type type = Type::Bag);
	//隐藏背包
	void hide();
	//更新背包物品显示 4 or 8
	void updateShownOfGoods(int number = 4);
	//更新经验值
	void updateShownOfExp();
	//更新金币
	void updateShownOfGold(int number);
	//更新属性值
	void updateShownOfProps();
	//当前背包是否已经打开
	bool isOpened() const;
	//获取当前选中的主角
	Character* getSelectedPlayer() const;
	//锁定/取消锁定 角色
	void lockPlayer(int uniqueID);
	void unlockPlayer();

	Type getType() const { return m_type; }

	bool isOnlyShowingBag() const { return m_bOnlyShowingBag; }
	void setOnlyShowingBag(bool isShowing) { m_bOnlyShowingBag = isShowing; }
private:
	void initializeUI();
	//更新页面索引
	void updatePageIndex();
	//改变当前显示类型
	bool changeType(Type type);
	//------------------------一系列回调函数---------------------------
	void closeBag(Object* sender);
	//选中玩家回调函数
	void selectPlayer(RadioButton*,int,RadioButtonGroup::EventType);
	//选中物品回调函数
	void selectGood(RadioButton* radioBtn,int index,RadioButtonGroup::EventType);
	//切换页面
	void turnPage(Object* sender, bool bNext);
	//使用物品交给GameScene
	void useGood(Object* sender);
	//装备/卸下
	void equipOrUnequip(Object* sender);
	void equipOrUnequipCallback(Object*,TouchEventType);
	//切换
	void toggle(Object* sender);
private:
	//更新物品选项 good 为nullptr，设置为隐藏
	void updateRadioButton(RadioButton* radioBtn, Good* good);
	void updateRadioButton(RadioButton* radioBtn, Character* player);

	void updateLabelOfProp(Character* player);
	void updateLabelOfDeltaProp(const PropertyStruct& deltaProp);
	//获取要显示的内容
	vector<Good*>& getContent();
	vector<Good*>& getContent(int);
	//更新标签
	void updatePropLabels(const string& chartletName, const PropertyStruct& propStruct,bool bPlayerProperties);
	//更新物品显示
	template<typename T>
	void updateRadioButtons(RadioButtonGroup* group, vector<T>& vec,int number)
	{
		//是否应该更新
		auto selectedIndex = group->getSelectedIndex();
		RadioButton* selectedBtn = nullptr;
		T selectedItem = nullptr;

		bool bShouldUpdate = false;

		if (selectedIndex == -1)
			selectedIndex = 0;
		else
			selectedBtn = group->getRadioButtonByIndex(selectedIndex);

		if (selectedBtn != nullptr)
			selectedItem = static_cast<T>(selectedBtn->getUserObject());
		
		if (selectedItem == nullptr || selectedItem != vec[selectedIndex])
			bShouldUpdate = true;

		for (int i = number - 1;i >= 0 ;i--)
		{
			auto radioBtn = group->getRadioButtonByIndex(i);

			T item = nullptr;
			
			if (i < (int)vec.size())
			{
				item = vec.at(i);
			}
			//更新对应的单选按钮
			this->updateRadioButton(radioBtn, item);
			//按钮是选中项 或者应该更新还没更新
			if (selectedIndex >= i && bShouldUpdate)
			{
				//当前是选中项，先取消选中
				if (selectedBtn == radioBtn)
					group->unselectedButton();
				//重新设置选中
				if (item != nullptr)
				{
					bShouldUpdate = false;
					group->setSelectedButton(radioBtn);
				}
			}
		}
	}

};
#endif
