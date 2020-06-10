ROOTPATH= ${shell 'pwd'}
LIB_DIR = ${ROOTPATH}/../out/lib
OBJ_DIR = ${ROOTPATH}/../out/obj

INCLUDE = -I$(ROOTPATH)/inc -I$(ROOTPATH)/../elog/inc  -I$(ROOTPATH)/../elog/plugins

DIR_SRC  = ${ROOTPATH}/src

SRC = $(wildcard ${DIR_SRC}/*.cpp)
OBJ = $(patsubst %.cpp,${OBJ_DIR}/%.o,$(notdir ${SRC}))

TARGET = ${LIB_DIR}/libjanisa.so

CC = g++
CFLAGS = -std=c++11 -O2 -g3 -Wall -fPIC -shared

$(shell mkdir -p ${OBJ_DIR} ${LIB_DIR})

all:$(TARGET)
$(TARGET):$(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

${OBJ_DIR}/%.o:${DIR_SRC}/%.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

.PHONY:clean
clean:
	@rm -rf $(OBJ) $(TARGET)