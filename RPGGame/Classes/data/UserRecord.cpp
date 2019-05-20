﻿#include "UserRecord.h"
#include "../GameMacros.h"

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
		//TODO:后续数据
	}
	//保存数据
	players.insert(make_pair(playerName, data));
}

void UserRecord::parseBag(rapidxml::xml_node<>* root)
{
}
