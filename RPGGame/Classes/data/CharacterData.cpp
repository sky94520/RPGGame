#include "CharacterData.h"
#include "../data/StaticData.h"

CharacterData::CharacterData()
{
}

bool CharacterData::init()
{
	return true;
}

bool CharacterData::loadCharacterFile(const string& filename)
{
	Json::CharReaderBuilder readerBuilder;
	unique_ptr<Json::CharReader>const jsonReader(readerBuilder.newCharReader());

	unique_ptr<char> uniqueStr = std::move(FileUtils::getInstance()->getUniqueDataFromFile(filename));
	const char* text = uniqueStr.get();

	Json::String errorMsg;
	//解析失败
	if (!jsonReader->parse(text, text + strlen(text), &m_jsonData, &errorMsg)) {
		LOG(errorMsg.c_str());
		return false;
	}
	auto members = m_jsonData.getMemberNames();
	//角色名
	for (auto chartletName: members) {
		//dict 再次遍历 角色相关 如行走图 战斗图等 以下属性全部可选
		auto keys = m_jsonData[chartletName].getMemberNames();
		for (auto key : keys) {
			Json::Value value = m_jsonData[chartletName][key];
			//加载对应资源
			if (key == "face" || key == "walk")
			{
				auto filename = value["filename"].asString();
				Director::getInstance()->getTextureCache()->addImage(filename);
			}
			else if (key == "sv")
			{
				string filename = value.asString();
				this->addSVAnimation(chartletName, filename);
			}
			else if (key == "level_up_filename") 
			{
				string filename = value.asString();
				this->addLevelUpData(chartletName, filename);
			}
		}
	}
	uniqueStr.reset();
	return true;
}

Animation* CharacterData::getWalkingAnimation(const string& chartletName, Direction direction)
{
	//获取角色map
	Json::Value data = m_jsonData[chartletName];
	if (data.isNull())
		return nullptr;

	Json::Value value = data["walk"];
	//获取文件名 等各种属性
	auto filename = value["filename"].asString();
	int index = value["index"].asInt();
	float delay = value["delay"].asFloat();
	int loops = value["loops"].asInt();
	bool restoreOriginalFrame = value["restoreOriginalFrame"].asBool();

	return this->getWalkingAnimation(filename,index,direction,delay,loops,restoreOriginalFrame);
}

Animation* CharacterData::getWalkingAnimation(const string& filename, int index, Direction dir, float delay, int loops, bool restoreOriginalFrame)
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
	vector<SpriteFrame*> frames;
	this->splitTexture(texture, rect, 48.f, frames);
	auto animation = Animation::createWithSpriteFrames(frames, delay, loops);
	animation->setRestoreOriginalFrame(restoreOriginalFrame);

	return animation;
}

SpriteFrame* CharacterData::getFaceSpriteFrame(const string& filename, int index)
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

SpriteFrame* CharacterData::getFaceSpriteFrame(const string& chartletName)
{
	//获取角色map
	Json::Value data = m_jsonData[chartletName];
	if (data.isNull())
		return nullptr;

	Json::Value value = data["face"];
	//获取文件名 等各种属性
	auto filename = value["filename"].asString();
	int index = value["index"].asInt();

	return this->getFaceSpriteFrame(filename, index);
}

Animation* CharacterData::getSVAnimation(const string&chartletName, FightState fightState)
{
	auto& vec = StaticData::getInstance()->getValueForKey("sv");
	int index = static_cast<int>(fightState);
	//获取对应状态的字符串
	auto& valueMap = vec[index];

	auto sState = valueMap["name"].asString();
	//获取动画
	auto animationName = StringUtils::format("%s_%s",chartletName.c_str(),sState.c_str());
	auto animation = AnimationCache::getInstance()->getAnimation(animationName);

	return animation;
}

string CharacterData::getTurnFilename(const string& chartletName) const
{
	Json::Value data = m_jsonData[chartletName];
	//TODO: 使用默认图片
	if (data.isNull())
		return "";
	
	return data["turn_name"].asString();
}

LevelUp& CharacterData::getDataByLevel(const string& chartletName,int level)
{
	auto& list = m_levelUpData[chartletName];
	return list.at(level - 1);
}

bool CharacterData::addSVAnimation(const string& chartletName, const string& filename)
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
	auto& vec = StaticData::getInstance()->getValueForKey("sv");
	vector<SpriteFrame*> frames;
	string animationName;

	for (int i =0;i < 18;i++)
	{
		int x = i % 3;
		int y = i / 3;
		//获取对应位置的动画名称
		auto& valueMap = vec[i];
		auto name = valueMap["name"].asString();
		auto loop = valueMap["loop"].asBool();
		auto duration = valueMap["duration"].asFloat();

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

bool CharacterData::addLevelUpData(const string& chartletName, const string& filename)
{
	vector<LevelUp>& list = m_levelUpData[chartletName];
	LevelUp st;
	//加载数据
	istringstream in(FileUtils::getInstance()->getDataFromFile(filename));
	string line;
	//不解析第一行
	bool bFirstLine = true;

	auto callback = [&st](int col, Value value)
	{
		switch (col)
		{
		case 0:st.lv = value.asInt(); break;
		case 1:st.properties.attack = value.asInt(); break;
		case 2:st.properties.agility = value.asInt(); break;
		case 3:st.properties.defense = value.asInt(); break;
		case 4:st.properties.magicAttack = value.asInt(); break;
		case 5:st.properties.luck = value.asInt(); break;
		case 6:st.properties.hp = value.asInt(); break;
		case 7:st.properties.mp = value.asInt(); break;
		case 8:st.exp = value.asInt(); break;
		case 9:st.skill = value.asString(); break;
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

void CharacterData::splitTexture(Texture* texture, const Rect& rect,float width, vector<SpriteFrame*>& frames)
{
	int col = int (rect.size.width / width);
	//进行切割
	SpriteFrame* frame = nullptr;

	for (int j = 0;j < col; j++)
	{
		Rect r(rect.origin, Size(width, rect.size.height));
		r.origin.x += j * width;
		//r.origin.y += row * size.height;

		frame = SpriteFrame::createWithTexture(texture,r);
		frames.push_back(frame);
	}
}
