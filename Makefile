all:streamer receiver

streamer:
	g++ -o streamer -L. streamer.cpp ./libliveMedia.a ./libgroupsock.a ./libBasicUsageEnvironment.a ./libUsageEnvironment.a -lssl -lcrypto

receiver:
	g++ -o receiver -L. receiver.cpp ./libliveMedia.a ./libgroupsock.a ./libBasicUsageEnvironment.a ./libUsageEnvironment.a -lssl -lcrypto

clean:
	rm -rf ./streamer ./receiver