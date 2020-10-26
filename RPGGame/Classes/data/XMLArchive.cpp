#include "XMLArchive.h"
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

bool XMLArchive::write(const string& fullpath, const string&mapName, const Point& tilePos)
{
	rapidxml::xml_document<> doc;
	// 声明
	rapidxml::xml_node<>* declaration = doc.allocate_node(rapidxml::node_declaration);
	declaration->append_attribute(doc.allocate_attribute("version", "1.0"));
	declaration->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(declaration);
	// 根节点
	rapidxml::xml_node<>* root = doc.allocate_node(rapidxml::node_element, "root");
	doc.append_node(root);
	//当前所处地图
	rapidxml::xml_node<>* map = doc.allocate_node(rapidxml::node_element, "map", mapName.c_str());
	root->append_node(map);
	//当前所处位置
	string tilePosition = StringUtils::format("{%d,%d}", (int)tilePos.x, (int)tilePos.y);
	rapidxml::xml_node<>* location = doc.allocate_node(rapidxml::node_element, "tile_position", tilePosition.c_str());
	root->append_node(location);
	//金币
	string goldStr = StringUtils::toString(m_pUserRecord->goldNumber);
	rapidxml::xml_node<>* gold = doc.allocate_node(rapidxml::node_element, "gold", goldStr.c_str());
	root->append_node(gold);
	//出售比例
	string sellRatioStr = StringUtils::toString(m_pUserRecord->sellRatio);
	rapidxml::xml_node<>* sellRatio = doc.allocate_node(rapidxml::node_element, "sell_ratio", sellRatioStr.c_str());
	root->append_node(sellRatio);
	//玩家状态
	this->writePlayer(doc, root);
	//背包物品
	this->writeBag(doc, root);
	//保存
	std::string text;
	rapidxml::print(std::back_inserter(text),doc,0);
	return FileUtils::getInstance()->writeToFile(text, fullpath);
}

void XMLArchive::parsePlayer(rapidxml::xml_node<>* root, bool bFirstGame)
{
	string playerName;
	PlayerData* data = new PlayerData();
	//获取player节点的属性
	for (auto attr = root->first_attribute(); attr != nullptr; attr = attr->next_attribute())
	{
		string name = attr->name();
		auto value = attr->value();

		if (name == "name")
			playerName = value;
		else if (name == "level")
			data->level = SDL_atoi(value);
		else if (name == "exp")
			data->exp = SDL_atoi(value);
		else if (name == "maxHp")
			data->maxHp = SDL_atoi(value);
		else if (name == "maxMp")
			data->maxMp = SDL_atoi(value);
		else if (name == "hp")
			data->properties.hp = SDL_atoi(value);
		else if (name == "mp")
			data->properties.mp = SDL_atoi(value);
		else if (name == "attack")
			data->properties.attack = SDL_atoi(value);
		else if (name == "defense")
			data->properties.defense = SDL_atoi(value);
		else if (name == "magicAttack")
			data->properties.magicAttack = SDL_atoi(value);
		else if (name == "magicDefense")
			data->properties.magicDefense = SDL_atoi(value);
		else if (name == "agility")
			data->properties.agility = SDL_atoi(value);
		else if (name == "luck")
			data->properties.luck = SDL_atoi(value);
	}
	for (auto node = root->first_node(); node != nullptr; node = node->next_sibling())
	{
		string name = node->name();
		if (name == "skill")
			this->parseSkill(node, data);
		else if (name == "equipment")
			this->parseEquipment(node, data);
	}
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
	m_pUserRecord->players.insert(make_pair(playerName, data));
}

void XMLArchive::parseSkill(rapidxml::xml_node<>* root, PlayerData* playerData)
{
	//解析并创建技能
	string skillName = root->first_attribute("name")->value();
	Good* skill = Good::create(skillName);
	skill->retain();
	playerData->skills.push_back(skill);
}

void XMLArchive::writeSkill(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* root, PlayerData* playerData)
{
	for (Good* skill : playerData->skills)
	{
		char* skillName = doc.allocate_string(skill->getPrototype().c_str());
		rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_element, "skill");
		node->append_attribute(doc.allocate_attribute("name", skillName));
		root->append_node(node);
	}
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

void XMLArchive::writeBag(rapidxml::xml_document<>& doc, rapidxml::xml_node<>*root)
{
	rapidxml::xml_node<>* bagNode = doc.allocate_node(rapidxml::node_element, "bag");
	root->append_node(bagNode);

	vector<Good*>& bagGoodList = m_pUserRecord->bagGoodList;
	for (Good* good : bagGoodList)
	{
		char* goodName = doc.allocate_string(good->getPrototype().c_str());
		char* number = doc.allocate_string(StringUtils::toString(good->getNumber()).c_str());

		rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_element, "good");
		node->append_attribute(doc.allocate_attribute("name", goodName));
		node->append_attribute(doc.allocate_attribute("number", number));
		bagNode->append_node(node);
	}
}

void XMLArchive::parseEquipment(rapidxml::xml_node<>* node, PlayerData* data)
{
	//解析装备 <equipment name="Sword" number="1"/>
	string goodName = node->first_attribute("name")->value();
	int number = stoi(node->first_attribute("number")->value());
	//创建并装备
	auto good = Good::create(goodName, number);
	SDL_SAFE_RETAIN(good);
	auto type = good->getEquipmentType();
	data->equipments.insert(make_pair(type, good));
}

void XMLArchive::writeEquipment(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* root, PlayerData* playerData)
{
	for (auto iter2 = playerData->equipments.begin(); iter2 != playerData->equipments.end(); iter2++)
	{
		EquipmentType type = iter2->first;
		Good* good = iter2->second;
		char* goodName = doc.allocate_string(good->getPrototype().c_str());
		char* number = doc.allocate_string(StringUtils::toString(good->getNumber()).c_str());

		rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_element, "equipment");
		node->append_attribute(doc.allocate_attribute("name", goodName));
		node->append_attribute(doc.allocate_attribute("number", number));
		root->append_node(node);
	}
}

void XMLArchive::writePlayer(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* root)
{
	for (auto iter = m_pUserRecord->players.begin(); iter != m_pUserRecord->players.end(); iter++)
	{
		string name = iter->first;
		PlayerData* data = iter->second;
		rapidxml::xml_node<>* playerNode = doc.allocate_node(rapidxml::node_element, "player");
		root->append_node(playerNode);
		//name="" level="" exp="" maxHp maxMp
		playerNode->append_attribute(doc.allocate_attribute("name", doc.allocate_string(name.c_str())));
		map<string, int> attrs;
		//属性
		attrs["level"] = data->level;
		attrs["exp"] = data->exp;
		attrs["mapHp"] = data->maxHp;
		attrs["maxMp"] = data->maxMp;
		//properties
		Properties& prop = data->properties;
		attrs["hp"] = prop.hp;
		attrs["mp"] = prop.mp;
		attrs["attack"] = prop.attack;
		attrs["defense"] = prop.defense;
		attrs["magicAttack"] = prop.magicAttack;
		attrs["magicDefense"] = prop.magicDefense;
		attrs["agility"] = prop.agility;
		attrs["luck"] = prop.luck;

		for (auto iter2 = attrs.begin(); iter2 != attrs.end(); iter2++)
		{
			char* key = doc.allocate_string(iter2->first.c_str());
			string value = StringUtils::toString(iter2->second);
			playerNode->append_attribute(doc.allocate_attribute(key, doc.allocate_string(value.c_str())));
		}
		//技能
		this->writeSkill(doc, playerNode, data);
		//装备
		this->writeEquipment(doc, playerNode, data);
	}
}
