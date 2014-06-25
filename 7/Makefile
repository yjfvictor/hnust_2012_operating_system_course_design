#
#  实验七  简单二级文件系统设计
# 
#  这题题目说的不是很清楚，具体什么是“二级文件系统”，
#  所以这里以ext2文件系统为例，来实现对ext2文件系统的操作。
# 
# 
#                       叶剑飞
#                       2014年6月18日
#
#

TARGET = 7
OBJS = main.o ext2.o parse_string.o deal_command.o
CC = gcc
CXX = g++
#DEBUG_OPTION=-ggdb -D_DEBUG
CFLAGS = -Wall $(DEBUG_OPTION)
CXXFLAGS = -ggdb -Wall
LDFLAGS = -ggdb -Wall

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)


.PHONY : clean
clean :
	-rm $(TARGET) $(OBJS)
