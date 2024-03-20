# Makefile 示例

# 定义目标（输出文件）
TARGET = legalizer

# 定义编译器和编译选项
CC = g++
CFLAGS = -std=c++11 -Wall

# 定义源文件
SRCS = legalizer.cpp

# 生成目标文件和最终可执行文件
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

# 清理生成的文件
clean:
	rm -f $(TARGET)