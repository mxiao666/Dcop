# 工程lib库配置文件
project(${PROJECT_NAME})

# 添加include目录
include_directories(./include)
# 设置库生成依赖文件
set(syslib
	src/clibase.cpp
	src/cnotify.cpp
	src/cstream.cpp
	src/log.cpp
	src/objKernel.cpp
	src/objTask.cpp
	src/objbase.cpp
	src/stacktrace.cpp
	src/streamhelper.cpp
	src/system.cpp
	src/template.cpp
	src/timer.cpp
	src/frameworkmgr.cpp)
# 生成libsys.a静态库 	
add_library(sys STATIC ${syslib})

# 本库还依赖其他库文件
target_link_libraries(sys pthread)