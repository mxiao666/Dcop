DIR_PATH = ${shell 'pwd'}
DIR_INC  = -I${DIR_PATH}/../lib/include -I${DIR_PATH}/../3rd/EasyLogger/easylogger/inc
DIR_SRC  = ${DIR_PATH}
DIR_OBJ  = ${DIR_PATH}/../out/obj
DIR_BIN  = ${DIR_PATH}/../out/bin
DIR_LIB  = ${DIR_BIN} #链接时库搜索路径,不指定将在系统目录检索
DIR_DYN  = ${DIR_BIN} #运行时库搜索路径,不指定将在系统目录检索

SRC = $(wildcard ${DIR_SRC}/*.cpp)
OBJ = $(patsubst %.cpp,${DIR_OBJ}/%.o,$(notdir ${SRC}))

TARGET = demo

BIN_TARGET = ${DIR_BIN}/${TARGET}

COM_LIB= -ljanisa -leasylogger -lpthread -L$(DIR_LIB)

CC = g++
CFLAGS = -g -Wall ${DIR_INC} -Wl,-rpath,$(DIR_DYN)

$(shell mkdir -p ${DIR_OBJ} ${DIR_BIN})

${BIN_TARGET}:${OBJ}
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(COM_LIB)

${DIR_OBJ}/%.o:${DIR_SRC}/%.cpp
	$(CC) $(CFLAGS) -c  $< -o $@

.PHONY:clean
clean:
	@rm -rf ${OBJ} ${BIN_TARGET} >/dev/null 2>&1