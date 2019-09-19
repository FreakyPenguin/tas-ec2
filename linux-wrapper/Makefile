TAS_CODE?=${HOME}/tas/code

CFLAGS+= -I/usr/include/libnl3

CFLAGS += -std=gnu99 -O3 -g -Wall -Werror
LDFLAGS += -pthread -g 
LDLIBS+= -lnl-3 -lnl-route-3 -lsystemd -ltas

wrapper: wrapper.o

all: wrapper

clean:
	rm -f wrapper *.o
