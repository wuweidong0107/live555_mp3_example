CFLAGS := -Wall -O2 -g
CFLAGS += -I$(shell pwd)/ -I./live555/BasicUsageEnvironment/include -I./live555/liveMedia/include -I./live555/groupsock/include -I./live555/UsageEnvironment/include

LDFLAGS := -lssl -lcrypto
LIVE555_LIB := ./live555/liveMedia/libliveMedia.a ./live555/groupsock/libgroupsock.a ./live555/BasicUsageEnvironment/libBasicUsageEnvironment.a ./live555/UsageEnvironment/libUsageEnvironment.a

all:live555 streamer receiver 

streamer: streamer.cpp $(LIVE555_LIB)
	g++ -o streamer -L. streamer.cpp $(LIVE555_LIB) $(LDFLAGS) $(CFLAGS)

receiver: receiver.cpp $(LIVE555_LIB)
	g++ -o receiver -L. receiver.cpp $(LIVE555_LIB) $(LDFLAGS) $(CFLAGS)

live555: FORCE
	cd live555/ && ./genMakefiles linux && make -j16
	
FORCE:
.PHONY: FORCE

clean:
	rm -rf ./streamer ./receiver
