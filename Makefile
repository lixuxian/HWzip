DIR_INC = ./include
DIR_SRC = ./src
DIR_OBJ = ./obj
DIR_BIN = ./bin
DIR_LIB = ./lib
DIR_BOOST = /Users/lixuxian/boost_1_68_0

SRC = $(wildcard ${DIR_SRC}/*.cpp)  
OBJ = $(patsubst %.cpp,${DIR_OBJ}/%.o,$(notdir ${SRC})) 

TARGET = hwzip
TARGET_MAC = hwzip-mac

BUILD_TARGET = build

CC = g++
CFLAGS = -Ofast -Wall -std=c++11 -I${DIR_INC} -I${DIR_BOOST} -fpermissive

OS:=$(shell uname -s)
IS_LINUX:=$(shell echo $(OS) | grep -i Linux)
IS_MAC:=$(shell echo $(OS) | grep -i Darwin)
IS_WIN:=$(shell echo $(OS) | grep -i MINGW)
ifdef IS_LINUX
LDFLAGS+=-L${DIR_LIB} -lppmd
else 
ifdef IS_MAC
LDFLAGS+=-L${DIR_LIB} -lppmd-mac
TARGET=${TARGET_MAC}
endif
ifdef IS_WIN
LDFLAGS+=-L${DIR_LIB} -lppmd-win
endif
endif

BIN_TARGET = ${DIR_BIN}/${TARGET}

all:${BUILD_TARGET} ${BIN_TARGET}

${BUILD_TARGET}:
	mkdir -p obj/


${DIR_OBJ}/%.o:${DIR_SRC}/%.cpp
	$(CC) $(CFLAGS) -c  $< -o $@

${BIN_TARGET}:${OBJ}
	$(CC) $(OBJ)  ${LDFLAGS}  -o $@

install:
	cp ${BIN_TARGET} /usr/local/bin
uninstall:
	rm -f /usr/local/bin/${TARGET}
clean:
	rm -f ${DIR_OBJ}/*.o ${BIN_TARGET}
