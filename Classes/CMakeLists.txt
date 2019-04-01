#工程所需最小版本号
cmake_minimum_required(VERSION 3.10)

#找到lua模块
find_package(Lua REQUIRED)

#添加对应的头文件搜索目录
include_directories(${PROJECT_SOURCE_DIR}
	${SDL2_INCLUDE_DIR}
	${LUA_INCLUDE_DIR})

message(WARNING ${LUA_INCLUDE_DIR})
#生成可执行文件
aux_source_directory(. SRC_LIST)
add_executable(main ${SRC_LIST})
#链接对应的函数库
target_link_libraries(main 
	SDL_Engine_shared
	${LUA_LIBRARIES})
#设置生成路径在源路径下
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
