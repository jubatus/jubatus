.PHONY: all clean build check install

all:
	./waf

clean:
	./waf clean

build:
	./waf build

check:
	./waf --checkall

install:
	./waf install
