DIR_INC = ./include
DIR_SRC = ./src
DIR_OBJ = ./obj
DIR_BIN = ./bin
DIR_LIB = ./lib

SRC = $(wildcard ${DIR_SRC}/*.cpp)  
OBJ = $(patsubst %.cpp,${DIR_OBJ}/%.o,$(notdir ${SRC})) 

TARGET = hwzip

BIN_TARGET = ${DIR_BIN}/${TARGET}

CC = g++
CFLAGS = -g -Wall -I${DIR_INC}
	 
${DIR_OBJ}/%.o:${DIR_SRC}/%.cpp
	$(CC) $(CFLAGS) -c  $< -o $@

# ${DIR_OBJ}/losslessComp.o:${DIR_SRC}/losslessComp.cpp
# 	$(CC) $(CFLAGS) -c  ${DIR_SRC}/losslessComp.cpp -L${DIR_LIB} -lppmd -o ${DIR_OBJ}/losslessComp.o 

${BIN_TARGET}:${OBJ}
	$(CC) $(OBJ)  -L${DIR_LIB} -lppmd  -o $@
  
clean:
	rm -f ${DIR_OBJ}/*.o ${BIN_TARGET}
