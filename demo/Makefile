DIR_PATH = ${shell 'pwd'}
DIR_INC  = -I${DIR_PATH}/../os/inc -I${DIR_PATH}/../elog/inc
DIR_SRC  = ${DIR_PATH}
DIR_OBJ  = ${DIR_PATH}/../out/obj
DIR_BIN  = ${DIR_PATH}/../out/bin
DIR_LIB  = -L${DIR_PATH}/../out/lib #链接时库搜索路径,不指定将在系统目录检索
DIR_DYN  = ../lib	  #运行时库搜索路径,不指定将在系统目录检索

SRC = $(wildcard ${DIR_SRC}/*.cpp)
OBJ = $(patsubst %.cpp,${DIR_OBJ}/%.o,$(notdir ${SRC}))

TARGET = ${DIR_BIN}/demo

COM_LIB= -ljanisa -lelog -lpthread $(DIR_LIB)

CC = g++
CFLAGS =  -O2 -g3 -Wall

$(shell mkdir -p ${DIR_OBJ} ${DIR_BIN})

all:$(TARGET)
${TARGET}:${OBJ}
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(COM_LIB) -Wl,-rpath,$(DIR_DYN) 

${DIR_OBJ}/%.o:${DIR_SRC}/%.cpp
	$(CC) $(CFLAGS) $(DIR_INC) -c  $< -o $@

.PHONY:clean
clean:
	@rm -rf $(OBJ) $(TARGET)