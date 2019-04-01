#include "NonPlayerCharacter.h"
#include "StaticData.h"
#include "GameScene.h"

NonPlayerCharacter::NonPlayerCharacter()
	:m_triggerType(TriggerType::None)
	,m_bUpdate(false)
	,m_bObsolete(false)
	,m_nPriority(PRIORITY_LOW)
{
}

NonPlayerCharacter::~NonPlayerCharacter()
{
}

NonPlayerCharacter* NonPlayerCharacter::create(const ValueMap& objects)
{
	auto npc = new NonPlayerCharacter();

	if (npc && npc->init(objects))
		npc->autorelease();
	else
		SDL_SAFE_DELETE(npc);

	return npc;
}

bool NonPlayerCharacter::init(const ValueMap& objects)
{
	bool ret = true;

	float x = objects.at("x").asFloat();
	float y = objects.at("y").asFloat();
	float width = objects.at("width").asFloat();
	float height = objects.at("height").asFloat();
	//获取脚本名
	m_name = objects.at("name").asString();
	//获取对应属性
	const auto& properties = objects.at("properties").asValueMap();
	//获取文件名
	m_filename = properties.at("script").asString();
	m_nPriority = properties.at("priority").asInt();
	//动画名可能不存在
	if (properties.find("chartlet") != properties.end())
	{
		m_chartletName = properties.at("chartlet").asString();
	}
	if (properties.find("update") != properties.end())
	{
		m_bUpdate = properties.at("update").asBool();
	}
	auto triggerStr = properties.at("trigger").asString();
	m_triggerType = this->getTriggerTypeByString(triggerStr);
	//设置碰撞面积
	m_boundingBox.origin = Point(x,y);
	m_boundingBox.size = Size(width,height);

	int uniqueID = this->getUniqueID();
	//获取贴图
	auto size = m_boundingBox.size;
	auto origin = m_boundingBox.origin;

	m_bDirty = true;
	//设置逻辑大小
	if (!m_chartletName.empty())
	{
		ret = Character::init(m_chartletName);
	}
	else if (this->getSprite() == nullptr)
	{
		this->setContentSize(size);
	}
	this->setPosition(Point(origin.x + size.width/2,origin.y + size.height/2));

	return ret;
}

void NonPlayerCharacter::clear()
{
	//清除内部绑定精灵
	this->unbingSprite();
	//默认为显示精灵且参与碰撞
	this->setVisible(true);
	//对部分属性赋予初值
	m_triggerType = TriggerType::None;
	m_bUpdate = false;
	m_bObsolete = false;
	m_nPriority = PRIORITY_LOW;

	m_name = "";
	m_filename = "";
	m_boundingBox = Rect::ZERO;

	Character::clear();
}

bool NonPlayerCharacter::initialize(const string& mapName)
{	
	auto gameScene = GameScene::getInstance();
	auto id = this->getUniqueID();
	//执行脚本
	int ret = LUA_OK; 
	if (!m_filename.empty())
	{
		ret = gameScene->executeScriptFile(m_filename.c_str());
	}
	//脚本加载成功，调用对应的初始化函数
	if (ret == LUA_OK)
	{
		//获取函数
		gameScene->getLuaGlobal(m_name.c_str());
		gameScene->getLuaField(-1, "initialize");
		//放入参数
		gameScene->getLuaGlobal(m_name.c_str());
		gameScene->pushInt(id);
		gameScene->pushString(mapName.c_str());
		//执行函数
		gameScene->executeFunction(3, 0);
		gameScene->pop(1);

		return true;
	}
	else
	{
		string errMsg = gameScene->checkLuaString(-1);
		gameScene->pop(1);

		printf("NonPlayerCharacter::init error:%s\n", errMsg.c_str());
	}

	return false;
}

bool NonPlayerCharacter::execute(int playerID)
{
	auto gameScene = GameScene::getInstance();

	int result = LUA_ERRRUN;
	bool ret = false;
	//获得函数
	gameScene->getLuaGlobal(m_name.c_str());
	gameScene->getLuaField(-1, "execute");
	//移除table
	gameScene->removeLuaIndex(-2);
	//放入参数
	gameScene->getLuaGlobal(m_name.c_str());
	gameScene->pushInt(playerID);
	//执行函数
	result = gameScene->resumeFunction(2);

	int n = lua_gettop(gameScene->getLuaState());
	//如果为 LUA_YIELD 则阻塞
	if (result == LUA_YIELD)
	{
		ret = true;
	}

	return ret;
}

void NonPlayerCharacter::update(float dt)
{
	auto gameScene = GameScene::getInstance();

	int result = LUA_ERRRUN;
	bool ret = false;
	//获得函数
	gameScene->getLuaGlobal(m_name.c_str());
	gameScene->getLuaField(-1, "update");
	//移除table
	gameScene->removeLuaIndex(-2);
	//放入参数
	gameScene->getLuaGlobal(m_name.c_str());
	gameScene->pushFloat(dt);
	//执行函数
	result = gameScene->resumeFunction(2);
	//如果为 LUA_YIELD 则阻塞
	if (result == LUA_YIELD)
	{
		ret = true;
	}
}

void NonPlayerCharacter::clean()
{
	auto gameScene = GameScene::getInstance();

	if(gameScene != nullptr)
	{
		//获得函数
		gameScene->getLuaGlobal(m_name.c_str());
		gameScene->getLuaField(-1 ,"clean");
		//放入参数
		gameScene->getLuaGlobal(m_name.c_str());
		//执行函数
		gameScene->executeFunction(1,0);
		gameScene->pop(1);
	}
}

bool NonPlayerCharacter::intersectRect(const Rect& rect)
{
	//当为隐藏时，不存在碰撞
	if (!this->isVisible())
		return false;

	//查看对应table中是否存在字段range
	auto gameScene = GameScene::getInstance();

	gameScene->getLuaGlobal(m_name.c_str());
	int ret = gameScene->getLuaField(-1, "range");
	//不存在该字段，检测矩形碰撞
	if (ret == LUA_TNIL)
	{
		gameScene->pop(2);

		auto r = this->getBoundingBox();
		return r.intersectRect(rect);
	}
	//获取字段
	string range = gameScene->checkLuaString(-1);
	gameScene->pop(2);
	//全范围
	if (range == "all")
		return true;
	else
		return m_boundingBox.intersectRect(rect);
}

bool NonPlayerCharacter::isTrigger(TriggerType triggerType)
{
	if (triggerType == TriggerType::All)
		return true;
	
	return this->m_triggerType == triggerType;
}

bool NonPlayerCharacter::isPassing() const
{
	return m_nPriority != PRIORITY_SAME;
}

TriggerType NonPlayerCharacter::getTriggerTypeByString(const string& sType)
{
	TriggerType type = TriggerType::None;

	if (sType == "Touch")
		type = TriggerType::Touch;
	else if (sType == "Click")
		type = TriggerType::Click;

	return type;
}
