.PHONY: all

all:
	./waf

clean:
	./waf clean

build:
	./waf build

check:
	./waf --checkall
