PREFIX ?= /usr/local
CXX ?= c++
CXXFLAGS += -fpic -lbearssl -s -Os -flto=full -w -Wfatal-errors -std=c++20
OBJECT = src/uri.o src/utils.o src/clear_url.o src/unshort_url.o
SRC = src/uri.cpp src/utils.cpp src/clear_url.cpp src/unshort_url.cpp
SHARED_LIBRARY = src/libunalix.so
HEADERS = src/unalix.hpp src/clear_url.hpp src/unshort_url.hpp

all : $(OBJECT)
	$(CXX) $(CXXFLAGS) -shared $(OBJECT) -o $(SHARED_LIBRARY)

utils.o : utils.hpp
uri.o : uri.hpp
clear_url.o : clear_url.hpp
unshort_url.o : unshort_url.hpp

main.o : clear_url.cpp utils.hpp uri.hpp

.PHONY:	clean

clean:
	rm --force $(OBJECT) $(SHARED_LIBRARY) 

install: $(SHARED_LIBRARY)
	! [ -d $(PREFIX)/lib ] && mkdir --parent $(PREFIX)/lib || true
	! [ -d $(PREFIX)/include/unalix ] && mkdir --parent $(PREFIX)/include/unalix || true
	install $(SHARED_LIBRARY) --mode=600 $(PREFIX)/lib/libunalix.so
	install $(HEADERS) --mode=600 $(PREFIX)/include/unalix