#include "CharacterData.h"

CharacterData::CharacterData()
{
}

bool CharacterData::init()
{
	return true;
}

bool CharacterData::loadCharacterFile(const string& filename)
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

Animation* CharacterData::getWalkingAnimation(const string& chartletName, Direction direction)
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

/*
Animation* CharacterData::getSVAnimation(const string&chartletName, FightState fightState)
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
*/

string CharacterData::getTurnFilename(const string& chartletName) const
{
	auto& valueMap = m_characterMap.at(chartletName).asValueMap();

	return valueMap.at("turn_name").asString();
}

/*
LevelUpCsv& CharacterData::getDataByLevel(const string& chartletName,int level)
{
	auto& list = m_levelUpCsvMap[chartletName];

	return list.at(level - 1);
}
*/

bool CharacterData::addSVAnimation(const string& chartletName, const string& filename)
{
	/*
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
	*/
	return true;
}

bool CharacterData::addLevelUpData(const string& chartletName, const string& filename)
{
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

