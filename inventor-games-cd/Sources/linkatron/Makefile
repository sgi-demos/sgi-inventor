#!smake

include /usr/include/make/commondefs

C++FILES = linkatron.c++ WorldInfo.c++ TableTop.c++ GraphPaper.c++ \
	   MechPiece.c++ Axle.c++ LinkClasses.c++ LinkEngines.c++  Shadow.c++  \
	   Motors.c++ ButtonStrip.c++ Buttons.c++ Sound.c++

HFILES = WorldInfo.h LinkClasses.h LinkEngines.h TableTop.h GraphPaper.h  \
	 linkatronMenus.h Shadow.h ButtonStrip.h Buttons.h \
	 Motors.h MechPiece.h Axle.h Sound.h

PROGRAM = linkatron

TARGETS = $(PROGRAM)

# Libraries to link with:
LLDLIBS =  -lInventorXt -lGL -lm -laudio -laudiofile

LC++DEFS =

default: $(TARGETS)

include $(COMMONRULES)

$(TARGETS): $(OBJECTS)
	$(C++F) -o $@ $(OBJECTS) $(LDFLAGS)

