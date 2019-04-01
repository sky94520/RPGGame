#include "StaticData.h"
#include "AStar.h"
#include "DynamicData.h"
#include "Actor.h"
#include "GameScene.h"

StaticData* StaticData::s_pInstance = nullptr;

StaticData* StaticData::getInstance()
{
	if (s_pInstance == nullptr)
	{
		s_pInstance = new StaticData();
		s_pInstance->init();
	}
	return s_pInstance;
}

void StaticData::purge()
{
	SDL_SAFE_RELEASE_NULL(s_pInstance);
}

StaticData::StaticData()
	:m_pAStar(nullptr)
{
}

StaticData::~StaticData()
{
	SDL_SAFE_RELEASE(m_pAStar);
}

bool StaticData::init()
{
	//读取文件并保存键值对
	m_valueMap = FileUtils::getInstance()->getValueMapFromFile(STATIC_DATA_PATH);
	
	m_pAStar = AStar::create();
	SDL_SAFE_RETAIN(m_pAStar);

	return true;
}

Value* StaticData::getValueForKey(const string& key)
{
	auto iter = m_valueMap.find(key);

	if(iter != m_valueMap.end())
		return &iter->second;

	return nullptr;
}

Point StaticData::getPointForKey(const string& key)
{
	Point point;

	auto value = this->getValueForKey(key);

	if (value != nullptr)
	{
		point = PointFromString(value->asString());
	}
	return point;
}

Size StaticData::getSizeForKey(const string& key)
{
	Size size;

	auto value = this->getValueForKey(key);

	if (value != nullptr)
	{
		size = SizeFromString(value->asString());
	}
	return size;
}


bool StaticData::loadCharacterFile(const string& filename)
{
	//获取键值对
	m_characterMap = FileUtils::getInstance()->getValueMapFromFile(filename);
	//遍历
	for (auto it1 = m_characterMap.begin(); it1 != m_characterMap.end(); it1++)
	{
		//角色名
		auto& chartletName = it1->first;
		auto& valueMap = it1->second.asValueMap();
		//角色相关 如行走图 战斗图等 以下属性全部可选
		for (auto value : valueMap)
		{
			auto key = value.first;
			//加载对应资源
			if (key == "face" || key == "walk")
			{
				auto& map = value.second.asValueMap();
				auto filename = map.at("filename").asString();
				//加载并返回
				Director::getInstance()->getTextureCache()->addImage(filename);
			}
			else if (key == "sv")
			{
				string filename = value.second.asString();
				this->addSVAnimation(chartletName, filename);
			}
			else if (key == "level_up_filename") 
			{
				string filename = value.second.asString();
				this->addLevelUpData(chartletName, filename);
			}
		}
	}

	return true;
}

Animation* StaticData::getWalkingAnimation(const string& chartletName, Direction direction)
{
	//获取角色map
	auto it = m_characterMap.find(chartletName);

	if (it == m_characterMap.end())
		return nullptr;

	auto& valueMap = it->second.asValueMap();
	auto& walkMap = valueMap.at("walk").asValueMap();
	//获取文件名 等各种属性
	auto filename = walkMap.at("filename").asString();
	int index = walkMap.at("index").asInt();
	float delay = walkMap.at("delay").asFloat();
	int loops = walkMap.at("loops").asInt();
	bool restoreOriginalFrame = walkMap.at("restoreOriginalFrame").asBool();

	return this->getWalkingAnimation(filename,index,direction,delay,loops,restoreOriginalFrame);
}

Animation* StaticData::getWalkingAnimation(const string& filename, int index, Direction dir, float delay, int loops, bool restoreOriginalFrame)
{
	auto texture = Director::getInstance()->getTextureCache()->getTextureForKey(filename);

	if (texture == nullptr)
		return nullptr;

	int row = index / 4;
	int col = index % 4;
	//获取偏移位置
	Point offset;
	Size size(48.f, 48.f);

	offset.x = col * 3 * size.width;
	offset.y = (row * 4 + static_cast<int>(dir)) * size.height;

	Rect rect(offset, Size(48 * 3, 48.f));
	//进行切割
	auto frames = this->splitTexture(texture, rect, 48.f);
	auto animation = Animation::createWithSpriteFrames(frames, delay, loops);
	animation->setRestoreOriginalFrame(restoreOriginalFrame);

	return animation;
}

SpriteFrame* StaticData::getFaceSpriteFrame(const string& filename, int index)
{
	SpriteFrame* spriteFrame = nullptr;
	//获取对应的贴图
	auto texture = Director::getInstance()->getTextureCache()->addImage(filename);
	//获取行列
	int row = 0, col = 0, width = 0;
	Rect rect;
	//获取宽
	texture->query(NULL,NULL,&width,NULL);

	width /= 4;

	row = index / 4;
	col = index % 4;
	//获取对应矩形
	rect = Rect(col * width, row * width, width, width);

	spriteFrame = SpriteFrame::createWithTexture(texture, rect);

	return spriteFrame;
}

SpriteFrame* StaticData::getFaceSpriteFrame(const string& chartletName)
{
	//获取角色map
	auto it = m_characterMap.find(chartletName);

	if (it == m_characterMap.end())
		return nullptr;

	auto& valueMap = it->second.asValueMap();
	auto& faceMap = valueMap.at("face").asValueMap();
	//获取文件名 等各种属性
	auto filename = faceMap.at("filename").asString();
	int index = faceMap.at("index").asInt();

	return this->getFaceSpriteFrame(filename, index);
}

SpriteFrame* StaticData::getIconSpriteFrame(int id)
{
	//获取IconSet
	Texture*texture = Director::getInstance()->getTextureCache()->getTextureForKey("img/system/IconSet.png");

	int width = 32;
	int height = 32;
	//row cols
	int row = id / 16;
	int col = id % 16;

	int x = width * col;
	int y = height * row;

	Rect rect = Rect(x, y, width, height);

	return SpriteFrame::createWithTexture(texture, rect);
}

Animation* StaticData::getSVAnimation(const string&chartletName, FightState fightState)
{
	auto& vec = this->getValueForKey("sv")->asValueVector();
	int index = static_cast<int>(fightState);
	//获取对应状态的字符串
	auto& valueMap = vec.at(index).asValueMap();

	auto sState = valueMap.at("name").asString();
	//获取动画名称
	auto animationName = StringUtils::format("%s_%s",chartletName.c_str(),sState.c_str());
	//获取动画
	auto animation = AnimationCache::getInstance()->getAnimation(animationName);

	return animation;
}

Animation* StaticData::getAnimation(const string& animationName)
{
	auto animation = AnimationCache::getInstance()->getAnimation(animationName);
	//进行加载
	auto gameScene = GameScene::getInstance();
	//获取对应table
	int nType = gameScene->getLuaGlobal(animationName.c_str());

	if (nType != LUA_TNIL)
	{
		//获取需要监听的帧索引
		gameScene->getLuaField(-1, "monitorFrames");
		//获取table长度
		int len = gameScene->getLuaLen(-1);

		ValueMap dict;
		dict["animationName"] = Value(animationName);

		for (int i = 1;i <= len;i++)
		{
			gameScene->getLuaIndex(-1, i);
			int index = gameScene->checkLuaInt(-1);

			gameScene->pop(1);

			dict["index"] = Value(index);

			animation->getFrames().at(index)->setUserInfo(dict);
		}
		gameScene->pop(1);
	}

	return animation;
}

string StaticData::getTurnFilename(const string& chartletName) const
{
	auto& valueMap = m_characterMap.at(chartletName).asValueMap();

	return valueMap.at("turn_name").asString();
}

LevelUpCsv& StaticData::getDataByLevel(const string& chartletName,int level)
{
	auto& list = m_levelUpCsvMap[chartletName];

	return list.at(level - 1);
}

bool StaticData::direction(Direction dir,string* sDir,Point* delta,Direction* oppsite)
{
	if (sDir == nullptr && delta == nullptr && oppsite == nullptr)
		return false;

	Point temp;
	Direction oppsiteDir = dir;
	string text;

	switch (dir)
	{
	case Direction::Down:
		text = "down";
		temp.y = 1.f;
		oppsiteDir = Direction::Up;
		break;
	case Direction::Left:
		text = "left";
		temp.x = -1.f;
		oppsiteDir = Direction::Right;
		break;
	case Direction::Right:
		text = "right";
		temp.x = 1.f;
		oppsiteDir = Direction::Left;
		break;
	case Direction::Up:
		text = "up";
		temp.y = -1.f;
		oppsiteDir = Direction::Down;
		break;
	default:
		break;
	}

	if (sDir != nullptr)
		*sDir = text;
	if (delta != nullptr)
		*delta = temp;
	if (oppsite != nullptr)
		*oppsite = oppsiteDir;

	return true;
}

string StaticData::toString(PropertyType type) const
{
	//获取属性数组
	auto array = STATIC_DATA_ARRAY("prop_array");
	int index = static_cast<int>(type);
	
	return array.at(index).asString();
}

bool StaticData::addSVAnimation(const string& chartletName, const string& filename)
{
	//加载对应贴图
	auto texture = Director::getInstance()->getTextureCache()->addImage(filename);
	//获取对应贴图的大小
	int w = 0,h = 0;
	texture->query(0,0, &w, &h);
	//获取小图片的大小 大图片为9行6列
	int width = w / 9;
	int height = h / 6;

	//获取预定的贴图
	auto &vec = this->getValueForKey("sv")->asValueVector();
	vector<SpriteFrame*> frames;
	string animationName;

	for (int i =0;i < 18;i++)
	{
		int x = i % 3;
		int y = i / 3;
		//获取对应位置的动画名称
		auto& valueMap = vec.at(i).asValueMap();
		auto name = valueMap.at("name").asString();
		auto loop = valueMap.at("loop").asBool();
		auto duration = valueMap.at("duration").asFloat();

		animationName = StringUtils::format("%s_%s",chartletName.c_str(),name.c_str());
		//一个动画是三帧
		for (int j = 0;j < 3;j++)
		{
			Rect rect;
			rect.origin = Point((3 * x + j) * width, y * height);
			rect.size = Size(width,height);

			SpriteFrame*frame = SpriteFrame::createWithTexture(texture,rect);
			frames.push_back(frame);
		}//TODO
		if (name == "walk")
		{
			for (int j = 2;j >= 0;j--)
			{
				frames.push_back(frames.at(j));
			}
		}
		//创建Animation
		auto animation = Animation::createWithSpriteFrames(frames,duration);

		if (loop == true)
			animation->setLoops(-1);
		else
			animation->setLoops(1);
		//添加到AnimationCache
		AnimationCache::getInstance()->addAnimation(animation,animationName);
		//清除此数组
		frames.clear();
		animationName.clear();
	}
	return true;
}

bool StaticData::addLevelUpData(const string& chartletName, const string& filename)
{
	vector<LevelUpCsv>& list = m_levelUpCsvMap[chartletName];
	LevelUpCsv st;
	//加载数据
	istringstream in(FileUtils::getInstance()->getDataFromFile(filename));
	string line;
	//不解析第一行
	bool bFirstLine = true;

	auto callback = [&st](int col,Value value)
	{
		switch (col)
		{
		case 0:st.lv = value.asInt();break;
		case 1:st.properties.attack = value.asInt();break;
		case 2:st.properties.agility = value.asInt();break;
		case 3:st.properties.defense = value.asInt();break;
		case 4:st.properties.magicAttack = value.asInt();break;
		case 5:st.properties.luck = value.asInt();break;
		case 6:st.properties.hp = value.asInt();break;
		case 7:st.properties.mp = value.asInt();break;
		case 8:st.exp = value.asInt();break;
		case 9:st.skill = value.asString();break;
		default:
			break;
		}
	};
	while (getline(in, line))
	{
		if (bFirstLine)
		{
			bFirstLine = false;
			continue;
		}
		//解析数据
		StringUtils::split(line, ",", callback);
		//存入数据
		list.push_back(st);
	}

	return true; 
}

vector<SpriteFrame*> StaticData::splitTexture(Texture* texture, const Rect& rect,float width)
{
	int col = int (rect.size.width / width);
	//进行切割
	vector<SpriteFrame*> frames;
	SpriteFrame* frame = nullptr;

	for (int j = 0;j < col; j++)
	{
		Rect r(rect.origin, Size(width, rect.size.height));
		r.origin.x += j * width;
		//r.origin.y += row * size.height;

		frame = SpriteFrame::createWithTexture(texture,r);
		frames.push_back(frame);
	}
	return frames;
}

