
all: chatserver.bin

chatserver.bin: common.cpp main.cpp thread2.cpp thread3.cpp common.h
	g++ -o chatserver.bin common.cpp main.cpp thread2.cpp thread3.cpp -lpthread -g

.PHONY: clean

clean:
	rm chatserver.bin
