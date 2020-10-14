#include "UserRecord.h"
#include "../GameMacros.h"
#include "../entity/Good.h"

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

bool UserRecord::readFromXML(const string& filename)
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
				this->parsePlayer(node);
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

void UserRecord::parsePlayer(rapidxml::xml_node<>* root)
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
	players.insert(make_pair(playerName, data));
	//技能
	auto skillRoot = root->first_node("skill");
	if (skillRoot != nullptr) {
		this->parseSkill(skillRoot);
	}
}

void UserRecord::parseSkill(rapidxml::xml_node<>* root)
{
}

void UserRecord::parseBag(rapidxml::xml_node<>* root)
{
	//遍历背包
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
