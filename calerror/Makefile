CFLAGS = -O2 -g

OS:=$(shell uname -s)
IS_MAC:=$(shell echo $(OS) | grep -i Darwin)

TARGET=cal_error
TARGET_MAC=cal_error_mac

ifdef IS_MAC
TARGET=${TARGET_MAC}
endif

all:
	g++ ${CFLAGS} -o ${TARGET} cal_error.cpp

clean:
	rm -f *o;
	rm -f ${TARGET};