




AM_CPPFLAGS = -I. -I/home/lia/sysc/systemc-2.3.3/include
bin_PROGRAMS = systemC_test
systemC_test_SOURCES = ClientExample.cpp main.cpp ServerExample.cpp SimpleQueue.cpp SimpleRouter.cpp
noinst_HEADERS = ClientExample.h PacketType.h QueuedType.h ServerExample.h SimpleQueue.h SimpleRouter.h


systemC_test_CXXFLAGS = -std=c++14 -Wall -fexceptions -O2
systemC_test_LDADD = -lm /home/lia/sysc/systemc-2.3.3/lib-linux64/libsystemc.a

clean-local:
distclean-local:
	rm -f config.cache


