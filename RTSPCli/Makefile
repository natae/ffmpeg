LIBS=liveMedia/libliveMedia.a groupsock/libgroupsock.a BasicUsageEnvironment/libBasicUsageEnvironment.a UsageEnvironment/libUsageEnvironment.a

COMPILE_OPT= -IUsageEnvironment/include -Igroupsock/include -IliveMedia/include -IBasicUsageEnvironment/include -I. -O2 -DSOCKLEN_T=socklen_t -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64 -Wall -DBSD=1

testRTSPClient.exe: testRTSPClient.o
	c++ -o testRTSPClient -L. testRTSPClient.o $(LIBS)
testRTSPClient.o: testRTSPClient.cpp
	c++ -c $(COMPILE_OPT) testRTSPClient.cpp


#g++ -o testRTSPClient -L. testRTSPClient.o ../liveMedia/libliveMedia.a ../groupsock/libgroupsock.a ../BasicUsageEnvironment/libBasicUsageEnvironment.a ../UsageEnvironment/libUsageEnvironment.a

#c++ -c -I../UsageEnvironment/include -I../groupsock/include -I../liveMedia/include -I../BasicUsageEnvironment/include -I. -O2 -DSOCKLEN_T=socklen_t -D_LARGEFILE_SOURCE=1 -D_FILE_OFFS ET_BITS=64 -Wall -DBSD=1 testRTSPClient.cpp
