DIR_PATH = ${shell 'pwd'}
DIR_INC  = ${DIR_PATH}/../lib/include
DIR_SRC  = ${DIR_PATH}
DIR_OBJ  = ${DIR_PATH}/obj
DIR_BIN  = ${DIR_PATH}/bin
DIR_LIB  = ${DIR_PATH}/../lib/bin #链接时库搜索路径,不指定将在系统目录检索
DIR_DYN  = ${DIR_PATH}/../lib/bin #运行时库搜索路径,不指定将在系统目录检索

SRC = $(wildcard ${DIR_SRC}/*.cpp)
OBJ = $(patsubst %.cpp,${DIR_OBJ}/%.o,$(notdir ${SRC}))

TARGET = demo

BIN_TARGET = ${DIR_BIN}/${TARGET}

COM_LIB= -lsys -lpthread -L$(DIR_LIB)

CC = g++
CFLAGS = -g -Wall -I${DIR_INC} -Wl,-rpath,$(DIR_DYN)

$(shell mkdir -p ${DIR_OBJ} ${DIR_BIN})

${BIN_TARGET}:${OBJ}
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(COM_LIB)

${DIR_OBJ}/%.o:${DIR_SRC}/%.cpp
	$(CC) $(CFLAGS) -c  $< -o $@

.PHONY:clean
clean:
	@rm -rf ${DIR_BIN} ${DIR_OBJ} >/dev/null 2>&1