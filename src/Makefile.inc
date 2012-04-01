CXX	:= g++
CFLAGS 	 = -Wall -fPIC -I$(INCLUDEDIR)
CDEBUG 	:= -ggdb3 -DDEBUG
COPT 	:= -O3

AR 	:= ar crs
MAKE 	:= make
#This variable is necessary in some machines so that bash expansion in real_clean is conducted properly
SHELL 	:= /bin/bash

ifndef $(PREFIX)
	PREFIX := $(abspath $(shell pwd)/../..)
endif

INCLUDEDIR = $(PREFIX)/src

DBG 	:= dbg
OPT 	:= opt

ifeq ($(shell uname), Linux)
	LIBATLAS = /usr/lib/atlas
else
	LIBATLAS = /Developer/SDKs/MacOSX10.5.sdk/usr/lib
	HEADERCBLAS = /Developer/SDKs/MacOSX10.5.sdk/System/Library/Frameworks/vecLib.framework/Versions/A/Headers/
endif

LIBINFL	:= inflio #inflla
LIBDIR 	:= $(PREFIX)/lib
LDFLAGS :=  -L$(LIBDIR) -lgzstream #-L$(LIBATLAS) -llapack -lcblas -latlas -lz

# ifeq ($(shell uname), Linux)
# 	LDFLAGS := $(LDFLAGS) -lgfortran
# else
# 	CFLAGS := $(CFLAGS) -I$(HEADERCBLAS)
# endif

ifeq ($(shell uname), Linux)
	SHARED := $(LIBDIR)/$(LIBNAME).so
else
	SHARED := $(LIBDIR)/$(LIBNAME).dylib
endif

ifeq ($(MAKECMDGOALS),opt)
	OBJ := $(OPT_OBJ)
	DYLIBFLAGS := $(CFLAGS) $(COPT)
else
	OBJ := $(DBG_OBJ)
	DYLIBFLAGS := $(CFLAGS) $(CDEBUG)
endif

.SUFFIXES:
.SUFFIXES: .h .o .cpp
