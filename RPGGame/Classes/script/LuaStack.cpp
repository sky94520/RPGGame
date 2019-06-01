#include "LuaStack.h"

LuaStack::LuaStack()
	:m_pLuaState(nullptr)
	,m_nCoroutineRet(0)
{
}

LuaStack::~LuaStack()
{
	if (m_pLuaState != nullptr)
	{
		lua_close(m_pLuaState);
	}
}

bool LuaStack::init()
{
	m_pLuaState = luaL_newstate();
	luaL_openlibs(m_pLuaState);

	return true;
}

void LuaStack::addLuaLoader(lua_CFunction func)
{
	//空指针，直接返回
	if (func == nullptr)
		return ;
	//获取对应的表
	lua_getglobal(m_pLuaState, "package");
	lua_getfield(m_pLuaState, -1 ,"searchers");

	lua_pushcfunction(m_pLuaState, func);
	//把searchers表原来的2以后的项往后移1位
	for (int i = (int)lua_rawlen(m_pLuaState, -2) + 1; i > 2; i--)
	{
		//获取项到栈顶
		lua_rawgeti(m_pLuaState, -2, i - 1);
		//转移
		lua_rawseti(m_pLuaState, -3, i);
	}
	//searchers[2] = func
	lua_rawseti(m_pLuaState, -2, 2);

	lua_setfield(m_pLuaState, -2, "searchers");

	//清除栈顶，即清除 package表
	lua_pop(m_pLuaState, 1);
}

void LuaStack::addLuaSearchPath(const char* path)
{
	//获取path
	lua_getglobal(m_pLuaState, "package");
	lua_getfield(m_pLuaState, -1,"path");

	const char* curPath = luaL_checkstring(m_pLuaState, -1);
	//添加
	lua_pushfstring(m_pLuaState, "%s;%s/?.lua", curPath, path);
	lua_setfield(m_pLuaState, -3, "path");

	lua_pop(m_pLuaState, 2);
}

bool LuaStack::isLoadedScriptFile(const char* filename) const
{
	//获取lua的LOADED
	lua_getfield(m_pLuaState, LUA_REGISTRYINDEX, "_LOADED");
	//是否存在对应的键值对
	int type = lua_getfield(m_pLuaState, -1, filename);

	lua_pop(m_pLuaState, 2);

	return type != LUA_TNIL;
}

int LuaStack::executeScriptFile(const string& filename)
{
	bool bShouldLoad = false;

	FileUtils* utils = FileUtils::getInstance();
	string fullpath = utils->fullPathForFilename(filename);
	//仅保留文件名
	auto begin = filename.find_last_of('/');
	auto end = filename.find_last_of('.');

	auto name = filename.substr(begin + 1, end - begin - 1);
	//是否已经加载
	bool loaded = this->isLoadedScriptFile(name.c_str());

	if (!bShouldLoad && loaded)
		return LUA_OK;
	//获取lua的LOADED
	lua_getfield(m_pLuaState, LUA_REGISTRYINDEX, "_LOADED");

	int ret = LUA_ERRRUN;
	size_t size = 0;
	unique_ptr<char> chunk = utils->getUniqueDataFromFile(fullpath, &size);
	//加载并执行
	if (chunk != nullptr)
	{
		const char* data = chunk.get();

		ret = luaL_loadbuffer(m_pLuaState, data, size, nullptr);
		if (ret == LUA_OK)
		{
			ret = lua_pcall(m_pLuaState, 0, 0, 0);
			//赋值
			if (ret == LUA_OK)
			{
				lua_pushboolean(m_pLuaState, 1);
				lua_setfield(m_pLuaState, -2, name.c_str());
			}
		}
		chunk.reset();
	}
	//TODO:打印错误信息
	if (ret != LUA_OK)
	{
		printf("%s", luaL_checkstring(m_pLuaState, -1));
	}

	lua_pop(m_pLuaState, 1);

	return ret;
}

int LuaStack::executeString(const char* codes)
{
	luaL_loadstring(m_pLuaState, codes);
	return this->executeFunction(0, 0);
}

int LuaStack::executeFunction(int nargs, int nresults)
{
	int ret = LUA_ERRRUN;
	//检测-(nargs + 1)是否是函数
	if (lua_isfunction(m_pLuaState, -(nargs + 1)))
	{
		ret = lua_pcall(m_pLuaState, nargs, nresults,NULL);
	}
	else
	{
		lua_pushfstring(m_pLuaState, "the index isn't function %d", -(nargs + 1));
	}
	return ret;
}

int LuaStack::resumeFunction(int nargs)
{
	int ret = LUA_ERRRUN;
	//执行
	ret = lua_resume(m_pLuaState, nullptr, nargs);
	m_nCoroutineRet = ret;

	if (ret != LUA_OK && ret != LUA_YIELD)
	{
		printf("error:%s",luaL_checkstring(m_pLuaState, -1));
	}
	//当执行无误时应该返回LUA_OK或者是LUA_YIELD
	return ret;
}

int LuaStack::yield(int nresult)
{
	return lua_yield(m_pLuaState, nresult);
}

/*
int LuaStack::resumeLuaScript(WaitType waitType, int nargs)
{
	int ret = LUA_ERRRUN;
	//当前等待类型为空 或者不同，唤醒失败
	if (waitType == WaitType::None ||
		waitType != m_pScriptLayer->getWaitType())
		return ret;
	//初始化等待类型
	m_pScriptLayer->setWaitType(WaitType::None);
	//恢复协程
	ret = this->resumeFunction(nargs);

	return ret;
}
*/

int LuaStack::getLuaGlobal(const char* name)
{
	return lua_getglobal(m_pLuaState, name);
}

int LuaStack::getLuaField(int index, const char* key)
{
	return lua_getfield(m_pLuaState, index, key);
}

void LuaStack::pushInt(int intValue)
{
	lua_pushinteger(m_pLuaState, (lua_Integer)intValue);
}

void LuaStack::pushFloat(float floatValue)
{
	lua_pushnumber(m_pLuaState, (lua_Number)floatValue);
}

void LuaStack::pushBoolean(bool boolValue)
{
	int value = boolValue ? 1 : 0;
	lua_pushboolean(m_pLuaState, value);
}

void LuaStack::pushString(const char* stringValue)
{
	lua_pushstring(m_pLuaState, stringValue);
}

void LuaStack::pushNil()
{
	lua_pushnil(m_pLuaState);
}

void LuaStack::pop(int n)
{
	lua_pop(m_pLuaState, n);
}

bool LuaStack::toLuaBoolean(int index)
{
	return lua_toboolean(m_pLuaState, index) == 1;
}

int LuaStack::checkLuaInt(int index)
{
	return (int)luaL_checkinteger(m_pLuaState, index);
}

float LuaStack::checkLuaFloat(int index)
{
	return (float)luaL_checknumber(m_pLuaState, index);
}

const char* LuaStack::checkLuaString(int index)
{
	return luaL_checkstring(m_pLuaState, index);
}

int LuaStack::getLuaLen(int index)
{
	return (int)lua_rawlen(m_pLuaState, index);
}

int LuaStack::getLuaIndex(int index, int n)
{
	return lua_geti(m_pLuaState, index, (lua_Integer)n);
}

void LuaStack::removeLuaIndex(int index)
{
	lua_remove(m_pLuaState, index);
}

int sdl_lua_loader(lua_State* pL)
{
	//后缀为 luac 和lua
	static const std::string BYTECODE_FILE_EXT = ".luac";
	static const std::string NOT_BYTECODE_FILE_EXT = ".lua";
	//取出文件名
	std::string filename(luaL_checkstring(pL, 1));
	//仅保留文件名
	auto begin = filename.find_last_of('/');
	auto end = filename.find_last_of('.');

	auto name = filename.substr(begin + 1, end - begin - 1);
	//去掉扩展名，如果有的话
	size_t pos = filename.rfind(BYTECODE_FILE_EXT);
	if (pos != std::string::npos)
	{
		filename = filename.substr(0, pos);
	}
	else
	{
		pos = filename.rfind(NOT_BYTECODE_FILE_EXT);

		if (pos == filename.length() - NOT_BYTECODE_FILE_EXT.size())
		{
			filename = filename.substr(0, pos);
		}
	}
	//将所有的.替换成/
	pos = filename.find_first_of(".");
	while (pos != std::string::npos)
	{
		filename.replace(pos, 1, "/");
		pos = filename.find_first_of(".", pos + 1);
	}
	//先在package.path中搜索脚本
	FileUtils* utils = FileUtils::getInstance();
	std::unique_ptr<char> chunk = nullptr;
	size_t chunkSize = 0;
	string chunkName;
	//获取package.path
	lua_getglobal(pL, "package");
	lua_getfield(pL, -1, "path");

	std::string searchpath(luaL_checkstring(pL, -1));
	lua_pop(pL, 1);

	begin = 0;
	size_t next = searchpath.find_first_of(";", 0);
	//遍历path中的所有路径，结合文件名进行组装
	do
	{
		if (next == std::string::npos)
			next = searchpath.length();
		std::string prefix = searchpath.substr(begin, next);
		if (prefix[0] == '.' && prefix[1] == '/')
		{
			prefix = prefix.substr(2);
		}
		pos = prefix.find("?.lua");
		chunkName = prefix.substr(0, pos) + filename + NOT_BYTECODE_FILE_EXT;

		if (utils->isFileExist(chunkName))
		{
			chunk =  std::move(utils->getUniqueDataFromFile(chunkName, &chunkSize));
			break;
		}
		else
		{
			chunkName = prefix.substr(0, pos) + filename + BYTECODE_FILE_EXT;
			if (utils->isFileExist(chunkName))
			{
				chunk =  std::move(utils->getUniqueDataFromFile(chunkName, &chunkSize));
				break;
			}
		}
		begin = next + 1;
		next = searchpath.find_first_of(";", begin);
	}while (begin < (int)searchpath.length());
	//可在此处进行解密
	if (chunk != nullptr)
	{
		const char* buffer = chunk.get();
		luaL_loadbuffer(pL, buffer, chunkSize, chunkName.c_str());

		chunk.reset();

		lua_pushstring(pL, name.c_str());

		return 2;
	}
	lua_pushstring(pL, "can not get file data");
	return 1;
}
