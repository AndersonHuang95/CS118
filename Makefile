CXX=g++
CXXOPTIMIZE= -O2
CXXFLAGS= -g -Wall -pthread -std=c++11 $(CXXOPTIMIZE)
USERID=504487373-304437322-104299133
CLASSES= HTTP-message.h HTTP-message.cpp 

all: web-server web-client

web-server: $(CLASSES) 
	$(CXX) -o $@ $^ $(CXXFLAGS) $@.cpp

web-client: $(CLASSES) 
	$(CXX) -o $@ $^ $(CXXFLAGS) $@.cpp 

clean:
	rm -rf *.o *~ *.gch *.swp *.dSYM web-server web-client *.tar.gz HTTP-message

tarball: clean
	tar -cvf $(USERID).tar.gz *
