#!smake

ROOT =
PROGRAM = SpaceCadet

include /usr/include/make/commondefs

C++FILES = Main.c++ Blaster.c++ Chamber.c++ HighScore.c++ \
	   Meter.c++ Mine.c++ Panel.c++ Ring.c++ Scene.c++ Setup.c++ Ship.c++ \
	   Sound.c++ Tunnel.c++ XStuff.c++
OPTIMIZER = -O

TARGETS = $(PROGRAM)

# Libraries to link with:
LLDLIBS =   -lInventorXt -laudio -laudiofile -lGL -lX11 -lm

default: $(TARGETS)

include $(COMMONRULES)

$(TARGETS): $(OBJECTS)
	$(C++F) -o $@ $(OBJECTS) $(LDFLAGS)
