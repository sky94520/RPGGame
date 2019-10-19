#ifndef __LuaScript_H__
#define __LuaScript_H__

#include <string>

#include "lua.hpp"
#include "SDL_Engine/SDL_Engine.h"

USING_NS_SDL;
using namespace std;

struct lua_State;

extern int sdl_lua_loader(lua_State* pL);

class LuaStack : public Object
{
public:
	LuaStack();
	~LuaStack();

	CREATE_FUNC(LuaStack);
	bool init();
    /**
     * Add lua loader.
     * 
     * @param func a function pointer point to the loader function.
     */
	void addLuaLoader(lua_CFunction func);

    /**
     * 添加lua文件搜索路径
     * 
     * @param path 添加的文件路径
     */
	void addLuaSearchPath(const char* path);

	/**
	 *是否已经加载了该文件
	 *
	 * @param filename 文件名
	 * @return 已经加载则返回true，否则返回false
	 */
	bool isLoadedScriptFile(const char* filename) const;

    /**
	 * 执行脚本文件 force是否强制执行该文件
     * 
     * @param filename 文件名称
	 * @param force 强制加载脚本文件
     */
	int executeScriptFile(const string& filename, bool force=false);

	/**
	 * 执行字符串
	 * @param codes 代码字符串
	 * @return 返回执行代码
	 */
	int executeString(const char* codes);

	/**
	 * 执行函数 参数和函数需要先入栈
	 * @param nargs 参数个数
	 * @param nresults 返回值个数
	 * @return 执行结果
	 */
	int executeFunction(int nargs, int nresults);

	/**
	 * 开始或者恢复协程 仅开始时需要函数入栈，其他恢复协程则只需要参数即可
	 *
	 * @param nargs 参数个数
	 * @return 返回函数执行结果
	 */
	int resumeFunction(int nargs);

	/**
	 * 暂停
	 * @param nresult 暂停协程
	 * @return 返回执行结果
	 */
	int yield(int nresult);
	
	//int resumeLuaScript(WaitType waitType, int nargs);

	/**
	 * 把对应类型的值放入栈顶，并返回其类型
	 *
	 * @param name 变量的名称
	 * @return 返回该变量的类型
	 */
	int getLuaGlobal(const char* name);

	/**
	 * 把t[k]放入栈顶，并返回其类型
	 * 
	 * @param index 索引
	 * @key 键名
	 * @return 
	 */
	int getLuaField(int index, const char* key);

	lua_State* getLuaState() const { return m_pLuaState;}

    /**
     * Pushes a integer number with value intValue onto the stack.
     * 
     * @param intValue a integer number.
     */
	void pushInt(int intValue);

    /**
     * Pushes a float number with value floatValue onto the stack.
     *
     * @param floatValue a float number.
     */
	void pushFloat(float floatValue);

    /**
     * Pushes a bool value with boolValue onto the stack.
     * 
     * @param boolValue a bool value.
     */
	void pushBoolean(bool boolValue);

    /**
     * Pushes the zero-terminated string pointed to by stringValue onto the stack.
     *
     * @param stringValue a pointer point to a zero-terminated string stringValue.
     */
	void pushString(const char* stringValue);

    /**
     * Pushes a nil value onto the stack.
     */
	void pushNil();

	/**
	 * 从栈中弹出若干个变量
	 * 
	 * @param n 弹出的变量的个数
	*/
	void pop(int n);

	/**
	 * 把栈中对应索引的值转换成布尔值并返回
	 * 
	 * @param index 栈中的索引
	 * @return 返回转换成功的值
	 */
	bool toLuaBoolean(int index);

	/**
	 * 检测栈中的索引是否可以转换成整型值
	 *
	 * @param index 变量在栈中的索引
	 * @return 类型转换成功后的值
	 */
	int checkLuaInt(int index);

	/**
	 * 检测栈中的索引是否可以转换成浮点值
	 *
	 * @param index 变量在栈中的索引
	 * @return 类型转换成功后的值
	 */
	float checkLuaFloat(int index);

	/**
	 * 检测栈中的索引是否可以转换成字符串
	 *
	 * @param index 变量在栈中的索引
	 * @return 类型转换成功后的值
	 */
	const char* checkLuaString(int index);

	/**
	 * 获取对应索引的长度 lua_getlen()
	 *
	 * @param index 在栈中的索引
	 * @return 返回该变量的长度
	 */
	int getLuaLen(int index);

	/**
	 * lua_geti()
	 */
	int getLuaIndex(int index, int n);

	/**
	 * 移除栈中的元素
	 *
	 */
	void removeLuaIndex(int index);
private:
	lua_State* m_pLuaState;
	int m_nCoroutineRet;
};
#endif