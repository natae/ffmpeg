AVFORMAT = -I/usr/local/include -L/usr/local/lib -L/usr/local/Cellar/opus/1.1/lib -L/usr/local/Cellar/freetype/2.5.2/lib -L/usr/local/Cellar/libass/0.10.2/lib -L/usr/local/lib -L/usr/local/Cellar/opus/1.1/lib -L/usr/local/Cellar/freetype/2.5.2/lib -L/usr/local/Cellar/libass/0.10.2/lib -L/usr/local/lib -lavformat -liconv -lSDLmain -lSDL -Wl,-framework,Cocoa -lxvidcore -lx264 -lvpx -lvorbisenc -lvorbis -logg -ltheoraenc -ltheoradec -logg -lopus -lmp3lame -lfreetype -lfdk-aac -lass -lm -lbz2 -lz -pthread -lavcodec -liconv -lSDLmain -lSDL -Wl,-framework,Cocoa -lxvidcore -lx264 -lvpx -lvorbisenc -lvorbis -logg -ltheoraenc -ltheoradec -logg -lopus -lmp3lame -lfreetype -lfdk-aac -lass -lm -lbz2 -lz -pthread -lavutil -lm 
SDL = -I/usr/local/include/SDL -D_GNU_SOURCE=1 -D_THREAD_SAFE -L/usr/local/lib -lSDLmain -lSDL -Wl,-framework,Cocoa
SWSCALE = -I/usr/local/include -L/usr/local/lib -lswscale -lm -lavutil -lm
AVCODEC = -I/usr/local/include -L/usr/local/lib -L/usr/local/Cellar/opus/1.1/lib -L/usr/local/Cellar/freetype/2.5.2/lib -L/usr/local/Cellar/libass/0.10.2/lib -L/usr/local/lib -lavcodec -liconv -lSDLmain -lSDL -Wl,-framework,Cocoa -lxvidcore -lx264 -lvpx -lvorbisenc -lvorbis -logg -ltheoraenc -ltheoradec -logg -lopus -lmp3lame -lfreetype -lfdk-aac -lass -lm -lbz2 -lz -pthread -lavutil -lm 
OPTION = -o test
test: test.cpp
	g++ test.cpp $(OPTION) $(AVCODEC) $(AVFORMAT) $(SWSCALE) $(SDL)

