CC = g++
CFLAGS = -Wall -g -Ofast -std=c++11

OBJS = losslessComp.o lossyComp.o fileProcess.o mixCompressor.o errComputer.o utils.o simplifyData.o

all: build hwzip

hwzip: $(OBJS) main.cpp
	$(CC) $(CFLAGS) $(OBJS) main.cpp -o hwzip

build/utils.o: utils.h utils.cpp
	$(CC) $(CFLAGS) utils.cpp -o build/utils.o

build/losslessComp.o: baseComp.h losslessComp.h losslessComp.cpp paq9a.h
	$(CC) $(CFLAGS) losslessComp.cpp -o build/losslessComp.o

build/simplifyData.o: simplifyData.h simplifyData.cpp
	$(CC) $(CFLAGS) simplifyData.cpp -o build/simplifyData.o

build/lossyComp.o: baseComp.h lossyComp.h lossyComp.cpp errComputer.h utils.h simplifyData.h
	$(CC) $(CFLAGS) lossyComp.cpp -o build/lossyComp.o

build/fileProcess.o: fileProcess.h fileProcess.cpp utils.h
	$(CC) $(CFLAGS) fileProcess.cpp -o build/fileProcess.o

build/mixCompressor.o: mixCompressor.h mixCompressor.cpp losslessComp.h lossyComp.h fileProcess.h utils.h
	$(CC) $(CFLAGS) mixCompressor.cpp -o build/mixCompressor.o


build:
	mkdir -p build/
clean:
	rm -f -r build/* hwzip