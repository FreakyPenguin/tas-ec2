TAS_CODE?=${HOME}/tas/code

CFLAGS+= -I$(TAS_CODE)/include -I$(TAS_CODE)/lib/tas/include
CFLAGS+= -I/usr/include/libnl3

CFLAGS += -std=gnu99 -O3 -g -Wall -Werror
LDFLAGS += -pthread -g 
LDLIBS+= -lnl-3 -lnl-route-3 -lsystemd

wrapper: wrapper.o $(TAS_CODE)/lib/libtas.so

all: wrapper

clean:
	rm -f wrapper *.o
