#include "Archive.h"
#include "UserRecord.h"

#include "../entity/Good.h"
#include "../entity/Character.h"

#include "../data/StaticData.h"
#include "../data/CharacterData.h"

bool XMLArchive::read(const string& filename, bool bFirstGame)
{
	rapidxml::xml_document<> doc;
	std::unique_ptr<char> text_ptr = std::move(FileUtils::getInstance()->getUniqueDataFromFile(filename));
	std::string text;

	//文件读取失败
	if (text_ptr == nullptr)
		return false;
	try
	{
		doc.parse<0>(text_ptr.get());
		//root
		auto root = doc.first_node();
		//对节点进行遍历
		for (auto node = root->first_node(); node != nullptr; node = node->next_sibling())
		{
			string name = node->name();

			if (name == "map")
				m_pUserRecord->mapFilename = node->value();
			else if (name == "tile_position")
				m_pUserRecord->tileCoordinate = PointFromString(node->value());
			else if (name == "gold")
				m_pUserRecord->goldNumber = SDL_atoi(node->value());
			else if (name == "sell_ratio")
				m_pUserRecord->sellRatio = SDL_atof(node->value());
			else if (name == "direction")
				;
			else if (name == "player")
				this->parsePlayer(node, bFirstGame);
			else if (name == "bag")
				this->parseBag(node);
		}
	}
	catch(std::runtime_error e)
	{
		std::cout << e.what() << std::endl;
	}
	//清除缓存
	text_ptr.reset();

	return true;
}

bool XMLArchive::write(const string& filename)
{
	return true;
}

void XMLArchive::parsePlayer(rapidxml::xml_node<>* root, bool bFirstGame)
{
	string playerName;
	PlayerData* data = new PlayerData();
	//获取player节点的属性
	for (auto attr = root->first_attribute(); attr != nullptr; attr = attr->next_attribute())
	{
		auto name = attr->name();
		auto value = attr->value();

		if (strcmp(name, "name") == 0)
			playerName = value;
		else if (strcmp(name, "level") == 0)
			data->level = SDL_atoi(value);
		else if (strcmp(name, "exp") == 0)
			data->exp = SDL_atoi(value);
	}
	//技能
	auto skillRoot = root->first_node("skill");
	if (skillRoot != nullptr) {
		this->parseSkill(skillRoot, data);
	}
	m_pUserRecord->players.insert(make_pair(playerName, data));
	//第一次进入游戏，使用预设的级别一
	if (bFirstGame)
	{
		auto characterData = StaticData::getInstance()->getCharacterData();
		auto &lvStruct = characterData->getDataByLevel(playerName, 1);
		auto &properties = lvStruct.properties;
		//设置属性
		data->properties = properties;
		data->maxHp = lvStruct.properties.hp;
		data->maxMp = lvStruct.properties.mp;
	}
}

void XMLArchive::parseSkill(rapidxml::xml_node<>* root, PlayerData* playerData)
{
	//解析并创建技能
	string skillName = root->first_attribute("name")->value();
	Good* skill = Good::create(skillName);
	skill->retain();
	playerData->skills.push_back(skill);
}

void XMLArchive::parseBag(rapidxml::xml_node<>* root)
{
	//背包物品<good name="Herbs" number="2">
	for (auto node = root->first_node(); node != nullptr; node = node->next_sibling())
	{
		//<good name="Herbs" number="2">
		auto nameAttr = node->first_attribute("name");
		auto numberAttr = node->first_attribute("number");
		//获取物品的名称和数量
		string name = nameAttr->value();
		int number = 1;
		if (numberAttr != nullptr) {
			number = SDL_atoi(numberAttr->value());
		}
		//创建物品
		Good* good = Good::create(name, number);
		SDL_SAFE_RETAIN(good);
		m_pUserRecord->bagGoodList.push_back(good);
	}
}

void XMLArchive::parseEquipment(rapidxml::xml_node<>* root, Character* player)
{
	auto chartletName = player->getChartletName();
	auto uniqueID = player->getUniqueID();
	//解析装备 <equipment name="Sword" number="1"/>
	for (auto node = root->first_node(); node != nullptr; node = node->next_sibling())
	{
		string goodName = node->first_attribute("name")->value();
		int number = stoi(node->first_attribute("number")->value());
		//创建并装备
		auto good = Good::create(goodName, number);
		m_pUserRecord->equip(chartletName, uniqueID, good);
	}
}
