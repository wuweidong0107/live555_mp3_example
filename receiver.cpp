#include "liveMedia.hh"
#include "GroupsockHelper.hh"
#include "BasicUsageEnvironment.hh"

void afterPlaying(void* clientData); 

struct sessionState_t {
    FramedSource* source;
    FileSink* sink;
    RTCPInstance* rtcpInstance;
} sessionState;

UsageEnvironment* env;

int main()
{
    TaskScheduler* scheduler = BasicTaskScheduler::createNew();
    env = BasicUsageEnvironment::createNew(*scheduler);

    // Create the data sink for 'stdout':
    sessionState.sink = FileSink::createNew(*env, "stdout");

    // Create 'groupsocks' for RTP and RTCP:
    char const* sessionAddressStr = "239.255.42.42";

    const unsigned short rtpPortNum = 6666;
    const unsigned short rtcpPortNum = rtpPortNum + 1;
    const unsigned char ttl = 1;

    NetAddressList sessionAddresses(sessionAddressStr);
    struct sockaddr_storage sessionAddress;
    copyAddress(sessionAddress, sessionAddresses.firstAddress());

    const Port rtpPort(rtpPortNum);
    const Port rtcpPort(rtcpPortNum);

    Groupsock rtpGroupsock(*env, sessionAddress, rtpPort, ttl);
    Groupsock rtcpGroupsock(*env, sessionAddress, rtcpPort, ttl);

    // Create the data source: a "MP3 *ADU* RTP source"
    RTPSource* rtpSource;
    rtpSource = MPEG1or2AudioRTPSource::createNew(*env, &rtpGroupsock);

    // Create (and start) a 'RTCP instance' for the RTP source:
    const unsigned estimatedSessionBandwidth = 160*10;
    const unsigned maxCNAMElen = 100;
    unsigned char CNAME[maxCNAMElen+1];
    gethostname((char*)CNAME, maxCNAMElen);
    CNAME[maxCNAMElen] = '\0';
    sessionState.rtcpInstance = RTCPInstance::createNew(*env, &rtcpGroupsock,
                                                estimatedSessionBandwidth, CNAME,
                                                NULL, rtpSource);
    sessionState.source = rtpSource;
    *env << "Beginning receiving multicast stream...\n";

    sessionState.sink->startPlaying(*sessionState.source, afterPlaying, NULL);

    env->taskScheduler().doEventLoop();
    return 0;
}

void afterPlaying(void*)
{
    *env << "..done receiving\n";

    Medium::close(sessionState.rtcpInstance);
    Medium::close(sessionState.sink);
    Medium::close(sessionState.source);
}