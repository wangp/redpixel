#
# Master makefile to get libnet, libcda, jgmod built before getting
# into building Red Pixel itself (see makefile.red).
#


# Platform detection.

ifdef DJDIR
PLATFORM := DJGPP
else
ifdef windir	# How do we detect Mingw properly?
PLATFORM := MINGW
else
ifneq ($(findstring Linux,$(shell uname)),)
PLATFORM := LINUX
endif
endif
endif



# default target.

all: libnet libcda jgmod agup thegame



# libnet.

LIBNET := libnet/lib/libnet.a

ifeq "$(PLATFORM)" "LINUX"
$(LIBNET):
	cp libnet/makfiles/linux.mak libnet/port.mak
	make -C libnet lib
endif

ifeq "$(PLATFORM)" "MINGW"
$(LIBNET):
	copy libnet\makfiles\mingw.mak libnet\port.mak
	make -C libnet lib
endif

ifeq "$(PLATFORM)" "DJGPP"
$(LIBNET):
	copy libnet\makfiles\djgpp.mak libnet\port.mak
	make -C libnet lib
endif

libnet: $(LIBNET)



# libcda.

LIBCDA := libcda/libcda.a

$(LIBCDA):
	make -C libcda libcda.a

libcda: $(LIBCDA)



# jgmod.

ifeq "$(PLATFORM)" "LINUX"
JGMOD_MAKEFILE := makefile.lnx
JGMOD_DIR := unix
endif

ifeq "$(PLATFORM)" "MINGW"
JGMOD_MAKEFILE := makefile.mgw
JGMOD_DIR := mingw32
endif

ifeq "$(PLATFORM)" "DJGPP"
JGMOD_MAKEFILE := makefile.dj
JGMOD_DIR := djgpp
endif

JGMOD := jgmod/lib/$(JGMOD_DIR)/libjgmod.a

$(JGMOD):
	make -C jgmod/src -f $(JGMOD_MAKEFILE) ../lib/$(JGMOD_DIR)/libjgmod.a

jgmod: $(JGMOD)



# agup.

AGUP := agup/agup.a

$(AGUP):
	make -C agup agup.a

agup: $(AGUP)



# Red Pixel.

include makefile.red

