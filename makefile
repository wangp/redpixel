#
# Master makefile to get libnet, libcda, jgmod built before getting
# into building Red Pixel itself (see makefile.red).
#


# Platform detection.

ifneq ($(findstring Linux,$(shell uname)),)
PLATFORM := LINUX
endif

ifdef MINGW
PLATFORM := MINGW
endif

ifdef DJDIR
PLATFORM := DJGPP
endif



# default target.

all: libnet libcda jgmod agup thegame



# libnet.

ifeq "$(PLATFORM)" "LINUX"
LIBNET_MAKEFILE := linux.mak
endif

ifeq "$(PLATFORM)" "MINGW"
LIBNET_MAKEFILE := mingw.mak
endif

ifeq "$(PLATFORM)" "DJGPP"
LIBNET_MAKEFILE := djgpp.mak
endif

LIBNET := libnet/lib/libnet.a

$(LIBNET):
	cp libnet/makfiles/$(LIBNET_MAKEFILE) libnet/port.mak
	make -C libnet lib

libnet: $(LIBNET)



# libcda.

LIBCDA := libcda/libcda.a

$(LIBCDA):
	make -C libcda libcda.a

libcda: $(LIBCDA)



# jgmod.

ifeq "$(PLATFORM)" "LINUX"

JGMOD := jgmod/lib/unix/libjgmod.a

$(JGMOD):
	make -C jgmod/src -f makefile.lnx ../lib/unix/libjgmod.a

endif

jgmod: $(JGMOD)



# agup.

ifeq "$(PLATFORM)" "LINUX"

AGUP := agup/agup.a

$(AGUP):
	make -C agup agup.a

endif

agup: $(AGUP)



# Red Pixel.

include makefile.red

