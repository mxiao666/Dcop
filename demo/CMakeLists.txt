# 工程lib库配置文件

set(PROJECT_NAME "OSTEST")

# 工程名称
project(${PROJECT_NAME})

# 设置编译输出目录
set(PROJECT_OUTPUT_DIR ${PROJECT_BINARY_DIR}/../output)

# 添加include目录，本例头文件在./目录下，所以直接取${PROJECT_SOURCE_DIR}/
include_directories(${PROJECT_SOURCE_DIR}/../lib/include)

#添加编写gcc参数
add_compile_options(-Wall -g -std=c++11)

# 添加链接库查找目录，库查找序按照参数顺序从向右查
link_directories(${PROJECT_OUTPUT_DIR})

# 设置生成的库文件目录，这里我们设置成和库目录相同
set(LIBRARY_OUTPUT_PATH ${PROJECT_OUTPUT_DIR}/lib)

# 设置可执行文件目录，同样设成和库目录相同
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_OUTPUT_DIR}/bin)

# 添加子目录（每个子目录下都必须有自已的CMakeLists.txt文件
add_subdirectory(${PROJECT_BINARY_DIR}/../lib lib.out)

# 生成可执行文件
add_executable(demo 
	getmessage.cpp
	demo.cpp)

#连接成最后的可执行文件	
target_link_libraries(demo sys)
