#include "UserRecord.h"

UserRecord::UserRecord()
	:m_goldNumber(0)
	,m_sellRatio(0.f)
{
}

UserRecord::~UserRecord()
{
}

bool UserRecord::readFromXML(const string& filename)
{
	rapidxml::xml_document<> doc;
	std::unique_ptr<char> text_ptr = std::move(FileUtils::getInstance()->getUniqueDataFromFile(filename));
	std::string text;

	//�ļ���ȡʧ��
	if (text_ptr == nullptr)
		return false;
	try
	{
		doc.parse<0>(text_ptr.get());
		//root
		auto root = doc.first_node();
		//�Խڵ���б���
		for (auto node = root->first_node(); node != nullptr; node = node->next_sibling())
		{
			string name = node->name();

			if (name == "map")
				m_mapFilename = node->value();
			else if (name == "tile_position")
				m_tilePosOfPlayer = PointFromString(node->value());
			else if (name == "gold")
				m_goldNumber = SDL_atoi(node->value());
			else if (name == "sell_ratio")
				m_sellRatio = SDL_atof(node->value());
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
	//�������
	text_ptr.reset();

	return true;
}

bool UserRecord::writeToXML(const string& filename)
{
	return true;
}

void UserRecord::parsePlayer(rapidxml::xml_node<>* root)
{
}

void UserRecord::parseBag(rapidxml::xml_node<>* root)
{
}
