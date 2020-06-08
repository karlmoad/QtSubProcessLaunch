.PHONY: all ref build clean
.DEFAULT_GOAL := build

DIST_PATH := build

all: clean build

ref:
	go get -u

build: clean
	go build -o $(DIST_PATH)/EchoService ./main.go

clean:
	rm -f $(DIST_PATH)/EchoService
