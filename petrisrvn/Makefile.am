#----------------------------------------------------------------------
# $Id: Makefile.am 15299 2021-12-30 21:36:22Z greg $
#----------------------------------------------------------------------

SUBDIRS=	wspnlib regression
AM_CPPFLAGS =	-I../lqiolib/src/headers -I../lqx/libsrc/headers/
bin_PROGRAMS=	petrisrvn
petrisrvn_SOURCES = actlist.cc \
	activity.cc \
	entry.cc \
	errmsg.cc \
	makeobj.cc \
	model.cc \
	petrisrvn.cc \
	phase.cc \
	place.cc \
	pragma.cc \
	processor.cc \
	results.cc \
	runlqx.cc \
	task.cc

noinst_HEADERS=	activity.h \
	actlist.h \
	entry.h \
	errmsg.h \
	makeobj.h \
	model.h \
	petrisrvn.h \
	phase.h \
	place.h \
	pragma.h \
	processor.h \
	results.h \
	runlqx.h \
	task.h

EXTRA_DIST 	= petrisrvn.1

petrisrvn_CXXFLAGS  = -std=c++11 -Wall -Wno-parentheses -Wno-unknown-pragmas
petrisrvn_LDADD	    = -L../lqx/libsrc/.libs -llqx -L../lqiolib/src/.libs -llqio -Lwspnlib -lwspn
petrisrvn_LDFLAGS   = @STATIC@
man1_MANS	    = petrisrvn.1
