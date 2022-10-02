# Makefile for Irrlicht Examples.
# Note that in this case we put the binary result beside the sources
# In some cases you might want to pass additional variables to the Makefile, like:
# make FREETYPE=1 ES2=1 GRID=1 SDL=1
Target = playground
Sources = main.cpp 
ifdef FREETYPE
Sources += CGUITTFont.cpp
endif
ifdef GRID
Sources += CGridSceneNode.cpp
endif

# general compiler settings
CPPFLAGS = -I../../include -I/usr/X11R6/include
ifdef FREETYPE
#Older debian versions
#CPPFLAGS += $(shell freetype-config --cflags)
#Newer debian versions
CPPFLAGS += $(shell pkg-config freetype2 --cflags)
endif
CXXFLAGS = -Wall -pipe -fno-exceptions -fno-rtti -fstrict-aliasing
#CXXFLAGS = -Wall -pipe -fno-exceptions -fstrict-aliasing
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
LDFLAGS = -L/usr/X11R6/lib$(LIBSELECT) -L../../lib/Linux -lIrrlicht -lXxf86vm -lXext -lX11 -lXcursor -lXrandr
ifdef ES2
	LDFLAGS += -lGLESv2 -lEGL
endif
ifdef SDL
	LDFLAGS += -lSDL
endif
LDFLAGS += -lGL
#LDFLAGS += -lSDL
ifdef FREETYPE
	#Older debian versions
	#LDFLAGS += -lfreetype
	#Newer debian versions
	LDFLAGS += $(shell pkg-config freetype2 --libs)
endif
# LDFLAGS += `pkg-config gtkmm-3.0 --cflags --libs`
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
