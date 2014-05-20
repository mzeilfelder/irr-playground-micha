# Makefile for Irrlicht Examples - note that in this case we put the binary result beside the sources
# It's usually sufficient to change just the target name and source file list
# and be sure that CXX is set to a valid compiler
Target = playground
Sources = main.cpp 
ifdef FREETYPE
Sources += CGUITTFont.cpp
endif

# general compiler settings
CPPFLAGS = -I../../include -I/usr/X11R6/include
ifdef FREETYPE
CPPFLAGS += $(shell freetype-config --cflags)
endif
CXXFLAGS = -Wall -pipe -fno-exceptions -fno-rtti -fstrict-aliasing
ifndef NDEBUG
CXXFLAGS += -g -D_DEBUG
else
CXXFLAGS += -fexpensive-optimizations -O3
endif
ifdef PROFILE
CXXFLAGS += -pg
endif
CXXFLAGS += -std=c++11

#default target is Linux
all: all_linux

ifeq ($(HOSTTYPE), x86_64)
LIBSELECT=64
endif

# target specific settings
all_linux: 
LDFLAGS = -L/usr/X11R6/lib$(LIBSELECT) -L../../lib/Linux -lIrrlicht -lGL -lXxf86vm -lXext -lX11 -lXcursor -lXrandr 
#LDFLAGS += -lSDL
ifdef FREETYPE
	LDFLAGS += -lfreetype
endif
all_linux clean_linux: SYSTEM=Linux
all_win32: LDFLAGS = -L../../lib/Win32-gcc -lIrrlicht -lopengl32 -lm
all_win32 clean_win32: SYSTEM=Win32-gcc
all_win32 clean_win32: SUF=.exe

# name of the binary - only valid for targets which set SYSTEM
#DESTPATH = ../../bin/$(SYSTEM)/$(Target)$(SUF)
DESTPATH = ./$(Target)$(SUF)

all_linux all_win32:
	$(warning Building...)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(Sources) -o $(DESTPATH) $(LDFLAGS)

clean: clean_linux clean_win32
	$(warning Cleaning...)

clean_linux clean_win32:
	@$(RM) $(DESTPATH)
	
.PHONY: all all_win32 clean clean_linux clean_win32 
