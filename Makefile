DIR_PATH = ${shell 'pwd'}

DIR_INC  = -I${DIR_PATH}/lib/include  -I${DIR_PATH}/3rd/EasyLogger/linux/inc \
-I${DIR_PATH}/3rd/EasyLogger//linux/plugins/  \
-I${DIR_PATH}/3rd/EasyLogger/linux/plugins/  -I${DIR_PATH}/3rd/EasyLogger/linux/inc

DIR_SRC  = ${DIR_PATH}/lib/src

DIR_OBJ  = ${DIR_PATH}/out/obj

DIR_BIN  = ${DIR_PATH}/out/bin

SRC = $(wildcard ${DIR_SRC}/*.cpp)

OBJ = $(patsubst %.cpp,${DIR_OBJ}/%.o,$(notdir ${SRC}))



TARGET = libjanisa.so
COM_LIB= -lpthread -L$(DIR_BIN)

BIN_TARGET = ${DIR_BIN}/${TARGET}

CC = g++
CFLAGS = -O2 -g3 -Wall ${DIR_INC} -fPIC -shared

$(shell mkdir -p ${DIR_OBJ} ${DIR_BIN})

${BIN_TARGET}:${OBJ}
	$(CC) $(CFLAGS) $(COM_LIB) -o $@ $^

${DIR_OBJ}/%.o:${DIR_SRC}/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:clean
clean:
	@rm -rf ${OBJ} ${BIN_TARGET} >/dev/null 2>&1