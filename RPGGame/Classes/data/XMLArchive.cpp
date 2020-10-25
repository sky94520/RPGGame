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
	//玩家状态
	this->writePlayer(doc, root);
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

void XMLArchive::writeSkill(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* root, PlayerData* playerData)
{
	for (Good* skill : playerData->skills)
	{
		const string skillName = skill->getPrototype();
		rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_element, "skill");
		node->append_attribute(doc.allocate_attribute("name", skillName.c_str()));
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
	rapidxml::xml_node<>* bagNode = doc.allocate_node(rapidxml::node_element, "player");
	root->append_node(bagNode);

	vector<Good*>& bagGoodList = m_pUserRecord->bagGoodList;
	for (Good* good : bagGoodList)
	{
		string goodName = good->getPrototype();
		string number = StringUtils::toString(good->getNumber());

		rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_element, "good");
		node->append_attribute(doc.allocate_attribute("name", goodName.c_str()));
		node->append_attribute(doc.allocate_attribute("number", number.c_str()));
		bagNode->append_node(node);
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

void XMLArchive::writeEquipment(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* root, PlayerData* playerData)
{
	for (auto iter2 = playerData->equipments.begin(); iter2 != playerData->equipments.end(); iter2++)
	{
		EquipmentType type = iter2->first;
		Good* good = iter2->second;
		string goodName = good->getPrototype();
		string number = StringUtils::toString(good->getNumber());

		rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_element, "equipment");
		node->append_attribute(doc.allocate_attribute("name", goodName.c_str()));
		node->append_attribute(doc.allocate_attribute("number", number.c_str()));
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
		playerNode->append_attribute(doc.allocate_attribute("name", name.c_str()));
		playerNode->append_attribute(doc.allocate_attribute("level", StringUtils::toString(data->level).c_str()));
		playerNode->append_attribute(doc.allocate_attribute("exp", StringUtils::toString(data->exp).c_str()));
		playerNode->append_attribute(doc.allocate_attribute("maxHp", StringUtils::toString(data->maxHp).c_str()));
		playerNode->append_attribute(doc.allocate_attribute("maxMp", StringUtils::toString(data->maxMp).c_str()));
		//properties
		Properties& prop = data->properties;
		playerNode->append_attribute(doc.allocate_attribute("hp", StringUtils::toString(prop.hp).c_str()));
		playerNode->append_attribute(doc.allocate_attribute("mp", StringUtils::toString(prop.mp).c_str()));
		playerNode->append_attribute(doc.allocate_attribute("attack", StringUtils::toString(prop.attack).c_str()));
		playerNode->append_attribute(doc.allocate_attribute("defense", StringUtils::toString(prop.defense).c_str()));
		playerNode->append_attribute(doc.allocate_attribute("magicAttack", StringUtils::toString(prop.magicAttack).c_str()));
		playerNode->append_attribute(doc.allocate_attribute("magicDefense", StringUtils::toString(prop.magicDefense).c_str()));
		playerNode->append_attribute(doc.allocate_attribute("agiliti", StringUtils::toString(prop.agility).c_str()));
		playerNode->append_attribute(doc.allocate_attribute("luck", StringUtils::toString(prop.luck).c_str()));
		//技能
		this->writeSkill(doc, playerNode, data);
		//装备
		this->writeEquipment(doc, playerNode, data);
	}
}
