#include "UserRecord.h"
#include "../entity/Good.h"
#include "../entity/Character.h"
#include "../data/StaticData.h"
#include "../data/CharacterData.h"

UserRecord::UserRecord()
	:goldNumber(0)
	,sellRatio(0.f)
{
}

UserRecord::~UserRecord()
{
	auto it = players.begin();

	while (it != players.end())
	{
		PlayerData* data = it->second;
		//释放技能
		for (auto it = data->skills.begin(); it != data->skills.end(); ) {
			Good* good = *it;
			SDL_SAFE_RELEASE(good);
			it = data->skills.erase(it);
		}
		delete data;
		it = players.erase(it);
	}
	//释放背包物品
	for (auto it = m_bagGoodList.begin(); it != m_bagGoodList.end();)
	{
		auto good = *it;
		it = m_bagGoodList.erase(it);
		SDL_SAFE_RELEASE(good);
	}
}

bool UserRecord::readFromXML(const string& filename, bool bFirstGame)
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
				mapFilename = node->value();
			else if (name == "tile_position")
				tileCoordinate = PointFromString(node->value());
			else if (name == "gold")
				goldNumber = SDL_atoi(node->value());
			else if (name == "sell_ratio")
				sellRatio = SDL_atof(node->value());
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

bool UserRecord::writeToXML(const string& filename)
{
	return true;
}

Properties UserRecord::getTotalProperties(const string& playerName)
{
	PlayerData* playerData = players.at(playerName);

	Properties result = playerData->properties;
	auto &equipmentMap = playerData->equipments;

	for (auto iterator : equipmentMap)
	{
		auto equipment = iterator.second;

		if (equipment == nullptr)
			continue;

		auto temp = equipment->getProperties();
		result = result + temp;
	}
	return result;
}

void UserRecord::equip(const string&playerName, int uniqueId, Good* good)
{
	PlayerData* playerData = players[playerName];

	auto equipmentType = good->getEquipmentType();
	auto& equipments = playerData->equipments;
	//原来装备不为空,卸载
	auto iter = equipments.find(equipmentType);
	if (iter != equipments.end())
	{
		auto oldGood = iter->second;
		oldGood->unequip();
		SDL_SAFE_RELEASE(oldGood);

		equipments.erase(iter);
	}
	//更换装备
	good->equip(uniqueId);
	SDL_SAFE_RETAIN(good);
	equipments.insert(make_pair(equipmentType, good));
}

void UserRecord::parsePlayer(rapidxml::xml_node<>* root, bool bFirstGame)
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
	players.insert(make_pair(playerName, data));
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

void UserRecord::parseSkill(rapidxml::xml_node<>* root, PlayerData* playerData)
{
	//解析并创建技能
	string skillName = root->first_attribute("name")->value();
	Good* skill = Good::create(skillName);
	skill->retain();
	playerData->skills.push_back(skill);
}

void UserRecord::parseBag(rapidxml::xml_node<>* root)
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
		m_bagGoodList.push_back(good);
	}
}

void UserRecord::parseEquipment(rapidxml::xml_node<>* root, Character* player)
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
		this->equip(chartletName, uniqueID, good);
	}
}
