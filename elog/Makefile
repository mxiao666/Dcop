ROOTPATH= ${shell 'pwd'}
LIB_DIR = ${ROOTPATH}/../out/lib
OBJ_DIR = ${ROOTPATH}/../out/obj

INCLUDE = -I$(ROOTPATH)/inc  -I$(ROOTPATH)/plugins/

OBJ += $(patsubst %.c, %.o, $(wildcard $(ROOTPATH)/src/*.c))
OBJ += $(patsubst %.c, %.o, $(wildcard $(ROOTPATH)/port/*.c))
OBJ += $(patsubst %.c, %.o, $(wildcard $(ROOTPATH)/plugins/file/*.c))

CC = gcc
CFLAGS = -O2 -g3 -Wall -fPIC -shared
TARGET = $(LIB_DIR)/libelog.so

$(shell mkdir -p $(LIB_DIR) ${OBJ_DIR})

all:$(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@ 

clean:
	@rm -rf $(OBJ) $(TARGET)