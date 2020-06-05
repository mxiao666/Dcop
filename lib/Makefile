DIR_PATH = ${shell 'pwd'}
DIR_INC  = ${DIR_PATH}/include
DIR_SRC  = ${DIR_PATH}/src
DIR_OBJ  = ${DIR_PATH}/obj
DIR_BIN  = ${DIR_PATH}/bin

SRC = $(wildcard ${DIR_SRC}/*.cpp)
OBJ = $(patsubst %.cpp,${DIR_OBJ}/%.o,$(notdir ${SRC}))

TARGET = libsys.so
COM_LIB= -lpthread

BIN_TARGET = ${DIR_BIN}/${TARGET}

CC = g++
CFLAGS = -g -Wall -I${DIR_INC} -fPIC -shared

$(shell mkdir -p ${DIR_OBJ} ${DIR_BIN})

${BIN_TARGET}:${OBJ}
	$(CC) $(CFLAGS) $(COM_LIB) -o $@ $^

${DIR_OBJ}/%.o:${DIR_SRC}/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:clean
clean:
	@rm -rf ${DIR_BIN} ${DIR_OBJ} >/dev/null 2>&1