# -*- Makefile -*-
# $HeadURL: http://rads-svn.sce.carleton.ca:8080/svn/lqn/trunk-V5/parasol/src/Makefile.am $
# Makefile for Parasol libraries.
#
# ------------------------------------------------------------------------
# $Id: Makefile.am 11142 2012-10-06 16:22:24Z greg $
# ------------------------------------------------------------------------



AM_CPPFLAGS	= -I../include -I/home/lia/sysc/systemc-2.3.3/include -DSC_INCLUDE_DYNAMIC_PROCESSES
noinst_LIBRARIES =  libparasol.a libparasolc.a libparasol++.a

libparasol_a_SOURCES = para_library.cpp standalone.cpp 
EXTRA_libparasol_a_SOURCES = drand48.c
libparasol_a_DEPENDENCIES = @DRAND48@
libparasol_a_LIBADD = @DRAND48@ -lm /home/lia/sysc/systemc-2.3.3/lib-linux64/libsystemc.a

libparasolc_a_SOURCES = para_library.cpp test_storage.cpp
EXTRA_libparasolc_a_SOURCES = drand48.c
libparasolc_a_LIBADD = @DRAND48@ -lm /home/lia/sysc/systemc-2.3.3/lib-linux64/libsystemc.a
libparasolc_a_DEPENDENCIES = @DRAND48@


libparasol___a_SOURCES = para_library.cpp carrier.cc node.cc semaphore.cc statistic.cc spinlock.cc para_entity.cc \
	task.cc port.cc genesis.cc test_storage.cpp TaskSC.cpp Link.cpp NodeSC.cpp Bus.cpp QueuedPort.cpp bumber_library.cpp

