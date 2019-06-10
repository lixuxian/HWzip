DIR_INC = ./include
DIR_SRC = ./src
DIR_OBJ = ./obj
DIR_BIN = ./bin
DIR_LIB = ./lib

SRC = $(wildcard ${DIR_SRC}/*.cpp)  
OBJ = $(patsubst %.cpp,${DIR_OBJ}/%.o,$(notdir ${SRC})) 

TARGET = hwzip

BUILD_TARGET = build

PPMD_TARGET = ppmd

CALERR_TARGET = cal_error

CC = g++
CFLAGS = -g -Ofast -Wall -std=c++11 -I${DIR_INC} -fpermissive

LDFLAGS =  -lglog -lpthread

LDFLAGS+=-L${DIR_LIB} -lppmd

BIN_TARGET = ${DIR_BIN}/${TARGET}

# all:${BUILD_TARGET} ${BIN_TARGET}
all:${CALERR_TARGET} ${PPMD_TARGET} ${BUILD_TARGET} ${BIN_TARGET}


${CALERR_TARGET}:
	$(MAKE) -C calerror

${PPMD_TARGET}:
	$(MAKE) -C 3rd/ppmd

${BUILD_TARGET}:
	mkdir -p obj/

${DIR_OBJ}/%.o:${DIR_SRC}/%.cpp
	$(CC) $(CFLAGS) -c  $< -o $@

${BIN_TARGET}:${OBJ}
	$(CC) $(OBJ)  $(CFLAGS) $(LDFLAGS) -o $@

install:
	cp ${BIN_TARGET} /usr/local/bin
uninstall:
	rm -f /usr/local/bin/${TARGET}
clean:
	$(MAKE) -C calerror clean
	$(MAKE) -C 3rd/ppmd clean
	rm -f ${DIR_OBJ}/*.o ${BIN_TARGET}
