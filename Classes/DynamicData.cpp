#include "DynamicData.h"
#include "Good.h"
#include "StaticData.h"
#include "Character.h"

PropertyStruct::PropertyStruct()
	:hp(0)
	,mp(0)
	,attack(0)
	,defense(0)
	,magicAttack(0)
	,magicDefense(0)
	,agility(0)
	,luck(0)
{
}

PropertyStruct::PropertyStruct(int hp, int mp, int attack, int defense, int magicAttack, int magicDefense, int agility, int luck)
	:hp(hp)
	,mp(mp)
	,attack(attack)
	,defense(defense),
	magicAttack(magicAttack)
	,magicDefense(magicDefense)
	,agility(agility)
	,luck(luck)
{
}

PropertyStruct PropertyStruct::operator-(const PropertyStruct& propStruct)
{
	PropertyStruct st;

	st.hp = hp - propStruct.hp;
	st.mp = mp - propStruct.mp;
	st.attack = attack - propStruct.attack;
	st.defense = defense - propStruct.defense;
	st.magicAttack = magicAttack - propStruct.magicAttack;
	st.magicDefense = magicDefense - propStruct.magicDefense;
	st.agility = agility - propStruct.agility;
	st.luck = luck - propStruct.luck;

	return st;
}

PropertyStruct& PropertyStruct::operator-=(const PropertyStruct& propStruct)
{
	this->hp -= propStruct.hp;
	this->mp -= propStruct.mp;
	this->attack -= propStruct.attack;
	this->defense -= propStruct.defense;
	this->magicAttack -= propStruct.magicAttack;
	this->magicDefense -= propStruct.magicDefense;
	this->agility -= propStruct.agility;
	this->luck -= propStruct.luck;

	return *this;
}

PropertyStruct& PropertyStruct::operator+=(const PropertyStruct& propStruct)
{
	this->hp += propStruct.hp;
	this->mp += propStruct.mp;
	this->attack += propStruct.attack;
	this->defense += propStruct.defense;
	this->magicAttack += propStruct.magicAttack;
	this->magicDefense += propStruct.magicDefense;
	this->agility += propStruct.agility;
	this->luck += propStruct.luck;

	return *this;
}

PropertyStruct PropertyStruct::operator+(const PropertyStruct& propStruct)
{
	PropertyStruct st;

	st.hp = hp + propStruct.hp;
	st.mp = mp + propStruct.mp;
	st.attack = attack + propStruct.attack;
	st.defense = defense + propStruct.defense;
	st.magicAttack = magicAttack + propStruct.magicAttack;
	st.magicDefense = magicDefense + propStruct.magicDefense;
	st.agility = agility + propStruct.agility;
	st.luck = luck + propStruct.luck;

	return st;
}

PlayerStruct::PlayerStruct()
	:level(1)
	,exp(0)
	,maxHp(0)
	,maxMp(0)
{
}

PlayerStruct::PlayerStruct(const PropertyStruct& properties)
	:properties(properties)
	,level(1)
	,exp(0)
{
	maxHp = properties.hp;
	maxMp = properties.mp;
}
//--------------------------------------------DynamicData---------------------------------------
DynamicData* DynamicData::s_pInstance = nullptr;

DynamicData* DynamicData::getInstance()
{
	if (s_pInstance == nullptr)
	{
		s_pInstance = new DynamicData();
		s_pInstance->init();
	}
	return s_pInstance;
}

void DynamicData::purge()
{
	SDL_SAFE_RELEASE_NULL(s_pInstance);
}

DynamicData::DynamicData()
	:m_bFirstGame(true)
	,m_nGoldNumber(0)
	,m_sellRatio(0.f)
	,m_nSaveDataIndex(0)
{
}

DynamicData::~DynamicData()
{
	for (auto it = m_bagGoodList.begin(); it != m_bagGoodList.end();)
	{
		auto good = *it;
		it = m_bagGoodList.erase(it);

		SDL_SAFE_RELEASE(good);
	}
	for (auto itMap = m_playerStructs.begin();itMap != m_playerStructs.end();)
	{
		auto& playerStruct = itMap->second;
		auto& equipments = playerStruct.equipments;
		auto& skills = playerStruct.skills;

		for (auto it = equipments.begin();it != equipments.end();)
		{
			SDL_SAFE_RELEASE(it->second);

			it = equipments.erase(it);
		}
		for (auto it = skills.begin();it != skills.end();)
		{
			auto good = *it;
			SDL_SAFE_RELEASE(good);

			it = skills.erase(it);
		}

		itMap = m_playerStructs.erase(itMap);
	}
	this->clearShopGoods();
}

bool DynamicData::init()
{
	//尝试打开管理存档文件
	auto fileUtil = FileUtils::getInstance();
	string path = fileUtil->getWritablePath();
	//对应的存档完整路径
	m_descFilename = path + "save.plist";
	m_descMap = FileUtils::getInstance()->getValueMapFromFile(m_descFilename);
	//是否存在music chunk键
	auto it = m_descMap.find("music");

	if (it != m_descMap.end())
	{
		SoundManager::getInstance()->setPlayingMusic(it->second.asBool());
	}
	it = m_descMap.find("chunk");

	if (it != m_descMap.end())
	{
		SoundManager::getInstance()->setPlayingChunk(it->second.asBool());
	}

	return true;
}

bool DynamicData::initializeSaveData(int idx)
{
	auto fileUtil = FileUtils::getInstance();
	//获取存档路径
	string path = fileUtil->getWritablePath();
	//对应的存档完整路径
	string filepath = m_filename = StringUtils::format("%ssave%d.plist", path.c_str(), idx);
	//不存在对应存档，则使用默认存档
	if ( !fileUtil->isFileExist(m_filename))
	{
		filepath = "data/default_data.plist";
		m_bFirstGame = true;
	}
	else
		m_bFirstGame = false;

	m_nSaveDataIndex = idx;
	//获得对应存档的键值对
	m_valueMap = fileUtil->getValueMapFromFile(filepath);
	//获取有用的数据
	m_nGoldNumber = m_valueMap.at("gold").asInt();

	m_sellRatio = m_valueMap.at("sell_ratio").asFloat();
	//解析背包中的物品并生成Good
	auto& goodList = m_valueMap.at("bag_good_list").asValueVector();
	
	for (auto value : goodList)
	{
		//解析文本，获取lua名称 和个数
		string text = value.asString();
		auto vec = StringUtils::split(text," ");

		string name = vec.front().asString();
		int number = vec.back().asInt();
		//创建并添加
		Good* good = Good::create(name, number);
		SDL_SAFE_RETAIN(good);

		m_bagGoodList.push_back(good);
	}
	return true;
}

bool DynamicData::save(const string&map,const Point& tilePos, int nDir)
{
	m_valueMap["map"] = Value(map);
	//保存位置
	string text = StringUtils::format("{%d,%d}",(int)tilePos.x,(int)tilePos.y);
	m_valueMap["tile_position"] = Value(text);
	//保存主角方向
	m_valueMap["direction"] = Value(nDir);
	//回写存档
	bool ret = FileUtils::getInstance()->writeValueMapToFile(m_valueMap, m_filename);
	//回写管理存档
	auto key = StringUtils::format("save%d", m_nSaveDataIndex);
	auto it = m_descMap.find(key);
	int gameTime = SDL_GetTicks() / 1000;
	ValueMap* descMap = nullptr;
	//TODO:目前仅有一个游戏时间
	if (it != m_descMap.end())
	{
		descMap = &it->second.asValueMap();
		
		gameTime += descMap->at("game_time").asInt();
	}
	else
	{
		m_descMap[key] = ValueMap();
		descMap = &m_descMap[key].asValueMap();
	}
	(*descMap)["game_time"] = Value(gameTime);
	//保存音乐和音效
	m_descMap["music"] = Value(SoundManager::getInstance()->isPlayingMusic());
	m_descMap["chunk"] = Value(SoundManager::getInstance()->isPlayingChunk());

	ret |= FileUtils::getInstance()->writeValueMapToFile(m_descMap, m_descFilename);

	return ret;
}

Good* DynamicData::addGood(const string& goodName,int number)
{
	Good* good = nullptr;
	//是否存在该物品
	auto it = find_if(m_bagGoodList.begin(), m_bagGoodList.end(), [&goodName](Good* good)
	{
		return good->getParentTableName() == goodName;
	});
	//背包中存在该物品
	if (it != m_bagGoodList.end())
	{
		good = *it;
		
		good->setNumber(good->getNumber() + number);
	}//背包中不存在该物品，创建
	else
	{
		good = Good::create(goodName, number);
		SDL_SAFE_RETAIN(good);

		m_bagGoodList.push_back(good);
	}
	//添加成功，更新存档数据
	if (good != nullptr)
	{
		auto &goodList = m_valueMap["bag_good_list"].asValueVector();

		this->updateSaveData(goodList, good, false);
	}
	return good;
}

bool DynamicData::subGood(const string& goodName,int number)
{
	bool ret = false;
	//背包中是否存在该物品
	auto it = find_if(m_bagGoodList.begin(),m_bagGoodList.end(),[goodName](Good*good)
	{
		return good->getParentTableName() == goodName;
	});
	//背包中不存在该物品
	if (it == m_bagGoodList.end())
	{
		return false;
	}
	auto good = *it;
	auto oldNumber = good->getNumber();
	SDL_SAFE_RETAIN(good);
	//存在足够的物品 扣除
	if (good->getNumber() > number)
	{
		good->setNumber(oldNumber - number);
		ret = true;
	}//直接删除
	else if (oldNumber == number)
	{
		good->setNumber(0);
		m_bagGoodList.erase(it);
		
		SDL_SAFE_RELEASE(good);
		ret = true;
	}
	//操作成功，才进行存档更新
	if (ret)
	{
		auto &goodList = m_valueMap["bag_good_list"].asValueVector();

		this->updateSaveData(goodList, good, false);
	}

	SDL_SAFE_RELEASE(good);
	return ret;
}

bool DynamicData::subGood(Good* good, int number)
{
	bool ret = false;
	auto goodNum = good->getNumber();
	//个数足够
	if (goodNum > number)
	{
		good->setNumber(goodNum - number);
		ret = true;
	}
	else if (goodNum == number)
	{
		auto it = find_if(m_bagGoodList.begin(),m_bagGoodList.end(),[good](Good* g)
		{
			return good == g;
		});
		if (it != m_bagGoodList.end())
		{
			m_bagGoodList.erase(it);
			SDL_SAFE_RELEASE(good);

			ret = true;
		}
	}
	return ret;
}

bool DynamicData::subEquipment(const string& playerName, const string& goodName, int number)
{
	bool ret = false;
	auto& playerStruct = m_playerStructs[playerName];
	auto& equipments = playerStruct.equipments;

	auto it = find_if(equipments.begin(), equipments.end(), [&goodName](const pair<EquipmentType, Good*>& it)
	{
		return (it.second)->getTableName() == goodName;
	});
	//找到对应的装备
	if (it != equipments.end())
	{
		auto good = it->second;
		auto num = good->getNumber() - number;
		
		SDL_SAFE_RETAIN(good);

		if (num > 0)
		{
			good->setNumber(num);
		}
		else if (num == 0)
		{
			good->unequip();
			good->setNumber(0);
			SDL_SAFE_RELEASE(good);

			equipments.erase(it);
		}
		//更新存档
		auto& characterMap = m_valueMap["character"].asValueMap();
		auto& playerMap = characterMap[playerName].asValueMap();
		auto& array = playerMap["equipments"].asValueVector();

		this->updateSaveData(array,good,false);
		SDL_SAFE_RELEASE(good);
	}
	return ret;
}

Good* DynamicData::getGood(const string& goodName)
{
	Good* good = nullptr;

	auto it = find_if(m_bagGoodList.begin(), m_bagGoodList.end(), [&goodName](Good* good)
	{
		return good->getParentTableName() == goodName;
	});

	if (it != m_bagGoodList.end())
	{
		good = *it;
	}
	return good;
}

void DynamicData::updateGood(Good* good)
{
	if (good->getNumber() > 0)
		return;
	auto it = find(m_bagGoodList.begin(), m_bagGoodList.end(), good);

	m_bagGoodList.erase(it);
	
	SDL_SAFE_RELEASE(good);
}

int DynamicData::getGoldNumber() const
{
	return m_nGoldNumber;
}

void DynamicData::setGoldNumber(int var)
{
	if (m_nGoldNumber == var)
		return;

	m_nGoldNumber = var;

	m_valueMap["gold"] = Value(m_nGoldNumber);
}

void DynamicData::addShopGood(const string& goodName, int number)
{
	//商店中是否存在该物品
	auto it = find_if(m_shopGoodList.begin(),m_shopGoodList.end(),[goodName](Good*good)
	{
		return good->getParentTableName() == goodName;
	});
	//背包中存在该物品
	if (it != m_shopGoodList.end())
	{
		auto good = *it;
		
		good->setNumber(good->getNumber() + number);
	}
	else//商店中不存在该物品，则创建并添加
	{
		Good* good = Good::create(goodName,number);
		SDL_SAFE_RETAIN(good);

		m_shopGoodList.push_back(good);
	}
}

bool DynamicData::subShopGood(const string& goodName, int number)
{
	bool ret = false;
	//背包中是否存在该物品
	auto it = find_if(m_shopGoodList.begin(),m_shopGoodList.end(),[goodName](Good* good)
	{
		return good->getParentTableName() == goodName;
	});
	//背包中不存在该物品
	if (it == m_shopGoodList.end())
	{
		return false;
	}
	auto good = *it;
	auto oldNumber = good->getNumber();
	SDL_SAFE_RETAIN(good);
	//存在足够的物品 扣除
	if (good->getNumber() > number)
	{
		good->setNumber(oldNumber - number);
		ret = true;
	}//直接删除
	else if (oldNumber == number)
	{
		good->setNumber(0);
		m_shopGoodList.erase(it);
		
		SDL_SAFE_RELEASE(good);
		ret = true;
	}
	return ret;
}

void DynamicData::subShopGood(Good* good,int number)
{
	auto goodNum = good->getNumber();
	//个数足够
	if (goodNum > number)
	{
		good->setNumber(goodNum - number);
	}
	else//移除
	{
		auto it = find_if(m_shopGoodList.begin(),m_shopGoodList.end(),[good](Good*g){
			return good == g;
		});
		if (it != m_shopGoodList.end())
		{
			m_shopGoodList.erase(it);
			SDL_SAFE_RELEASE(good);
		}
	}
}

void DynamicData::clearShopGoods()
{
	for (auto it = m_shopGoodList.begin();it != m_shopGoodList.end();)
	{
		auto good = *it;
		SDL_SAFE_RELEASE(good);

		it = m_shopGoodList.erase(it);
	}
}

void DynamicData::setPlayerStruct(Character* player, const ValueMap& valueMap)
{
	auto playerName = player->getChartletName();
	//TODO:名称
	m_playerStructs[playerName].name = playerName;
	//读取
	for (auto it2 = valueMap.begin();it2 != valueMap.end();it2++)
	{
		auto name = it2->first;
		auto value = it2->second;

		if (name == "level")
		{
			m_playerStructs[playerName].level = value.asInt();
		}
		else if (name == "exp")
		{
			m_playerStructs[playerName].exp = value.asInt();
		}
		else if (name == "max_hp")
		{
			m_playerStructs[playerName].maxHp = value.asInt();
		}
		else if (name == "max_mp")
		{
			m_playerStructs[playerName].maxMp = value.asInt();
		}//解析属性值
		else if (name == "properties")
		{
			this->setProperties(playerName, value.asValueMap());
		}//解析技能
		else if (name == "skills")
		{
			this->studySkill(playerName, value.asValueVector());
		}//解析装备
		else if (name == "equipments")
		{
			this->equip(player, value.asValueVector());
		}
	}
	//第一次进入游戏，使用预设的级别一
	if (m_bFirstGame)
	{
		auto &lvStruct = StaticData::getInstance()->getDataByLevel(playerName, 1);
		auto &properties = lvStruct.properties;
		//设置属性
		this->setProperties(playerName,properties);
		this->setMaxHitPoint(playerName,lvStruct.properties.hp);
		this->setMaxManaPoint(playerName,lvStruct.properties.mp);
	}
}

int DynamicData::getMaxHitPoint(const string& playerName)const
{
	auto &playerStruct = m_playerStructs.at(playerName);

	return playerStruct.maxHp;
}

void DynamicData::setMaxHitPoint(const string& playerName,int var)
{
	auto &playerStruct = m_playerStructs.at(playerName);

	playerStruct.maxHp = var;

	this->updateSaveData(playerName,"max_hp",var);
}

int DynamicData::getMaxManaPoint(const string& playerName)const
{
	auto &playerStruct = m_playerStructs.at(playerName);

	return playerStruct.maxMp;
}

void DynamicData::setMaxManaPoint(const string& playerName,int var)
{
	auto &playerStruct = m_playerStructs.at(playerName);

	playerStruct.maxMp = var;
	this->updateSaveData(playerName,"max_mp",var);
}

int DynamicData::getExp(const string& playerName) const
{
	auto& playerStruct = m_playerStructs.at(playerName);
	return playerStruct.exp;
}

void DynamicData::setExp(const string& playerName,int var)
{
	auto& playerStruct = m_playerStructs[playerName];
	playerStruct.exp = var;

	this->updateSaveData(playerName, "exp", var);
}

int DynamicData::getLevel(const string& playerName) const
{
	auto &playerStruct = m_playerStructs.at(playerName);
	return playerStruct.level;
}

void DynamicData::setLevel(const string& playerName,int var)
{
	auto &playerStruct = m_playerStructs.at(playerName);
	playerStruct.level = var;

	this->updateSaveData(playerName, "level", var);
}

string& DynamicData::getName(const string& playerName)
{
	auto& playerStruct = m_playerStructs.at(playerName);

	return playerStruct.name;
}

Good* DynamicData::getEquipment(const string& playerName, EquipmentType equipmentType)
{
	auto& playerStruct = m_playerStructs[playerName];
	auto& equipments = playerStruct.equipments;
	//获取用户
	auto iter = equipments.find(equipmentType);

	if (iter == equipments.end())
		return nullptr;
	else
		return iter->second;
}

void DynamicData::equip(const string&playerName,int uniqueId,Good* good)
{
	auto& playerStruct = m_playerStructs[playerName];
	auto equipmentType = good->getEquipmentType();
	auto& equipments = playerStruct.equipments;
	//获取用户
	auto iter = equipments.find(equipmentType);
	//原来装备不为空,卸载
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
	equipments.insert(make_pair(equipmentType,good));
	//更新存档
	auto& characterMap = m_valueMap["character"].asValueMap();
	auto& playerMap = characterMap[playerName].asValueMap();

	if (playerMap.find("equipments") == playerMap.end())
		playerMap["equipments"] = ValueVector();

	ValueVector& array = playerMap["equipments"].asValueVector();

	this->updateSaveData(array, good, false);
}

void DynamicData::unequip(const string& playerName, EquipmentType equipmentType)
{
	auto &playerStruct = m_playerStructs[playerName];
	auto &equipments = playerStruct.equipments;
	//获取用户
	auto iter = equipments.find(equipmentType);

	if (iter == equipments.end())
		return ;
	auto good = iter->second;
	good->unequip();
	good->setNumber(good->getNumber() - 1);//TODO
	SDL_SAFE_RELEASE(good);

	equipments.erase(iter);
	//更新存档
	auto& characterMap = m_valueMap["character"].asValueMap();
	auto& playerMap = characterMap[playerName].asValueMap();
	auto& array = playerMap["equipments"].asValueVector();

	this->updateSaveData(array,good,false);
}

void DynamicData::overlyingEquipment(const string& playerName, Good* good, int number)
{
	good->setNumber(good->getNumber() + number);
	//更新存档
	auto& characterMap = m_valueMap["character"].asValueMap();
	auto& playerMap = characterMap[playerName].asValueMap();
	auto& array = playerMap["equipments"].asValueVector();

	this->updateSaveData(array,good,false);
}

bool DynamicData::splitEquipment(const string& playerName, EquipmentType type, Good* good, int number)
{
	SDL_SAFE_RETAIN(good);
	auto count = good->getNumber();
	bool ret = false;

	if (count > number)
	{
		good->setNumber(count - number);
		ret = true;
	}
	else if (count == number)
	{
		this->unequip(playerName, type);
		ret = true;
	}
	//添加拆分的物品到背包
	if (ret)
		this->addGood(good->getParentTableName(), number);
	//更新存档
	auto& characterMap = m_valueMap["character"].asValueMap();
	auto& playerMap = characterMap[playerName].asValueMap();
	auto& array = playerMap["equipments"].asValueVector();

	this->updateSaveData(array,good,false);
	SDL_SAFE_RELEASE(good);

	return ret;
}

PropertyStruct DynamicData::getTotalProperties(const string& playerName)
{
	auto &playerStruct = m_playerStructs.at(playerName);

	PropertyStruct propStruct = playerStruct.properties;
	auto &equipmentMap = playerStruct.equipments;

	for (auto iterator : equipmentMap)
	{
		auto equipment = iterator.second;

		if (equipment == nullptr)
			continue;

		auto temp = equipment->getPropertyStruct();

		propStruct = propStruct + temp;
	}
	return propStruct;
}

PropertyStruct& DynamicData::getProperties(const string& playerName)
{
	auto &playerStruct = m_playerStructs[playerName];

	return playerStruct.properties;
}

void DynamicData::setProperties(const string& playerName, const PropertyStruct& st)
{
	auto &playerStruct = m_playerStructs[playerName];
	playerStruct.properties = st;

	this->updateSaveData(playerName,"hp",st.hp);
	this->updateSaveData(playerName,"mp",st.mp);
	this->updateSaveData(playerName,"attack",st.attack);
	this->updateSaveData(playerName,"magic_attack",st.magicAttack);
	this->updateSaveData(playerName,"defense",st.defense);
	this->updateSaveData(playerName,"magic_defense",st.magicDefense);
	this->updateSaveData(playerName,"agility",st.agility);
	this->updateSaveData(playerName,"luck",st.luck);
}
int DynamicData::getProperty(const string& playerName,PropertyType type)
{
	auto properties = this->getTotalProperties(playerName);
	int value = 0;

	switch (type)
	{
	case PropertyType::Hp:
		value = properties.hp;
		break;
	case PropertyType::Mp:
		value = properties.mp;
		break;
	case PropertyType::Attack:
		value = properties.attack;
		break;
	case PropertyType::Defense:
		value = properties.defense;
		break;
	case PropertyType::MagicAttack:
		value = properties.magicAttack;
		break;
	case PropertyType::MagicDefense:
		value = properties.magicDefense;
		break;
	case PropertyType::Agility:
		value = properties.agility;
		break;
	case PropertyType::Luck:
		value = properties.luck;
		break;
	default:
		break;
	}

	return value;
}

void DynamicData::setProperty(const string& playerName, PropertyType type, unsigned int value)
{
	auto& playerStruct = m_playerStructs[playerName];
	auto& properties = playerStruct.properties;
	string propName;

	switch (type)
	{
	case PropertyType::Hp:
		{
			auto maxHp = this->getMaxHitPoint(playerName);
			//设置最大值限制
			if (maxHp < value)
				value = maxHp;
			properties.hp = value;

			propName = "hp";
		}break;
	case PropertyType::Mp:
		{
			auto maxMp = this->getMaxManaPoint(playerName);
			//设置最大值限制
			if (maxMp < value)
				value = maxMp;
			properties.mp = value;
			propName = "mp";
		}break;
	case PropertyType::Attack:
		properties.attack = value;
		propName = "attack";
		break;
	case PropertyType::Defense:
		properties.defense = value;
		propName = "defense";
		break;
	case PropertyType::MagicAttack:
		properties.magicAttack = value;
		propName = "magic_attack";
		break;
	case PropertyType::MagicDefense:
		properties.magicDefense = value;
		propName = "magic_defense";
		break;
	case PropertyType::Agility:
		properties.agility = value;
		propName = "agility";
		break;
	case PropertyType::Luck:
		properties.luck = value;
		propName = "luck";
		break;
	default:
		break;
	}
	this->updateSaveData(playerName, propName, value);
}

bool DynamicData::studySkill(const string& playerName, const string& skillName)
{
	//检测是否学习了该技能
	auto &skills = m_playerStructs[playerName].skills;

	auto it = find_if(skills.begin(), skills.end(), [skillName](Good* good)
	{
		return good->getParentTableName() == skillName;
	});
	if (it != skills.end())
	{
		printf("the player has studied the skill\n");
		return false;
	}
	//学习技能
	Good* good = Good::create(skillName);
	skills.push_back(good);
	SDL_SAFE_RETAIN(good);
	//更新数据
	auto& playerMap = m_valueMap["character"].asValueMap();
	auto& playerData = playerMap[playerName].asValueMap();
	ValueVector& skillList = playerData["skills"].asValueVector();

	this->updateSaveData(skillList, good, true);

	return true;
}

vector<Good*>& DynamicData::getSkills(const string& playerName)
{
	auto it = m_playerStructs.find(playerName);
	auto& playerStruct = it->second;
	auto& skills = playerStruct.skills;

	return skills;
}

ValueMap& DynamicData::getTotalValueMapOfCharacter()
{
	return m_valueMap.at("character").asValueMap();
}

string DynamicData::getMapFilePath() const
{
	return m_valueMap.at("map").asString();
}

Point DynamicData::getTileCoordinateOfPlayer() const
{
	auto strPoint = m_valueMap.at("tile_position").asString();
	auto position = PointFromString(strPoint);

	return position;
}

ValueMap& DynamicData::getDescValueMap()
{
	return m_descMap;
}

Value* DynamicData::getValueOfKey(const string& key)
{
	Value* value = nullptr;
	//查找
	auto it = m_valueMap.find(key);

	if (it != m_valueMap.end())
	{
		value = &(it->second);
	}
	return value;
}

void DynamicData::setValueOfKey(const string& key, Value& value)
{
	auto it = m_valueMap.find(key);

	if (it != m_valueMap.end())
	{
		it->second = value;
	}
	else//直接插入
	{
		m_valueMap.insert(make_pair(key, value));
	}
}

bool DynamicData::removeKeyValue(const string& key)
{
	auto it = m_valueMap.find(key);
	bool bRet = false;

	if (it != m_valueMap.end())
	{
		m_valueMap.erase(it);
		bRet = true;
	}
	return bRet;
}

bool DynamicData::isHaveQuest(const string& taskName) const
{
	auto it = m_valueMap.find("accepted_task");
	//注册任务
	if (it == m_valueMap.end())
		return false;
	//是否存在对应的任务名称
	auto& tasks = m_valueMap.at("accepted_task").asValueVector();

	auto itVec = find_if(tasks.begin(),tasks.end(),[&taskName](Value& value)
	{
		return value.asString() == taskName;
	});
	return itVec != tasks.end();
}

bool DynamicData::addQuest(const string& taskName)
{
	//该任务已经完成，则直接返回
	if (this->isCompletedQuest(taskName))
		return false;

	auto it = m_valueMap.find("accepted_task");
	//注册任务
	if (it == m_valueMap.end())
		m_valueMap["accepted_task"] = ValueVector();
	//是否存在对应的任务名称
	auto& tasks = m_valueMap["accepted_task"].asValueVector();

	auto itVec = find_if(tasks.begin(),tasks.end(),[&taskName](Value& value)
	{
		return value.asString() == taskName;
	});
	//没找到 注册任务
	if (itVec == tasks.end())
	{
		tasks.push_back(Value(taskName));
		return true;
	}
	return false;
}

bool DynamicData::completeQuest(const string& taskName)
{
	//是否有接受的任务
	bool ret = this->removeAcceptedTask(taskName);
	//任务删除失败
	if (!ret)
		return false;
	//在任务完成列表中注册
	auto it = m_valueMap.find("successful_task");
	//当前没有完成的任务，返回false
	if (it == m_valueMap.end())
	{
		m_valueMap["successful_task"] = ValueVector();
	}
	//是否存在对应的任务名称
	auto &tasks = m_valueMap["successful_task"].asValueVector();

	auto itVec = find_if(tasks.begin(),tasks.end(),[&taskName](Value& value)
	{
		return value.asString() == taskName;
	});
	//没有 插入完成任务
	if (itVec == tasks.end())
	{
		tasks.push_back(Value(taskName));
		return true;
	}
	return false;
}

bool DynamicData::isCompletedQuest(const string& taskName)
{
	//在任务完成列表中查找
	auto it = m_valueMap.find("successful_task");
	//当前没有完成的任务，返回false
	if (it == m_valueMap.end())
	{
		return false;
	}
	//是否存在对应的任务名称
	auto& tasks = m_valueMap["successful_task"].asValueVector();
	auto itVec = find_if(tasks.begin(),tasks.end(),[&taskName](Value& value)
	{
		return value.asString() == taskName;
	});
	//该任务没有完成
	if (itVec == tasks.end())
	{
		return true;
	}
	return false;
}

vector<Value>& DynamicData::getAcceptedTaskList()
{
	auto it = m_valueMap.find("accepted_task");
	//注册任务
	if (it == m_valueMap.end())
		m_valueMap["accepted_task"] = ValueVector();
	
	return m_valueMap["accepted_task"].asValueVector();
}

void DynamicData::updateSaveData(ValueVector& array, Good* good, bool bSkill)
{
	auto goodName = good->getParentTableName();
	auto number = good->getNumber();

	ValueVector::iterator it;
	//获得对应的迭代器
	for (it = array.begin();it != array.end(); it++)
	{
		auto str = it->asString();
		auto index = str.find(goodName);
		//找到该物品
		if (index != string::npos)
		{
			break;
		}
	}
	string text;
	//是技能
	if (bSkill)
	{
		text = StringUtils::format("%s", goodName.c_str());
	}
	else//物品
	{
		text = StringUtils::format("%s %d", goodName.c_str(), number);
	}
	//找到对应字段，则进行覆盖
	if (it != array.end())
	{
		if (number > 0)
			array[it - array.begin()] = Value(text);
		else if (number == 0)
			array.erase(it);
	}
	else if (number > 0)//物品个数大于0，在后面添加
	{
		array.push_back(Value(text));
	}
}

void DynamicData::updateSaveData(const string& playerName, const string& propName, int value)
{
	auto& playerMap = m_valueMap["character"].asValueMap();
	auto& playerData = playerMap[playerName].asValueMap();

	if (propName == "max_hp" || propName == "max_mp" 
		|| propName == "level" || propName == "exp")
	{
		playerData[propName] = Value(value);
	}
	else
	{
		auto& properties = playerData["properties"].asValueMap();
		properties[propName] = Value(value);
	}
}

void DynamicData::setProperties(const string& playerName, const ValueMap& propertyMap)
{
	auto& properties = m_playerStructs[playerName].properties;

	for (auto it = propertyMap.begin();it != propertyMap.end();it++)
	{
		auto name = it->first;
		auto value = it->second.asInt();

		if (name == "hp")
			properties.hp = value;
		else if (name == "mp")
			properties.mp = value;
		else if (name == "attack")
			properties.attack = value;
		else if (name == "defense")
			properties.defense = value;
		else if (name == "magic_attack")
			properties.magicAttack = value;
		else if (name == "magic_defense")
			properties.magicDefense = value;
		else if (name == "agility")
			properties.agility = value;
		else if (name == "luck")
			properties.luck = value;
	}
}

void DynamicData::equip(Character* player, const ValueVector& valueVector)
{
	auto chartletName = player->getChartletName();
	auto uniqueID = player->getUniqueID();

	for (auto it = valueVector.begin();it != valueVector.end();it++)
	{
		auto text = it->asString();
		auto vec = StringUtils::split(text," ");
		auto goodName = vec.at(0).asString();
		auto number = vec.at(1).asInt();
		//先在商品中添加物品
		auto good = Good::create(goodName,number);
		//装备
		this->equip(chartletName, uniqueID, good);
	}
}

void DynamicData::studySkill(const string& playerName, const ValueVector& vec)
{
	for (auto it = vec.begin();it != vec.end();it++)
	{
		auto skillName = it->asString();

		this->studySkill(playerName,skillName);
	}
}

bool DynamicData::removeAcceptedTask(const string& taskName)
{
	//是否有接受的任务
	auto it = m_valueMap.find("accepted_task");
	//注册任务
	if (it == m_valueMap.end())
		m_valueMap["accepted_task"] = ValueVector();
	//是否存在对应的任务名称
	auto& acceptedTasks = m_valueMap["accepted_task"].asValueVector();

	auto itVec = find_if(acceptedTasks.begin(),acceptedTasks.end(),[&taskName](Value& value)
	{
		return value.asString() == taskName;
	});
	if (itVec == acceptedTasks.end())
		return false;
	acceptedTasks.erase(itVec);

	return true;
}