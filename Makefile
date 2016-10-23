CFLAGS:=-std=gnu99 -Wall

# Demo flags
CFLAGS+=-Wno-unused-variable

all: loggen test/demo

clean:
	rm -f loggen test/demo

