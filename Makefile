CFLAGS := -Wall -O2 -g
CFLAGS += -I$(shell pwd)/ -I./live555/BasicUsageEnvironment/include -I./live555/liveMedia/include -I./live555/groupsock/include -I./live555/UsageEnvironment/include

LDFLAGS := -lssl -lcrypto
LIVE555_LIB := ./live555/liveMedia/libliveMedia.a ./live555/groupsock/libgroupsock.a ./live555/BasicUsageEnvironment/libBasicUsageEnvironment.a ./live555/UsageEnvironment/libUsageEnvironment.a

all:streamer receiver

streamer:
	g++ -o streamer -L. streamer.cpp $(LIVE555_LIB) $(LDFLAGS) $(CFLAGS)

receiver:
	g++ -o receiver -L. receiver.cpp $(LIVE555_LIB) $(LDFLAGS) $(CFLAGS)

clean:
	rm -rf ./streamer ./receiver