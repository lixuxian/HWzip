# 1、准备工作，编译方式、目标文件名、依赖库路径的定义。
CC = g++
CFLAGS  := -O3 -std=c++11 
LIB_PATH = ../../lib


OBJS = pmd.o #.o文件与.cpp文件同名
LIB = libppmd.a # 目标文件名 

INCLUDE_PATH= -I./lib -I./libpmd

all:$(LIB)

# 2. 生成.o文件 
%.o:%.cpp
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE_PATH)

# 3. 生成静态库文件
$(LIB) : $(OBJS)
	rm -f $@
	ar cr $@ $(OBJS)
	rm -f $(OBJS)
	cp $(LIB) $(LIB_PATH)

tags :
	ctags -R *

# 4. 删除中间过程生成的文件 
clean:
	rm -f $(OBJS) $(TARGET) $(LIB)