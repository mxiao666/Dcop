# 工程lib库配置文件
project(${PROJECT_NAME})

# 添加include目录
include_directories(../include)
# 设置库生成依赖文件
set(syslib 
	src/os_clibase.cpp
	src/os_cstream.cpp
	src/os_frameworkmgr.cpp
	src/os_log.cpp
	src/os_stacktrace.cpp
	src/os_streamhelper.cpp
	src/os_system.cpp)
	
# 生成libsys.a静态库 	
add_library(sys STATIC ${syslib})

# 本库还依赖其他库文件
target_link_libraries(sys pthread)