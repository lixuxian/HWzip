DIR_INC = ./include
DIR_SRC = ./src
DIR_OBJ = ./obj
DIR_BIN = ./bin
DIR_LIB = ./lib

SRC = $(wildcard ${DIR_SRC}/*.cpp)  
OBJ = $(patsubst %.cpp,${DIR_OBJ}/%.o,$(notdir ${SRC})) 

TARGET = hwzip

BUILD_TARGET = build
	
BIN_TARGET = ${DIR_BIN}/${TARGET}

all:${BUILD_TARGET} ${BIN_TARGET}

${BUILD_TARGET}:
	mkdir -p obj/

CC = g++
CFLAGS = -g -Wall -I${DIR_INC} -std=c++11 -fpermissive

OS:=$(shell uname -s)
IS_LINUX:=$(shell echo $(OS) | grep -i Linux)
IS_MAC:=$(shell echo $(OS) | grep -i Darwin)
IS_WIN:=$(shell echo $(OS) | grep -i MINGW)
ifdef IS_LINUX
LDFLAGS+=-L${DIR_LIB} -lppmd
else 
ifdef IS_MAC
LDFLAGS+=-L${DIR_LIB} -lppmd-mac
endif
ifdef IS_WIN
LDFLAGS+=-L${DIR_LIB} -lppmd-win
endif
endif
	 
${DIR_OBJ}/%.o:${DIR_SRC}/%.cpp
	$(CC) $(CFLAGS) -c  $< -o $@

${BIN_TARGET}:${OBJ}
	$(CC) $(OBJ)  ${LDFLAGS}  -o $@

clean:
	rm -f ${DIR_OBJ}/*.o ${BIN_TARGET}
