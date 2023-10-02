#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"

Boolean const isSSM = False;

UsageEnvironment* env;

struct sessionState_t {
    FramedSource* source;
    RTPSink* sink;
    RTCPInstance* rtcpInstance;
    Groupsock* rtpGroupsock;
    Groupsock* rtcpGroupsock;
} sessionState;

char const* inputFileName = "test.mp3";

void play();

void afterPlaying(void*)
{
    *env << "...done streaming\n";

    Medium::close(sessionState.source);
    play();
}

void play()
{
    sessionState.source = MP3FileSource::createNew(*env, inputFileName);
    if (sessionState.source == NULL) {
        *env << "Unable to open file \"" << inputFileName << "\" as a MP3 file source\n";
        exit(1);
    }

    *env << "Beginning streaming...\n";
    sessionState.sink->startPlaying(*sessionState.source, afterPlaying, NULL);
}

int main()
{
    TaskScheduler* scheduler = BasicTaskScheduler::createNew();
    env = BasicUsageEnvironment::createNew(*scheduler);

    char const* destinationAddressStr = "239.255.42.42";
    const unsigned short rtpPortNum = 6666;
    const unsigned short rtcpPortNum = rtpPortNum + 1;
    const unsigned char ttl = 1;

    NetAddressList destinationAddresses(destinationAddressStr);
    struct sockaddr_storage destinationAddress;
    copyAddress(destinationAddress, destinationAddresses.firstAddress());

    const Port rtpPort(rtpPortNum);
    const Port rtcpPort(rtcpPortNum);

    sessionState.rtpGroupsock 
        = new Groupsock(*env, destinationAddress, rtpPort, ttl);
    sessionState.rtcpGroupsock
        = new Groupsock(*env, destinationAddress, rtcpPort, ttl);

    // Create a 'MP3 RTP' sink from the RTP 'groupsock':
    sessionState.sink = MPEG1or2AudioRTPSink::createNew(*env, sessionState.rtpGroupsock);

    // Create (and start) a 'RTCP instance' for this RTP sink:
    const unsigned estimatedSessionBandwidth = 160*10;  // in kbps;
    const unsigned maxCNAMElen = 100;
    unsigned char CNAME[maxCNAMElen + 1];
    gethostname((char*)CNAME, maxCNAMElen);
    CNAME[maxCNAMElen] = '\0';
    sessionState.rtcpInstance = RTCPInstance::createNew(*env, sessionState.rtcpGroupsock,
                                                        estimatedSessionBandwidth, CNAME,
                                                        sessionState.sink, NULL,
                                                        isSSM);

    play();
    env->taskScheduler().doEventLoop();
    return 0;
}